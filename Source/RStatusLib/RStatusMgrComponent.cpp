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
   Health.Max   = 200  + StatsTotal.Strength * 10;
   Health.Regen = 0    + StatsTotal.Strength * 0.1;

   Stamina.Max   = 100 + StatsTotal.Agility * 2;
   Stamina.Regen = 20  + StatsTotal.Agility * 0.15;

   Mana.Max   = 50 + StatsTotal.Intelligence * 10;
   Mana.Regen = 0  + StatsTotal.Intelligence * 0.1;
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

