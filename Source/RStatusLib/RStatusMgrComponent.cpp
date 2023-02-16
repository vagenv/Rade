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
   DOREPLIFETIME (URStatusMgrComponent, ExtraStats_Base);
   DOREPLIFETIME (URStatusMgrComponent, ExtraStats_Added);

   DOREPLIFETIME (URStatusMgrComponent, ExtraResistence);
   DOREPLIFETIME (URStatusMgrComponent, ExtraEffects);
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
   StatusRegen (DeltaTime);
}


//=============================================================================
//                 Recalc stats
//=============================================================================

void URStatusMgrComponent::RecalcStatus ()
{
   RecalcCoreStats ();
   RecalcExtraStats ();
   RecalcStatusValues ();

   OnStatusUpdated.Broadcast ();
}

void URStatusMgrComponent::RecalcCoreStats ()
{
   FRCoreStats CoreStats_Total_New = GetCoreStats_Base ();

   // Flat
   for (const FRStatusEffectWithTag &It : ExtraEffects) {
      if (It.Effect.Scale == ERStatusEffectScale::FLAT) {
         if (It.Effect.Target == ERStatusEffectTarget::STR) CoreStats_Total_New.STR += It.Effect.Value;
         if (It.Effect.Target == ERStatusEffectTarget::AGI) CoreStats_Total_New.AGI += It.Effect.Value;
         if (It.Effect.Target == ERStatusEffectTarget::INT) CoreStats_Total_New.INT += It.Effect.Value;
      }
   }
   // Percentage
   for (const FRStatusEffectWithTag &It : ExtraEffects) {
      if (It.Effect.Scale == ERStatusEffectScale::PERCENT) {
         if (It.Effect.Target == ERStatusEffectTarget::STR) CoreStats_Total_New.STR *= ((100. + It.Effect.Value) / 100.);
         if (It.Effect.Target == ERStatusEffectTarget::AGI) CoreStats_Total_New.AGI *= ((100. + It.Effect.Value) / 100.);
         if (It.Effect.Target == ERStatusEffectTarget::INT) CoreStats_Total_New.INT *= ((100. + It.Effect.Value) / 100.);
      }
   }
   CoreStats_Added = CoreStats_Total_New - GetCoreStats_Base ();
}

void URStatusMgrComponent::RecalcExtraStats ()
{
   const FRichCurve* AgiToEvasionData     = AgiToEvasion.GetRichCurveConst ();
   const FRichCurve* AgiToCriticalData    = AgiToCritical.GetRichCurveConst ();
   const FRichCurve* AgiToAttackSpeedData = AgiToAttackSpeed.GetRichCurveConst ();

   if (!ensure (AgiToEvasionData))  return;
   if (!ensure (AgiToCriticalData)) return;

   FRCoreStats StatsTotal = GetCoreStats_Total ();
   float EvasionTotal     = AgiToEvasionData->Eval (StatsTotal.AGI);
   float CriticalTotal    = AgiToCriticalData->Eval (StatsTotal.AGI);
   float AttackSpeedTotal = AgiToAttackSpeedData->Eval (StatsTotal.AGI);

   // Flat
   for (const FRStatusEffectWithTag &It : ExtraEffects) {
      if (It.Effect.Scale == ERStatusEffectScale::FLAT) {
         if (It.Effect.Target == ERStatusEffectTarget::Evasion)     EvasionTotal     += It.Effect.Value;
         if (It.Effect.Target == ERStatusEffectTarget::Critical)    CriticalTotal    += It.Effect.Value;
         if (It.Effect.Target == ERStatusEffectTarget::AttackSpeed) AttackSpeedTotal += It.Effect.Value;
      }
   }
   // Percentage
   for (const FRStatusEffectWithTag &It : ExtraEffects) {
      if (It.Effect.Scale == ERStatusEffectScale::PERCENT) {
         if (It.Effect.Target == ERStatusEffectTarget::Evasion)     EvasionTotal     *= ((100. + It.Effect.Value) / 100.);
         if (It.Effect.Target == ERStatusEffectTarget::Critical)    CriticalTotal    *= ((100. + It.Effect.Value) / 100.);
         if (It.Effect.Target == ERStatusEffectTarget::AttackSpeed) AttackSpeedTotal *= ((100. + It.Effect.Value) / 100.);
      }
   }

   FRCoreStats StatsCurrent = GetCoreStats_Base ();
   ExtraStats_Base.Evasion      = AgiToEvasionData->Eval (StatsCurrent.AGI);
   ExtraStats_Added.Evasion     = EvasionTotal - ExtraStats_Base.Evasion;
   ExtraStats_Base.Critical     = AgiToCriticalData->Eval (StatsCurrent.AGI);
   ExtraStats_Added.Critical    = CriticalTotal - ExtraStats_Base.Critical;
   ExtraStats_Base.AttackSpeed  = AgiToAttackSpeedData->Eval (StatsCurrent.AGI);
   ExtraStats_Added.AttackSpeed = CriticalTotal - ExtraStats_Base.Critical;
}

void URStatusMgrComponent::RecalcStatusValues ()
{
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
   for (const FRStatusEffectWithTag &It : ExtraEffects) {
      if (It.Effect.Scale == ERStatusEffectScale::FLAT) {
         if (It.Effect.Target == ERStatusEffectTarget::HealthMax)    Health.Max    += It.Effect.Value;
         if (It.Effect.Target == ERStatusEffectTarget::HealthRegen)  Health.Regen  += It.Effect.Value;
         if (It.Effect.Target == ERStatusEffectTarget::StaminaMax)   Stamina.Max   += It.Effect.Value;
         if (It.Effect.Target == ERStatusEffectTarget::StaminaRegen) Stamina.Regen += It.Effect.Value;
         if (It.Effect.Target == ERStatusEffectTarget::ManaMax)      Mana.Max      += It.Effect.Value;
         if (It.Effect.Target == ERStatusEffectTarget::ManaRegen)    Mana.Regen    += It.Effect.Value;

      }
   }
   // Percentage
   for (const FRStatusEffectWithTag &It : ExtraEffects) {
      if (It.Effect.Scale == ERStatusEffectScale::PERCENT) {
         if (It.Effect.Target == ERStatusEffectTarget::HealthMax)    Health.Max    *= ((100. + It.Effect.Value) / 100.);
         if (It.Effect.Target == ERStatusEffectTarget::HealthRegen)  Health.Regen  *= ((100. + It.Effect.Value) / 100.);
         if (It.Effect.Target == ERStatusEffectTarget::StaminaMax)   Stamina.Max   *= ((100. + It.Effect.Value) / 100.);
         if (It.Effect.Target == ERStatusEffectTarget::StaminaRegen) Stamina.Regen *= ((100. + It.Effect.Value) / 100.);
         if (It.Effect.Target == ERStatusEffectTarget::ManaMax)      Mana.Max      *= ((100. + It.Effect.Value) / 100.);
         if (It.Effect.Target == ERStatusEffectTarget::ManaRegen)    Mana.Regen    *= ((100. + It.Effect.Value) / 100.);
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
//                 Get Extra Stats
//=============================================================================

FRExtraStats URStatusMgrComponent::GetExtraStats_Base () const
{
   return ExtraStats_Base;
}
FRExtraStats URStatusMgrComponent::GetExtraStats_Added () const
{
   return ExtraStats_Added;
}
FRExtraStats URStatusMgrComponent::GetExtraStats_Total () const
{
   return GetExtraStats_Base () + GetExtraStats_Added ();
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
   return ((FMath::Rand () % 100) <= GetExtraStats_Total ().Critical);
}

bool URStatusMgrComponent::RollEvasion () const
{
   return ((FMath::Rand () % 100) <= GetExtraStats_Total ().Evasion);
}

//=============================================================================
//                 Status Calls
//=============================================================================

void URStatusMgrComponent::StatusRegen (float DeltaTime)
{
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
   Stamina.Current = FMath::Clamp (Stamina.Current - Amount, 0, Stamina.Max);
}

FRStatusValue URStatusMgrComponent::GetMana () const
{
   return Mana;
}

void URStatusMgrComponent::UseMana (float Amount)
{
   Mana.Current = FMath::Clamp (Mana.Current - Amount, 0, Mana.Max);
}

//==========================================================================
//                 Effect Funcs
//==========================================================================

TArray<FRStatusEffect> URStatusMgrComponent::GetEffects () const
{
   TArray<FRStatusEffect> res;
   for (const FRStatusEffectWithTag& it : ExtraEffects) {
      res.Add (it.Effect);
   }
   return res;
}

void URStatusMgrComponent::SetEffects (const FString &Tag, const TArray<FRStatusEffect> &AddValues)
{
   // Clean
   RmEffects (Tag);

   // Add again
   for (const FRStatusEffect& it : AddValues) {
      FRStatusEffectWithTag newRes;
      newRes.Tag    = Tag;
      newRes.Effect = it;
      ExtraEffects.Add (newRes);
   }

   RecalcStatus ();
}

void URStatusMgrComponent::RmEffects (const FString &Tag)
{
   TArray<int32> ToRemove;
   for (int32 i = 0; i < ExtraEffects.Num (); i++) {
      const FRStatusEffectWithTag& It = ExtraEffects[i];
      if (It.Tag == Tag) ToRemove.Add (i);
   }
   // Nothing to remove
   if (!ToRemove.Num ()) return;

   // Remove in reverse order;
   for (int32 i = ToRemove.Num () - 1; i >= 0; i--) {
      ExtraEffects.RemoveAt (ToRemove[i]);
   }

   RecalcStatus ();
}

//=============================================================================
//                 Resistance Funcs
//=============================================================================

TArray<FRResistanceStat> URStatusMgrComponent::GetResistance () const
{
   TArray<FRResistanceStat> res;
   for (const FRResistanceStatWithTag& it : ExtraResistence) {
      res.Add (it.Resistance);
   }
   return res;
}

void URStatusMgrComponent::AddResistance (const FString &Tag, const TArray<FRResistanceStat> &AddValues)
{
   // Clean
   RmResistance (Tag);

   // Add again
   for (const FRResistanceStat& it : AddValues) {
      FRResistanceStatWithTag newRes;
      newRes.Tag        = Tag;
      newRes.Resistance = it;
      ExtraResistence.Add (newRes);
   }
   RecalcStatus ();
}

void URStatusMgrComponent::RmResistance (const FString &Tag)
{
   R_RETURN_IF_NOT_ADMIN;

   TArray<int32> ToRemove;
   for (int32 i = 0; i < ExtraResistence.Num (); i++) {
      const FRResistanceStatWithTag& It = ExtraResistence[i];
      if (It.Tag == Tag) ToRemove.Add (i);
   }
   // Nothing to remove
   if (!ToRemove.Num ()) return;

   // Remove in reverse order;
   for (int32 i = ToRemove.Num () - 1; i >= 0; i--) {
      ExtraResistence.RemoveAt (ToRemove[i]);
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

   if (RollEvasion ()) {
      // Report Evasion
      return 0;
   }

   if (DamageType) {
      float Resistance = 0;

      // TODO: Replace with total calculated Resistance
      for (const FRResistanceStatWithTag &it : ExtraResistence) {
         if (it.Resistance.DamageType == DamageEvent.DamageTypeClass) {
            Resistance = it.Resistance.Value;
            break;
         }
      }

      DamageAmount = DamageType->CalcDamage (DamageAmount, Resistance);
      DamageType->TakeDamage (GetOwner(), Resistance, DamageAmount, DamageEvent, EventInstigator, DamageCauser);
      //R_LOG_PRINTF ("Final Damage [%.1f]", DamageAmount);
   } else {
      R_LOG_PRINTF ("Non-URDamageType class of Damage applied. [%.1f] Damage applied directly.", DamageAmount);
   }

   // TODO: Report damage
   if (DamageAmount != .0f && !bDead) {

      Health.Current -= DamageAmount;
      if (Health.Current < 0) Health.Current = 0;
      if (Health.Current == 0) {
         // TODO: Report death
      }
   }
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

