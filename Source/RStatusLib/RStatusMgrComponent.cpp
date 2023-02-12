// Copyright 2015-2023 Vagen Ayrapetyan

#include "RStatusMgrComponent.h"
#include "RUtilLib/RLog.h"
#include "RUtilLib/RCheck.h"
#include "RSaveLib/RSaveMgr.h"
#include "Net/UnrealNetwork.h"
#include "Engine/DamageEvents.h"

//=============================================================================
//                 Core
//=============================================================================

URStatusMgrComponent::URStatusMgrComponent ()
{
   PrimaryComponentTick.bCanEverTick = true;
   PrimaryComponentTick.bStartWithTickEnabled = true;
   SetIsReplicatedByDefault (true);

   Stats_Base = FRCharacterStats (10);

   // --- Character stats scaling
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
   StrToHealthRegenData->AddKey (   0,   1); // Minimum
   StrToHealthRegenData->AddKey (   1, 1.1);
   StrToHealthRegenData->AddKey (  10,   2);
   StrToHealthRegenData->AddKey ( 100,  10);
   StrToHealthRegenData->AddKey (1000,  50);
   StrToHealthRegenData->AddKey (5000, 100);

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
   AgiToStaminaRegenData->AddKey (   0,   10); // Minimum
   AgiToStaminaRegenData->AddKey (   1, 10.2);
   AgiToStaminaRegenData->AddKey (  10,   12);
   AgiToStaminaRegenData->AddKey ( 100,   20);
   AgiToStaminaRegenData->AddKey (1000,   40);
   AgiToStaminaRegenData->AddKey (5000,   50);

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
   IntToManaRegenData->AddKey (   0,   0); // Minimum
   IntToManaRegenData->AddKey (   1, 0.1);
   IntToManaRegenData->AddKey (  10,   1);
   IntToManaRegenData->AddKey ( 100,  10);
   IntToManaRegenData->AddKey (1000,  50);
   IntToManaRegenData->AddKey (5000, 100);
}

// Replication
void URStatusMgrComponent::GetLifetimeReplicatedProps (TArray<FLifetimeProperty> &OutLifetimeProps) const
{
   Super::GetLifetimeReplicatedProps (OutLifetimeProps);
   DOREPLIFETIME (URStatusMgrComponent, bDead);
   DOREPLIFETIME (URStatusMgrComponent, Health);
   DOREPLIFETIME (URStatusMgrComponent, Mana);
   DOREPLIFETIME (URStatusMgrComponent, Stamina);
   DOREPLIFETIME (URStatusMgrComponent, Stats_Base);
   DOREPLIFETIME (URStatusMgrComponent, Stats_Add);
   DOREPLIFETIME (URStatusMgrComponent, BaseResistence);
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

      Calc_Status ();

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

void URStatusMgrComponent::Calc_Status ()
{
   FRCharacterStats StatsTotal = GetStatsTotal ();

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

   Health.Max    = StrToHealthMaxData->Eval (StatsTotal.Strength);
   Health.Regen  = StrToHealthRegenData->Eval (StatsTotal.Strength);
   Stamina.Max   = AgiToStaminaMaxData->Eval (StatsTotal.Agility);
   Stamina.Regen = AgiToStaminaRegenData->Eval (StatsTotal.Agility);
   Mana.Max      = IntToManaMaxData->Eval (StatsTotal.Intelligence);
   Mana.Regen    = IntToManaRegenData->Eval (StatsTotal.Intelligence);
}

//=============================================================================
//                 Status Calls
//=============================================================================

void URStatusMgrComponent::StatusRegen (float DeltaTime)
{
   Health.Tick (DeltaTime);
   Mana.Tick (DeltaTime);

   if (MovementComponent && MovementComponent->IsMovingOnGround ()) Stamina.Tick (DeltaTime);
}

//=============================================================================
//                 Stats Calls
//=============================================================================

FRCharacterStats URStatusMgrComponent::GetStatsBase () const
{
   return Stats_Base;
}

FRCharacterStats URStatusMgrComponent::GetStatsAdd () const
{
   return Stats_Add;
}

FRCharacterStats URStatusMgrComponent::GetStatsTotal () const
{
   return Stats_Base + Stats_Add;
}

void URStatusMgrComponent::AddStat (const FRCharacterStats &AddValue)
{
   Stats_Add = Stats_Add + AddValue;
   Calc_Status ();
   OnStatusUpdated.Broadcast ();
}

void URStatusMgrComponent::RmStat (const FRCharacterStats &RmValue)
{
   Stats_Add = Stats_Add - RmValue;
   Calc_Status ();
   OnStatusUpdated.Broadcast ();
}

bool URStatusMgrComponent::HasStats (const FRCharacterStats &RequiredStats) const
{
   return GetStatsTotal ().MoreThan (RequiredStats);
}

//=============================================================================
//                 Resistance Calls
//=============================================================================

TArray<FRResistanceStat> URStatusMgrComponent::GetResistance () const
{
   return BaseResistence;
}

void URStatusMgrComponent::AddResistance (const TArray<FRResistanceStat> &AddValue)
{
   for (const FRResistanceStat& ItAddValue : AddValue) {
      bool found = false;
      for (FRResistanceStat &ItCurrentValue : BaseResistence) {
         if (ItCurrentValue.DamageType == ItAddValue.DamageType) {
            found = true;
            ItCurrentValue.Resistance += ItAddValue.Resistance;
            break;
         }
      }
      if (!found) {
         BaseResistence.Add (ItAddValue);
      }
   }

   OnStatusUpdated.Broadcast ();
}

void URStatusMgrComponent::RmResistance (const TArray<FRResistanceStat> &RmValue)
{
   R_RETURN_IF_NOT_ADMIN;
   for (const FRResistanceStat& ItRmValue : RmValue) {
      bool found = false;
      for (FRResistanceStat &ItCurrentValue : BaseResistence) {
         if (ItCurrentValue.DamageType == ItRmValue.DamageType) {
            found = true;
            ItCurrentValue.Resistance -= ItRmValue.Resistance;
            break;
         }
      }
      if (!found) {
         // Add as negative resistance
         FRResistanceStat newValue = ItRmValue;
         newValue.Resistance *= -1;
         BaseResistence.Add (newValue);
      }
   }

   OnStatusUpdated.Broadcast ();
}


//=============================================================================
//                 RCharacter events
//=============================================================================

float URStatusMgrComponent::TakeDamage (float DamageAmount,
                                        FDamageEvent const& DamageEvent,
                                        AController* EventInstigator,
                                        AActor* DamageCauser)
{
   R_RETURN_IF_NOT_ADMIN_BOOL;
   URDamageType *DamageType = Cast<URDamageType>(DamageEvent.DamageTypeClass->GetDefaultObject (false));
   if (DamageType) {
      float Resistance = 0;

      // TODO: Replace with total calculated Resistance
      for (const FRResistanceStat &stat : BaseResistence) {
         if (stat.DamageType == DamageEvent.DamageTypeClass) {
            Resistance = stat.Resistance;
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

