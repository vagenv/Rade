// Copyright 2015-2023 Vagen Ayrapetyan

#include "RStatusMgrComponent.h"
#include "RUtilLib/RLog.h"
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

   BaseStats = FRCharacterStats (10);
}

// Replication
void URStatusMgrComponent::GetLifetimeReplicatedProps (TArray<FLifetimeProperty> &OutLifetimeProps) const
{
   Super::GetLifetimeReplicatedProps (OutLifetimeProps);
   DOREPLIFETIME (URStatusMgrComponent, bDead);
}

void URStatusMgrComponent::BeginPlay()
{
   Super::BeginPlay();
   const UWorld *world = GetWorld ();
   if (!ensure (world)) return;

   bIsServer = GetOwner ()->HasAuthority ();
   if (bIsServer) {
      bDead = false;

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

void URStatusMgrComponent::StatusRegen (float DeltaTime)
{
   if (Health.Current < Health.Max && Health.Regen) {
      Health.Current = Health.Current + Health.Regen * DeltaTime;
      Health.Current = FMath::Clamp (Health.Current, 0, Health.Max);
   }

   if (Mana.Current < Mana.Max && Mana.Regen) {
      Mana.Current = Mana.Current + Mana.Regen * DeltaTime;
      Mana.Current = FMath::Clamp (Mana.Current, 0, Mana.Max);
   }

   if (Stamina.Current < Stamina.Max && Stamina.Regen) {
      Stamina.Current = Stamina.Current + Stamina.Regen * DeltaTime;
      Stamina.Current = FMath::Clamp (Stamina.Current, 0, Stamina.Max);
   }
}


//=============================================================================
//                 RCharacter events
//=============================================================================

float URStatusMgrComponent::TakeDamage (float DamageAmount,
                                        FDamageEvent const& DamageEvent,
                                        AController* EventInstigator,
                                        AActor* DamageCauser)
{
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

   if (DamageAmount != .0f && !bDead) {

      /*
      // TODO: Report death, rather than doing it manually here.
      ARCharacter* RCharacter = CastChecked<ARCharacter>(GetOwner ());
      Health.Current -= DamageAmount;
      if (Health.Current < 0) Health.Current = 0;
      if (  Health.Current == 0
         && RCharacter)
      {
         RCharacter->Die_Server (DamageCauser, EventInstigator);
      }
      */
   }
   return DamageAmount;
}

//=============================================================================
//                 Save / Load
//=============================================================================

void URStatusMgrComponent::OnSave ()
{
   if (!bIsServer) {
      R_LOG ("Client has no authority to perform this action.");
      return;
   }

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
   if (!bIsServer) {
      R_LOG ("Client has no authority to perform this action.");
      return;
   }

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

