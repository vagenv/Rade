// Copyright 2015-2023 Vagen Ayrapetyan

#include "RStatusMgrComponent.h"
#include "RWorldStatusMgr.h"
#include "RActiveStatusEffect.h"

#include "RUtilLib/RUtil.h"
#include "RUtilLib/RLog.h"
#include "RUtilLib/RCheck.h"
#include "RUtilLib/RTimer.h"
#include "RUtilLib/RWorldAssetMgr.h"

#include "RDamageLib/RDamageTypes.h"
#include "RDamageLib/RWorldDamageMgr.h"
#include "RExperienceLib/RExperienceMgrComponent.h"

#include "Net/UnrealNetwork.h"

//=============================================================================
//                 Core
//=============================================================================

URStatusMgrComponent::URStatusMgrComponent ()
{
   PrimaryComponentTick.bCanEverTick = true;
   PrimaryComponentTick.bStartWithTickEnabled = true;
   SetIsReplicatedByDefault (true);

   Health.Current =  90.;
   Health.Max     = 100.;
   Health.Regen   =   2.;

   Stamina.Current =  50.;
   Stamina.Max     = 100.;
   Stamina.Regen   =  20.;

   Mana.Current = 30.;
   Mana.Max     = 50.;
   Mana.Regen   =  5.;
}

void URStatusMgrComponent::GetLifetimeReplicatedProps (TArray<FLifetimeProperty> &OutLifetimeProps) const
{
   Super::GetLifetimeReplicatedProps (OutLifetimeProps);

   DOREPLIFETIME (URStatusMgrComponent, bDead);
   DOREPLIFETIME (URStatusMgrComponent, PassiveEffects);
   DOREPLIFETIME (URStatusMgrComponent, Resistance);
}

//=============================================================================
//                 Core
//=============================================================================

void URStatusMgrComponent::BeginPlay ()
{
   Super::BeginPlay ();

   // --- Create a backup of initial value
   Start_Health  = Health;
   Start_Mana    = Mana;
   Start_Stamina = Stamina;

   // Character Movement
   if (ACharacter *Character = Cast<ACharacter> (GetOwner ())) {
      MovementComponent = Character->GetCharacterMovement ();
   }

   if (R_IS_NET_ADMIN) {
      bDead = false;

      // Bind To AActor::OnTakeAnyDamage
      GetOwner ()->OnTakeAnyDamage.AddDynamic (this, &URStatusMgrComponent::AnyDamage);
   }

   FindWorldMgrs ();
}

void URStatusMgrComponent::EndPlay (const EEndPlayReason::Type EndPlayReason)
{
   Super::EndPlay (EndPlayReason);
}

void URStatusMgrComponent::TickComponent (float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
   Super::TickComponent (DeltaTime, TickType, ThisTickFunction);
   StatusRegen (DeltaTime);
}

void URStatusMgrComponent::FindWorldMgrs ()
{
   if (!WorldStatusMgr) {
      WorldStatusMgr = URWorldStatusMgr::GetInstance (this);
   }

   if (!WorldDamageMgr) {
      WorldDamageMgr = URWorldDamageMgr::GetInstance (this);
   }

   if (!WorldStatusMgr || !WorldDamageMgr) {
      FTimerHandle RetryHandle;
      RTIMER_START (RetryHandle,
                    this, &URStatusMgrComponent::FindWorldMgrs,
                    1, false);
   }
}

//=============================================================================
//                 Dead
//=============================================================================

void URStatusMgrComponent::SetDead (bool Dead)
{
   R_RETURN_IF_NOT_ADMIN;
   if (Dead == bDead) return;
   bDead = Dead;

   // Broadcast only after value has been changed;
   if (!Dead) {
      // --- Reset to original values
      Health  = Start_Health;
      Mana    = Start_Mana;
      Stamina = Start_Stamina;

      ReportRevive ();
      if (WorldDamageMgr) WorldDamageMgr->ReportRevive (GetOwner ());
   }

   if (Dead) {

      // Stop active effects
      TArray<URActiveStatusEffect*> StillActiveEffects;
      GetOwner ()->GetComponents (StillActiveEffects);
      for (URActiveStatusEffect* ItEffect : StillActiveEffects) {
         ItEffect->Stop ();
      }

      Health.Current = 0;
      ReportDeath ();
   }
}

bool URStatusMgrComponent::IsDead () const
{
   return bDead;
}

void URStatusMgrComponent::OnRep_Dead ()
{
   if (!R_IS_VALID_WORLD) return;
   if (bDead) {
      Health.Current = 0;
      ReportDeath ();
   } else {
      // --- Reset to original values
      Health  = Start_Health;
      Mana    = Start_Mana;
      Stamina = Start_Stamina;
      ReportRevive ();
   }
}

void URStatusMgrComponent::ReportDeath ()
{
   if (R_IS_VALID_WORLD && OnDeath.IsBound ()) OnDeath.Broadcast ();
}

void URStatusMgrComponent::ReportRevive ()
{
   if (R_IS_VALID_WORLD && OnRevive.IsBound ()) OnRevive.Broadcast ();
}

//=============================================================================
//                 Recalc stats
//=============================================================================

void URStatusMgrComponent::RecalcStatus ()
{
   R_RETURN_IF_NOT_ADMIN;
   RecalcStatusValues ();

   SetHealth  (Health);
   SetStamina (Stamina);
   SetMana    (Mana);
}

void URStatusMgrComponent::RecalcStatusValues ()
{
   R_RETURN_IF_NOT_ADMIN;
   if (!WorldStatusMgr) return;

   // --- Status
   Health.Max    = Start_Health.Max;
   Health.Regen  = Start_Health.Regen;
   Mana.Max      = Start_Mana.Max;
   Mana.Regen    = Start_Mana.Regen;
   Stamina.Max   = Start_Stamina.Max;
   Stamina.Regen = Start_Stamina.Regen;

   // Flat
   for (const FRPassiveStatusEffectWithTag &ItEffect : GetPassiveEffectsWithTag ()) {
      if (ItEffect.Value.EffectTarget == ERStatusEffectTarget::HealthMax)    Health.Max    += ItEffect.Value.Flat;
      if (ItEffect.Value.EffectTarget == ERStatusEffectTarget::HealthRegen)  Health.Regen  += ItEffect.Value.Flat;
      if (ItEffect.Value.EffectTarget == ERStatusEffectTarget::StaminaMax)   Stamina.Max   += ItEffect.Value.Flat;
      if (ItEffect.Value.EffectTarget == ERStatusEffectTarget::StaminaRegen) Stamina.Regen += ItEffect.Value.Flat;
      if (ItEffect.Value.EffectTarget == ERStatusEffectTarget::ManaMax)      Mana.Max      += ItEffect.Value.Flat;
      if (ItEffect.Value.EffectTarget == ERStatusEffectTarget::ManaRegen)    Mana.Regen    += ItEffect.Value.Flat;
   }
   // Percentage
   for (const FRPassiveStatusEffectWithTag &ItEffect : GetPassiveEffectsWithTag ()) {
      if (ItEffect.Value.EffectTarget == ERStatusEffectTarget::HealthMax)    Health.Max    *= ((100. + ItEffect.Value.Percent) / 100.);
      if (ItEffect.Value.EffectTarget == ERStatusEffectTarget::HealthRegen)  Health.Regen  *= ((100. + ItEffect.Value.Percent) / 100.);
      if (ItEffect.Value.EffectTarget == ERStatusEffectTarget::StaminaMax)   Stamina.Max   *= ((100. + ItEffect.Value.Percent) / 100.);
      if (ItEffect.Value.EffectTarget == ERStatusEffectTarget::StaminaRegen) Stamina.Regen *= ((100. + ItEffect.Value.Percent) / 100.);
      if (ItEffect.Value.EffectTarget == ERStatusEffectTarget::ManaMax)      Mana.Max      *= ((100. + ItEffect.Value.Percent) / 100.);
      if (ItEffect.Value.EffectTarget == ERStatusEffectTarget::ManaRegen)    Mana.Regen    *= ((100. + ItEffect.Value.Percent) / 100.);
   }

   // Clamp current values;
   Health.Current  = FMath::Clamp (Health.Current,  0, Health.Max);
   Mana.Current    = FMath::Clamp (Mana.Current,    0, Mana.Max);
   Stamina.Current = FMath::Clamp (Stamina.Current, 0, Stamina.Max);
}

//=============================================================================
//                 Status
//=============================================================================

void URStatusMgrComponent::StatusRegen (float DeltaTime)
{
   if (IsDead ()) return;
   Health.Tick (DeltaTime);
   Mana.Tick (DeltaTime);

   if (MovementComponent && MovementComponent->IsMovingOnGround ()) Stamina.Tick (DeltaTime);
}

FRStatusValue URStatusMgrComponent::GetHealth () const
{
   return Health;
}

void URStatusMgrComponent::UseHealth (float Amount)
{
   Health.Current = FMath::Clamp (Health.Current - Amount, 0, Health.Max);
   if (R_IS_NET_ADMIN) SetHealth (Health);
}

FRStatusValue URStatusMgrComponent::GetStamina () const
{
   return Stamina;
}

void URStatusMgrComponent::UseStamina (float Amount)
{
   Stamina.Current = FMath::Clamp (Stamina.Current - Amount, 0, Stamina.Max);
   if (R_IS_NET_ADMIN) SetStamina (Stamina);
}

FRStatusValue URStatusMgrComponent::GetMana () const
{
   return Mana;
}

void URStatusMgrComponent::UseMana (float Amount)
{
   Mana.Current = FMath::Clamp (Mana.Current - Amount, 0, Mana.Max);
   if (R_IS_NET_ADMIN) SetMana (Mana);
}

void URStatusMgrComponent::SetHealth_Implementation (FRStatusValue Health_)
{
   if (R_IS_NET_ADMIN) return;
   Health = Health_;
}

void URStatusMgrComponent::SetMana_Implementation (FRStatusValue Mana_)
{
   if (R_IS_NET_ADMIN) return;
   Mana = Mana_;
}

void URStatusMgrComponent::SetStamina_Implementation (FRStatusValue Stamina_)
{
   if (R_IS_NET_ADMIN) return;
   Stamina = Stamina_;
}

//=============================================================================
//                 Critical / Evasion
//=============================================================================

bool URStatusMgrComponent::RollCritical () const
{
   return ((FMath::Rand () % 100) <= CriticalChance);
}

bool URStatusMgrComponent::RollEvasion () const
{
   return ((FMath::Rand () % 100) <= EvasionChance);
}

//==========================================================================
//                 Passive Effect
//==========================================================================

TArray<FRPassiveStatusEffect> URStatusMgrComponent::GetPassiveEffects () const
{
   return URPassiveStatusEffectUtilLibrary::MergeEffects (PassiveEffects);
}

TArray<FRPassiveStatusEffectWithTag> URStatusMgrComponent::GetPassiveEffectsWithTag () const
{
   return PassiveEffects;
}

bool URStatusMgrComponent::SetPassiveEffects (const FString &Tag, const TArray<FRPassiveStatusEffect> &AddValues)
{
   R_RETURN_IF_NOT_ADMIN_BOOL;
   if (!ensure (!Tag.IsEmpty ())) return false;
   // Clean
   RmPassiveEffects (Tag);

   // Add again
   for (const FRPassiveStatusEffect& ItAddValue : AddValues) {
      FRPassiveStatusEffectWithTag newRes;
      newRes.Tag   = Tag;
      newRes.Value = ItAddValue;
      PassiveEffects.Add (newRes);
   }

   RecalcStatus ();
   ReporPassiveEffectsUpdated ();
   return true;
}

bool URStatusMgrComponent::RmPassiveEffects (const FString &Tag)
{
   R_RETURN_IF_NOT_ADMIN_BOOL;
   if (!ensure (!Tag.IsEmpty ())) return false;
   TArray<int32> ToRemove;
   for (int32 iEffect = 0; iEffect < PassiveEffects.Num (); iEffect++) {
      const FRPassiveStatusEffectWithTag& ItEffect = PassiveEffects[iEffect];
      if (ItEffect.Tag == Tag) ToRemove.Add (iEffect);
   }
   // Nothing to remove
   if (!ToRemove.Num ()) return false;

   // Remove in reverse order;
   for (int32 iToRemove = ToRemove.Num () - 1; iToRemove >= 0; iToRemove--) {
      PassiveEffects.RemoveAt (ToRemove[iToRemove]);
   }

   RecalcStatus ();
   ReporPassiveEffectsUpdated ();
   return true;
}

void URStatusMgrComponent::OnRep_PassiveEffects ()
{
   ReporPassiveEffectsUpdated ();
}

void URStatusMgrComponent::ReporPassiveEffectsUpdated ()
{
   if (R_IS_VALID_WORLD && OnPassiveEffectsUpdated.IsBound ()) OnPassiveEffectsUpdated.Broadcast ();
}

//==========================================================================
//                 Active Effect
//==========================================================================


bool URStatusMgrComponent::ApplyActiveStatusEffect (
   AActor* Causer_,
   AActor* Target_,
   const TSoftClassPtr<URActiveStatusEffect> Effect_)
{
   // --- Check Values
   if (!ensure (Causer_)) return false;
   if (!ensure (Target_)) return false;
   if (!ensure (!Effect_.IsNull ())) return false;

   UWorld* World = URUtil::GetWorld (Target_);
   if (!World) return false;
   URStatusMgrComponent* StatusMgr = URUtil::GetComponent<URStatusMgrComponent> (Target_);
   if (!ensure (StatusMgr)) return false;

   return StatusMgr->AddActiveStatusEffect (Causer_, Effect_);
}


bool URStatusMgrComponent::AddActiveStatusEffect (
   AActor* Causer_,
   const TSoftClassPtr<URActiveStatusEffect> Effect_)
{
   R_RETURN_IF_NOT_ADMIN_BOOL;
   if (!ensure (Causer_)) return false;
   if (!ensure (!Effect_.IsNull ())) return false;
   if (IsDead ())                    return false;

   TArray<URActiveStatusEffect*> CurrentEffects;
   GetOwner()->GetComponents (CurrentEffects);
   for (URActiveStatusEffect* ItActiveEffect : CurrentEffects) {
      if (!IsValid (ItActiveEffect)) continue;
      if (ItActiveEffect->GetClass () == Effect_) {
         ItActiveEffect->Causer = Causer_;
         ItActiveEffect->Refresh ();
         return true;
      }
   }

   URWorldAssetMgr* AssetMgr = URWorldAssetMgr::GetInstance (this);
   if (!AssetMgr) return false;

   // Check that async load is not already in process
   if (EffectLoadHandle.IsValid ()) return false;

   EffectLoadHandle = AssetMgr->StreamableManager.RequestAsyncLoad (Effect_.GetUniqueID (),
      [this, Causer_] () {
         if (!EffectLoadHandle.IsValid ()) return;
         if (EffectLoadHandle->HasLoadCompleted ()) {
            if (UObject* Obj = EffectLoadHandle->GetLoadedAsset ()) {
               if (UClass* EffectClass = Cast<UClass> (Obj)) {
                  URActiveStatusEffect* Effect = URUtil::AddComponent<URActiveStatusEffect> (GetOwner (), EffectClass);
                  if (Effect) Effect->Causer = Causer_;
               }
            }
         }

         // Release data from memory
         EffectLoadHandle->ReleaseHandle ();
         EffectLoadHandle.Reset ();
      });

   return true;
}

void URStatusMgrComponent::ReportActiveEffectsUpdated ()
{
   if (R_IS_VALID_WORLD && OnActiveEffectsUpdated.IsBound ()) OnActiveEffectsUpdated.Broadcast ();
}

//=============================================================================
//                 Resistance
//=============================================================================

TArray<FRDamageResistance> URStatusMgrComponent::GetResistance () const
{
   return URDamageUtilLibrary::MergeResistance (Resistance);
}

TArray<FRDamageResistanceWithTag> URStatusMgrComponent::GetResistanceWithTag () const
{
   return Resistance;
}

FRDamageResistance URStatusMgrComponent::GetResistanceFor (TSubclassOf<UDamageType> const DamageType) const
{
   FRDamageResistance Result;
   if (!ensure (DamageType)) return Result;
   for (const FRDamageResistanceWithTag& ItResistance : Resistance) {
      if (!ItResistance.Value.DamageType) continue;
      if (DamageType->GetPathName () == ItResistance.Value.DamageType.ToString ()) {
         Result.Flat    += ItResistance.Value.Flat;
         Result.Percent += ItResistance.Value.Percent;
      }
   }
   return Result;
}

void URStatusMgrComponent::AddResistance (const FString &Tag, const TArray<FRDamageResistance> &AddValues)
{
   R_RETURN_IF_NOT_ADMIN;
   if (!ensure (!Tag.IsEmpty ()))  return;

   // Clean
   RmResistance (Tag);

   // Add again
   for (const FRDamageResistance& ItAddValue : AddValues) {
      FRDamageResistanceWithTag newRes;
      newRes.Tag   = Tag;
      newRes.Value = ItAddValue;
      Resistance.Add (newRes);
   }
   ReportResistanceUpdated ();
}

void URStatusMgrComponent::RmResistance (const FString &Tag)
{
   R_RETURN_IF_NOT_ADMIN;
   if (!ensure (!Tag.IsEmpty ())) return;

   TArray<int32> ToRemove;
   for (int32 iResistance = 0; iResistance < Resistance.Num (); iResistance++) {
      const FRDamageResistanceWithTag& ItResistance = Resistance[iResistance];
      if (ItResistance.Tag == Tag) ToRemove.Add (iResistance);
   }
   // Nothing to remove
   if (!ToRemove.Num ()) return;

   // Remove in reverse order;
   for (int32 iToRemove = ToRemove.Num () - 1; iToRemove >= 0; iToRemove--) {
      Resistance.RemoveAt (ToRemove[iToRemove]);
   }
   ReportResistanceUpdated ();
}

void URStatusMgrComponent::OnRep_Resistance ()
{
   ReportResistanceUpdated ();
}

void URStatusMgrComponent::ReportResistanceUpdated ()
{
   if (R_IS_VALID_WORLD && OnResistanceUpdated.IsBound ()) OnResistanceUpdated.Broadcast ();
}

//=============================================================================
//                 TakeDamage Events
//=============================================================================

void URStatusMgrComponent::AnyDamage (AActor*            Target,
                                      float              Amount,
                                      const UDamageType* Type_,
                                      AController*       InstigatedBy,
                                      AActor*            Causer)
{
   R_RETURN_IF_NOT_ADMIN;
   if (!ensure (Target)) return;
   if (!ensure (Type_))  return;
   if (!ensure (Causer)) return;

   if (!IsDead ()) {
      const URDamageType* Type = Cast<URDamageType>(Type_);
      // Check if RDamage was applied
      if (ensure (Type)) {
         // Check if evaded
         if (Type->Evadeable && RollEvasion ()) {
            ReportREvade (Amount, Type, Causer);
            return;
         }
         ReportRDamage (Amount, Type, Causer);
      }
   }
}

void URStatusMgrComponent::ReportRDamage_Implementation (float               Amount,
                                                         const URDamageType* Type,
                                                         AActor*             Causer)
{
   if (!ensure (Causer)) return;

   if (Type) {
      FRDamageResistance DamageResistance = GetResistanceFor (Type->GetClass ());

      Amount = Type->CalcDamage (Amount, DamageResistance);
      Type->BP_AnyDamage (GetOwner (), DamageResistance, Amount, Causer);
      //R_LOG_PRINTF ("Final Damage [%.1f] Resistance:[%1.f]", Amount, Resistance);
   }

   UseHealth (Amount);

   if (WorldDamageMgr) WorldDamageMgr->ReportDamage (GetOwner (), Amount, Type, Causer);

   if (!Health.Current) {
      if (R_IS_NET_ADMIN) SetDead (true);
      if (WorldDamageMgr) WorldDamageMgr->ReportDeath (GetOwner (), Causer, Type);
   }

   if (R_IS_VALID_WORLD && OnAnyRDamage.IsBound ()) OnAnyRDamage.Broadcast (Amount, Type, Causer);
}

void URStatusMgrComponent::ReportREvade_Implementation (float               Amount,
                                                        const URDamageType* Type,
                                                        AActor*             Causer)
{
   if (!ensure (Causer)) return;
   if (R_IS_VALID_WORLD && OnEvadeRDamage.IsBound ()) OnEvadeRDamage.Broadcast (Amount, Type, Causer);
}

