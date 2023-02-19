// Copyright 2015-2023 Vagen Ayrapetyan

#include "RStatusMgrComponent.h"
#include "RUtilLib/RLog.h"
#include "RUtilLib/RCheck.h"
#include "RSaveLib/RSaveMgr.h"

#include "Net/UnrealNetwork.h"
#include "Engine/DamageEvents.h"

#include "RStatusEffect.h"

//=============================================================================
//                 Core
//=============================================================================

URStatusMgrComponent::URStatusMgrComponent ()
{
   PrimaryComponentTick.bCanEverTick = true;
   PrimaryComponentTick.bStartWithTickEnabled = true;
   SetIsReplicatedByDefault (true);

   // Default
   CoreStats_Base = FRCoreStats (10);

   //==========================================================================
   //                 Status Value Curves
   //==========================================================================

   // Health MAX
   FRichCurve* StrToHealthMaxData = StrToHealthMax.GetRichCurve ();
   StrToHealthMaxData->AddKey (   0,  200); // Minimum
   StrToHealthMaxData->AddKey (   1,  210);
   StrToHealthMaxData->AddKey (  10,  300);
   StrToHealthMaxData->AddKey ( 100, 1000);
   StrToHealthMaxData->AddKey (1000, 5000);
   StrToHealthMaxData->AddKey (5000, 9999);

   // Health Regen
   FRichCurve* StrToHealthRegenData = StrToHealthRegen.GetRichCurve ();
   StrToHealthRegenData->AddKey (   0,   1  ); // Minimum
   StrToHealthRegenData->AddKey (   1,   1.1);
   StrToHealthRegenData->AddKey (  10,   2  );
   StrToHealthRegenData->AddKey ( 100,  10  );
   StrToHealthRegenData->AddKey (1000,  50  );
   StrToHealthRegenData->AddKey (5000, 100  );

   // Stamina MAX
   FRichCurve* AgiToStaminaMaxData = AgiToStaminaMax.GetRichCurve ();
   AgiToStaminaMaxData->AddKey (   0, 100); // Minimum
   AgiToStaminaMaxData->AddKey (   1, 102);
   AgiToStaminaMaxData->AddKey (  10, 120);
   AgiToStaminaMaxData->AddKey ( 100, 200);
   AgiToStaminaMaxData->AddKey (1000, 400);
   AgiToStaminaMaxData->AddKey (5000, 500);

   // Stamina Regen
   FRichCurve* AgiToStaminaRegenData = AgiToStaminaRegen.GetRichCurve ();
   AgiToStaminaRegenData->AddKey (   0,   10  ); // Minimum
   AgiToStaminaRegenData->AddKey (   1,   10.2);
   AgiToStaminaRegenData->AddKey (  10,   12  );
   AgiToStaminaRegenData->AddKey ( 100,   20  );
   AgiToStaminaRegenData->AddKey (1000,   40  );
   AgiToStaminaRegenData->AddKey (5000,   50  );

   // Mana MAX
   FRichCurve* IntToManaMaxData = IntToManaMax.GetRichCurve ();
   IntToManaMaxData->AddKey (   0,   50); // Minimum
   IntToManaMaxData->AddKey (   1,   60);
   IntToManaMaxData->AddKey (  10,  150);
   IntToManaMaxData->AddKey ( 100, 1000);
   IntToManaMaxData->AddKey (1000, 5000);
   IntToManaMaxData->AddKey (5000, 9999);

   // Mana Regen
   FRichCurve* IntToManaRegenData = IntToManaRegen.GetRichCurve ();
   IntToManaRegenData->AddKey (   0,   0  ); // Minimum
   IntToManaRegenData->AddKey (   1,   0.1);
   IntToManaRegenData->AddKey (  10,   1  );
   IntToManaRegenData->AddKey ( 100,  10  );
   IntToManaRegenData->AddKey (1000,  50  );
   IntToManaRegenData->AddKey (5000, 100  );

   //==========================================================================
   //                 Extra Stat Curves
   //==========================================================================

   // Critical
   FRichCurve* AgiToCriticalData = AgiToCritical.GetRichCurve ();
   AgiToCriticalData->AddKey (   0,   0); // Minimum
   AgiToCriticalData->AddKey (   1,   1);
   AgiToCriticalData->AddKey (  10,   5);
   AgiToCriticalData->AddKey ( 100,  20);
   AgiToCriticalData->AddKey (1000,  30);
   AgiToCriticalData->AddKey (5000,  40);

   // Evasion
   FRichCurve* AgiToEvasionData = AgiToEvasion.GetRichCurve ();
   AgiToEvasionData->AddKey (   0,    0); // Minimum
   AgiToEvasionData->AddKey (   1,    1);
   AgiToEvasionData->AddKey (  10,    5);
   AgiToEvasionData->AddKey ( 100,   20);
   AgiToEvasionData->AddKey (1000,   35);
   AgiToEvasionData->AddKey (5000,   40);

   // Attack Speed.
   // Per weapon curve for AttackSpeed -> AttacksPerSecond?
   // Min/Max for each weapon.
   FRichCurve* AgiToAttackSpeedData = AgiToAttackSpeed.GetRichCurve ();
   AgiToAttackSpeedData->AddKey (0,       0); // Minimum
   AgiToAttackSpeedData->AddKey (5000, 5000); // Maximum
}

// Replication
void URStatusMgrComponent::GetLifetimeReplicatedProps (TArray<FLifetimeProperty> &OutLifetimeProps) const
{
   Super::GetLifetimeReplicatedProps (OutLifetimeProps);

   // --- Status
   DOREPLIFETIME (URStatusMgrComponent, bDead);
   DOREPLIFETIME (URStatusMgrComponent, Health);
   DOREPLIFETIME (URStatusMgrComponent, Mana);
   DOREPLIFETIME (URStatusMgrComponent, Stamina);

   DOREPLIFETIME (URStatusMgrComponent, CoreStats_Base);
   DOREPLIFETIME (URStatusMgrComponent, CoreStats_Added);
   DOREPLIFETIME (URStatusMgrComponent, SubStats_Base);
   DOREPLIFETIME (URStatusMgrComponent, SubStats_Added);

   DOREPLIFETIME (URStatusMgrComponent, PassiveEffects);
   DOREPLIFETIME (URStatusMgrComponent, ActiveEffects);
   DOREPLIFETIME (URStatusMgrComponent, Resistence);
}

void URStatusMgrComponent::OnRep_Status ()
{
   OnStatusUpdated.Broadcast ();
}

//=============================================================================
//                 Core
//=============================================================================

void URStatusMgrComponent::BeginPlay ()
{
   Super::BeginPlay ();
   const UWorld *world = GetWorld ();
   if (!ensure (world)) return;

   if (ACharacter *Character = Cast<ACharacter> (GetOwner ())) {
      MovementComponent = Character->GetCharacterMovement ();
   }

   if (R_IS_NET_ADMIN) {
      bDead = false;
      bIsAdmin = true;

      RecalcStatus ();

      // Save/Load Current Status
      if (bSaveLoad) {
         FRSaveEvent SavedDelegate;
         SavedDelegate.AddDynamic (this, &URStatusMgrComponent::OnSave);
         URSaveMgr::OnSave (world, SavedDelegate);

         FRSaveEvent LoadedDelegate;
         LoadedDelegate.AddDynamic (this, &URStatusMgrComponent::OnLoad);
         URSaveMgr::OnLoad (world, LoadedDelegate);
      }
   }
}

void URStatusMgrComponent::EndPlay (const EEndPlayReason::Type EndPlayReason)
{
   Super::EndPlay (EndPlayReason);
}

void URStatusMgrComponent::TickComponent (float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
   Super::TickComponent (DeltaTime, TickType, ThisTickFunction);
   if (bIsAdmin) StatusRegen (DeltaTime);
}


//=============================================================================
//                 Recalc stats
//=============================================================================

void URStatusMgrComponent::RecalcStatus ()
{
   R_RETURN_IF_NOT_ADMIN;
   RecalcCoreStats ();
   RecalcSubStats ();
   RecalcStatusValues ();

   OnStatusUpdated.Broadcast ();
}

void URStatusMgrComponent::RecalcCoreStats ()
{
   R_RETURN_IF_NOT_ADMIN;
   FRCoreStats CoreStats_Total_New = GetCoreStats_Base ();

   // Flat
   for (const FRPassiveStatusEffectWithTag &ItEffect : GetPassiveEffectsWithTag ()) {
      if (ItEffect.Value.Scale == ERStatusEffectScale::FLAT) {
         if (ItEffect.Value.Target == ERStatusEffectTarget::STR) CoreStats_Total_New.STR += ItEffect.Value.Value;
         if (ItEffect.Value.Target == ERStatusEffectTarget::AGI) CoreStats_Total_New.AGI += ItEffect.Value.Value;
         if (ItEffect.Value.Target == ERStatusEffectTarget::INT) CoreStats_Total_New.INT += ItEffect.Value.Value;
      }
   }
   // Percentage
   for (const FRPassiveStatusEffectWithTag &ItEffect : GetPassiveEffectsWithTag ()) {
      if (ItEffect.Value.Scale == ERStatusEffectScale::PERCENT) {
         if (ItEffect.Value.Target == ERStatusEffectTarget::STR) CoreStats_Total_New.STR *= ((100. + ItEffect.Value.Value) / 100.);
         if (ItEffect.Value.Target == ERStatusEffectTarget::AGI) CoreStats_Total_New.AGI *= ((100. + ItEffect.Value.Value) / 100.);
         if (ItEffect.Value.Target == ERStatusEffectTarget::INT) CoreStats_Total_New.INT *= ((100. + ItEffect.Value.Value) / 100.);
      }
   }
   CoreStats_Added = CoreStats_Total_New - GetCoreStats_Base ();
}

void URStatusMgrComponent::RecalcSubStats ()
{
   R_RETURN_IF_NOT_ADMIN;
   const FRichCurve* AgiToEvasionData     = AgiToEvasion.GetRichCurveConst ();
   const FRichCurve* AgiToCriticalData    = AgiToCritical.GetRichCurveConst ();
   const FRichCurve* AgiToAttackSpeedData = AgiToAttackSpeed.GetRichCurveConst ();

   if (!ensure (AgiToEvasionData))  return;
   if (!ensure (AgiToCriticalData)) return;

   FRCoreStats StatsTotal = GetCoreStats_Total ();
   float EvasionTotal     = AgiToEvasionData->Eval (StatsTotal.AGI);
   float CriticalTotal    = AgiToCriticalData->Eval (StatsTotal.AGI);
   float AttackSpeedTotal = AgiToAttackSpeedData->Eval (StatsTotal.AGI);
   float MoveSpeedTotal   = 0;

   // Flat
   for (const FRPassiveStatusEffectWithTag &ItEffect : GetPassiveEffectsWithTag ()) {
      if (ItEffect.Value.Scale == ERStatusEffectScale::FLAT) {
         if (ItEffect.Value.Target == ERStatusEffectTarget::Evasion)     EvasionTotal     += ItEffect.Value.Value;
         if (ItEffect.Value.Target == ERStatusEffectTarget::Critical)    CriticalTotal    += ItEffect.Value.Value;
         if (ItEffect.Value.Target == ERStatusEffectTarget::AttackSpeed) AttackSpeedTotal += ItEffect.Value.Value;
         if (ItEffect.Value.Target == ERStatusEffectTarget::MoveSpeed)   MoveSpeedTotal   += ItEffect.Value.Value;
      }
   }
   // Percentage
   for (const FRPassiveStatusEffectWithTag &ItEffect : GetPassiveEffectsWithTag ()) {
      if (ItEffect.Value.Scale == ERStatusEffectScale::PERCENT) {
         if (ItEffect.Value.Target == ERStatusEffectTarget::Evasion)     EvasionTotal     *= ((100. + ItEffect.Value.Value) / 100.);
         if (ItEffect.Value.Target == ERStatusEffectTarget::Critical)    CriticalTotal    *= ((100. + ItEffect.Value.Value) / 100.);
         if (ItEffect.Value.Target == ERStatusEffectTarget::AttackSpeed) AttackSpeedTotal *= ((100. + ItEffect.Value.Value) / 100.);
         if (ItEffect.Value.Target == ERStatusEffectTarget::MoveSpeed)   MoveSpeedTotal   *= ((100. + ItEffect.Value.Value) / 100.);
      }
   }

   FRCoreStats StatsCurrent = GetCoreStats_Base ();
   SubStats_Base.Evasion      = AgiToEvasionData->Eval (StatsCurrent.AGI);
   SubStats_Added.Evasion     = EvasionTotal - SubStats_Base.Evasion;
   SubStats_Base.Critical     = AgiToCriticalData->Eval (StatsCurrent.AGI);
   SubStats_Added.Critical    = CriticalTotal - SubStats_Base.Critical;
   SubStats_Base.AttackSpeed  = AgiToAttackSpeedData->Eval (StatsCurrent.AGI);
   SubStats_Added.AttackSpeed = CriticalTotal - SubStats_Base.Critical;
   SubStats_Base.MoveSpeed    = MoveSpeedTotal;
   SubStats_Added.MoveSpeed   = 0;
}

void URStatusMgrComponent::RecalcStatusValues ()
{
   R_RETURN_IF_NOT_ADMIN;
   const FRichCurve* StrToHealthMaxData    = StrToHealthMax.GetRichCurveConst ();
   const FRichCurve* StrToHealthRegenData  = StrToHealthRegen.GetRichCurveConst ();
   const FRichCurve* AgiToStaminaMaxData   = AgiToStaminaMax.GetRichCurveConst ();
   const FRichCurve* AgiToStaminaRegenData = AgiToStaminaRegen.GetRichCurveConst ();
   const FRichCurve* IntToManaMaxData      = IntToManaMax.GetRichCurveConst ();
   const FRichCurve* IntToManaRegenData    = IntToManaRegen.GetRichCurveConst ();

   if (!ensure (StrToHealthMaxData))    return;
   if (!ensure (StrToHealthRegenData))  return;
   if (!ensure (AgiToStaminaMaxData))   return;
   if (!ensure (AgiToStaminaRegenData)) return;
   if (!ensure (IntToManaMaxData))      return;
   if (!ensure (IntToManaRegenData))    return;

   // --- Status
   FRCoreStats StatsTotal = GetCoreStats_Total ();
   Health.Max     = StrToHealthMaxData->Eval    (StatsTotal.STR);
   Health.Regen   = StrToHealthRegenData->Eval  (StatsTotal.STR);
   Stamina.Max    = AgiToStaminaMaxData->Eval   (StatsTotal.AGI);
   Stamina.Regen  = AgiToStaminaRegenData->Eval (StatsTotal.AGI);
   Mana.Max       = IntToManaMaxData->Eval      (StatsTotal.INT);
   Mana.Regen     = IntToManaRegenData->Eval    (StatsTotal.INT);

   // Flat
   for (const FRPassiveStatusEffectWithTag &ItEffect : GetPassiveEffectsWithTag ()) {
      if (ItEffect.Value.Scale == ERStatusEffectScale::FLAT) {
         if (ItEffect.Value.Target == ERStatusEffectTarget::HealthMax)    Health.Max    += ItEffect.Value.Value;
         if (ItEffect.Value.Target == ERStatusEffectTarget::HealthRegen)  Health.Regen  += ItEffect.Value.Value;
         if (ItEffect.Value.Target == ERStatusEffectTarget::StaminaMax)   Stamina.Max   += ItEffect.Value.Value;
         if (ItEffect.Value.Target == ERStatusEffectTarget::StaminaRegen) Stamina.Regen += ItEffect.Value.Value;
         if (ItEffect.Value.Target == ERStatusEffectTarget::ManaMax)      Mana.Max      += ItEffect.Value.Value;
         if (ItEffect.Value.Target == ERStatusEffectTarget::ManaRegen)    Mana.Regen    += ItEffect.Value.Value;
      }
   }
   // Percentage
   for (const FRPassiveStatusEffectWithTag &ItEffect : GetPassiveEffectsWithTag ()) {
      if (ItEffect.Value.Scale == ERStatusEffectScale::PERCENT) {
         if (ItEffect.Value.Target == ERStatusEffectTarget::HealthMax)    Health.Max    *= ((100. + ItEffect.Value.Value) / 100.);
         if (ItEffect.Value.Target == ERStatusEffectTarget::HealthRegen)  Health.Regen  *= ((100. + ItEffect.Value.Value) / 100.);
         if (ItEffect.Value.Target == ERStatusEffectTarget::StaminaMax)   Stamina.Max   *= ((100. + ItEffect.Value.Value) / 100.);
         if (ItEffect.Value.Target == ERStatusEffectTarget::StaminaRegen) Stamina.Regen *= ((100. + ItEffect.Value.Value) / 100.);
         if (ItEffect.Value.Target == ERStatusEffectTarget::ManaMax)      Mana.Max      *= ((100. + ItEffect.Value.Value) / 100.);
         if (ItEffect.Value.Target == ERStatusEffectTarget::ManaRegen)    Mana.Regen    *= ((100. + ItEffect.Value.Value) / 100.);
      }
   }
}

//=============================================================================
//                 Get Core Stats
//=============================================================================

FRCoreStats URStatusMgrComponent::GetCoreStats_Base () const
{
   return CoreStats_Base;
}
FRCoreStats URStatusMgrComponent::GetCoreStats_Added () const
{
   return CoreStats_Added;
}
FRCoreStats URStatusMgrComponent::GetCoreStats_Total () const
{
   return GetCoreStats_Base () + GetCoreStats_Added ();
}

//=============================================================================
//                 Get Sub Stats
//=============================================================================

FRSubStats URStatusMgrComponent::GetSubStats_Base () const
{
   return SubStats_Base;
}
FRSubStats URStatusMgrComponent::GetSubStats_Added () const
{
   return SubStats_Added;
}
FRSubStats URStatusMgrComponent::GetSubStats_Total () const
{
   return GetSubStats_Base () + GetSubStats_Added ();
}

//=============================================================================
//                 Stat functions
//=============================================================================

bool URStatusMgrComponent::HasStats (const FRCoreStats &RequiredStats) const
{
   return GetCoreStats_Total ().MoreThan (RequiredStats);
}

bool URStatusMgrComponent::RollCritical () const
{
   return ((FMath::Rand () % 100) <= GetSubStats_Total ().Critical);
}

bool URStatusMgrComponent::RollEvasion () const
{
   return ((FMath::Rand () % 100) <= GetSubStats_Total ().Evasion);
}

//=============================================================================
//                 Status Calls
//=============================================================================

void URStatusMgrComponent::StatusRegen (float DeltaTime)
{
   R_RETURN_IF_NOT_ADMIN;
   if (IsDead ()) return;
   Health.Tick (DeltaTime);
   Mana.Tick (DeltaTime);

   if (MovementComponent && MovementComponent->IsMovingOnGround ()) Stamina.Tick (DeltaTime);
}

bool URStatusMgrComponent::IsDead () const
{
   return bDead;
}

FRStatusValue URStatusMgrComponent::GetHealth () const
{
   return Health;
}

void URStatusMgrComponent::UseHealth (float Amount)
{
   R_RETURN_IF_NOT_ADMIN;
   Health.Current = FMath::Clamp (Health.Current - Amount, 0, Health.Max);
   if (!Health.Current) {
      bDead = true;
      // TODO: Report Death
   }
}

FRStatusValue URStatusMgrComponent::GetStamina () const
{
   return Stamina;
}

void URStatusMgrComponent::UseStamina (float Amount)
{
   R_RETURN_IF_NOT_ADMIN;
   Stamina.Current = FMath::Clamp (Stamina.Current - Amount, 0, Stamina.Max);
}

FRStatusValue URStatusMgrComponent::GetMana () const
{
   return Mana;
}

void URStatusMgrComponent::UseMana (float Amount)
{
   R_RETURN_IF_NOT_ADMIN;
   Mana.Current = FMath::Clamp (Mana.Current - Amount, 0, Mana.Max);
}

//==========================================================================
//                 Passive Effect Funcs
//==========================================================================

TArray<FRPassiveStatusEffect> URStatusMgrComponent::GetPassiveEffects () const
{
   TArray<FRPassiveStatusEffect> Result;
   for (const FRPassiveStatusEffectWithTag& ItEffects : PassiveEffects) {
      bool found = false;
      // Combine
      for (FRPassiveStatusEffect& ItRes : Result) {
         if (  ItRes.Target == ItEffects.Value.Target
            && ItRes.Scale  == ItEffects.Value.Scale)
         {
            found = true;
            ItRes.Value += ItEffects.Value.Value;
            break;
         }
      }
      // Add new entry
      if (!found) Result.Add (ItEffects.Value);
   }
   return Result;
}

TArray<FRPassiveStatusEffectWithTag> URStatusMgrComponent::GetPassiveEffectsWithTag () const
{
   return PassiveEffects;
}

bool URStatusMgrComponent::SetPassiveEffects (const FString &Tag, const TArray<FRPassiveStatusEffect> &AddValues)
{
   R_RETURN_IF_NOT_ADMIN_BOOL;
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
   return true;
}

bool URStatusMgrComponent::RmPassiveEffects (const FString &Tag)
{
   R_RETURN_IF_NOT_ADMIN_BOOL;
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
   return true;
}

//==========================================================================
//                 Active Effect Funcs
//==========================================================================

bool URStatusMgrComponent::AddActiveEffect (ARActiveStatusEffect* Effect)
{
   R_RETURN_IF_NOT_ADMIN_BOOL;
   for (int iEffect = 0; iEffect < ActiveEffects.Num (); iEffect++) {
      if (ActiveEffects[iEffect] == Effect) {
         return false;
      }
   }

   ActiveEffects.Add (Effect);
   return true;
}

bool URStatusMgrComponent::RmActiveEffect (ARActiveStatusEffect* Effect)
{
   R_RETURN_IF_NOT_ADMIN_BOOL;
   for (int iEffect = 0; iEffect < ActiveEffects.Num (); iEffect++) {
      if (ActiveEffects[iEffect] == Effect) {
         ActiveEffects.RemoveAt (iEffect);
         return true;
      }
   }
   return false;
}

TArray<ARActiveStatusEffect* > URStatusMgrComponent::GetActiveEffects () const
{
   return ActiveEffects;
}

//=============================================================================
//                 Resistance Funcs
//=============================================================================

TArray<FRResistanceStat> URStatusMgrComponent::GetResistance () const
{
   TArray<FRResistanceStat> Result;
   for (const FRResistanceStatWithTag& ItResistance : Resistence) {
      bool found = false;
      // Combine
      for (FRResistanceStat& ItRes : Result) {
         if (ItRes.DamageType == ItResistance.Value.DamageType) {
            found = true;
            ItRes.Value += ItResistance.Value.Value;
            break;
         }
      }
      // Add new entry
      if (!found) Result.Add (ItResistance.Value);
   }
   return Result;
}

TArray<FRResistanceStatWithTag> URStatusMgrComponent::GetResistanceWithTag () const
{
   return Resistence;
}

float URStatusMgrComponent::GetResistanceFor (TSubclassOf<UDamageType> const DamageType) const
{
   float Result = 0;
   for (const FRResistanceStatWithTag& ItResistance : Resistence) {
      if (DamageType == ItResistance.Value.DamageType) {
         Result += ItResistance.Value.Value;
      }
   }
   return Result;
}

void URStatusMgrComponent::AddResistance (const FString &Tag, const TArray<FRResistanceStat> &AddValues)
{
   R_RETURN_IF_NOT_ADMIN;
   // Clean
   RmResistance (Tag);

   // Add again
   for (const FRResistanceStat& ItAddValue : AddValues) {
      FRResistanceStatWithTag newRes;
      newRes.Tag   = Tag;
      newRes.Value = ItAddValue;
      Resistence.Add (newRes);
   }
   RecalcStatus ();
}

void URStatusMgrComponent::RmResistance (const FString &Tag)
{
   R_RETURN_IF_NOT_ADMIN;

   TArray<int32> ToRemove;
   for (int32 iResistance = 0; iResistance < Resistence.Num (); iResistance++) {
      const FRResistanceStatWithTag& ItResistance = Resistence[iResistance];
      if (ItResistance.Tag == Tag) ToRemove.Add (iResistance);
   }
   // Nothing to remove
   if (!ToRemove.Num ()) return;

   // Remove in reverse order;
   for (int32 iToRemove = ToRemove.Num () - 1; iToRemove >= 0; iToRemove--) {
      Resistence.RemoveAt (ToRemove[iToRemove]);
   }

   RecalcStatus ();
}

//=============================================================================
//                 TakeDamage Events
//=============================================================================

float URStatusMgrComponent::TakeDamage (float DamageAmount,
                                        FDamageEvent const& DamageEvent,
                                        AController* EventInstigator,
                                        AActor* DamageCauser)
{
   R_RETURN_IF_NOT_ADMIN_BOOL;
   URDamageType *DamageType = Cast<URDamageType>(DamageEvent.DamageTypeClass->GetDefaultObject (false));

   if (!IsDead ()) {

      if (RollEvasion () && DamageEvent.DamageTypeClass != URDamageType_Fall::StaticClass ()) {
         R_LOG ("Evaded attack!");
         // TODO: Report Evasion
         return 0;
      }

      if (DamageType) {
         float Resistance = GetResistanceFor (DamageEvent.DamageTypeClass);

         DamageAmount = DamageType->CalcDamage (DamageAmount, Resistance);
         DamageType->TakeDamage (GetOwner(), Resistance, DamageAmount, DamageEvent, EventInstigator, DamageCauser);
         //R_LOG_PRINTF ("Final Damage [%.1f] Resistance:[%1.f]", DamageAmount, Resistance);
      } else {
         R_LOG_PRINTF ("Non-URDamageType class of Damage applied. [%.1f] Damage applied directly.", DamageAmount);
      }

      UseHealth (DamageAmount);
   }
   // TODO: Report Damage

   return DamageAmount;
}

//=============================================================================
//                 Save / Load
//=============================================================================

void URStatusMgrComponent::OnSave ()
{
   R_RETURN_IF_NOT_ADMIN;

   // --- Save player status
   FBufferArchive ToBinary;
   ToBinary << Health << Mana << Stamina;

   FString SaveUniqueId = GetOwner ()->GetName () + "_StatusMgr";

   // Set binary data to save file
   if (!URSaveMgr::Set (GetWorld (), SaveUniqueId, ToBinary)) {
      R_LOG_PRINTF ("Failed to save [%s] Status.", *SaveUniqueId);
   }
}

void URStatusMgrComponent::OnLoad ()
{
   R_RETURN_IF_NOT_ADMIN;

   // --- Load player status
   FString SaveUniqueId = GetOwner ()->GetName () + "_StatusMgr";

   // Get binary data from save file
   TArray<uint8> BinaryArray;
   if (!URSaveMgr::Get (GetWorld (), SaveUniqueId, BinaryArray)) {
      R_LOG_PRINTF ("Failed to load [%s] Status.", *SaveUniqueId);
      return;
   }

   FMemoryReader FromBinary = FMemoryReader (BinaryArray, true);
   FromBinary.Seek (0);
   FromBinary << Health << Mana << Stamina;
}


//=============================================================================
//                 TUIL
//=============================================================================
URStatusMgrComponent* URStatusMgrComponent::Get (AActor* Target)
{
   if (Target == nullptr) {
      R_LOG_STATIC ("Invalid Target object");
      return false;
   }
   URStatusMgrComponent* StatusMgr = nullptr;
   {
      TArray<URStatusMgrComponent*> StatusMgrList;
      Target->GetComponents (StatusMgrList);
      if (StatusMgrList.Num ()) StatusMgr = StatusMgrList[0];
   }
   if (!StatusMgr) {
      R_LOG_STATIC_PRINTF ("Could not find StatusMgrComponent in [%s]", *Target->GetName ());
      return false;
   }
   return StatusMgr;
}

