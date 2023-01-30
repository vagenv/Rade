// Copyright 2015-2023 Vagen Ayrapetyan

#include "RStatusMgrComponent.h"
#include "RUtilLib/RLog.h"
#include "RSaveLib/RSaveMgr.h"
#include "RCharacter.h"
#include "Net/UnrealNetwork.h"
#include "Engine/DamageEvents.h"
#include "RDamageType.h"

//=============================================================================
//                 Core
//=============================================================================

URStatusMgrComponent::URStatusMgrComponent ()
{
   SetIsReplicatedByDefault (true);
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
      if (bSaveLoadStatus) {
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

//=============================================================================
//                 RCharacter events
//=============================================================================

float URStatusMgrComponent::TakeDamage (float DamageAmount,
                                        FDamageEvent const& DamageEvent,
                                        AController* EventInstigator,
                                        AActor* DamageCauser)
{
   ARCharacter* RCharacter = CastChecked<ARCharacter>(GetOwner ());

   // Calc damage with resistance and scaling
   //R_LOG_PRINTF ("Damage type [%s]", *DamageEvent.DamageTypeClass->GetDefaultObject()->GetName ());

   URDamageType *DamageType = DamageEvent.DamageTypeClass->GetDefaultObject<URDamageType>();
   if (!ensure (DamageType)) return 0;


   // TODO: Get Resistance.
   DamageAmount = DamageType->CalcDamage (DamageAmount, 0);

   //R_LOG_PRINTF ("Final Damage [%f]", DamageAmount);


   if (DamageAmount != .0f && !bDead) {
      Health.Current -= DamageAmount;

      if (Health.Current < 0) Health.Current = 0;

      if (  Health.Current == 0
         && RCharacter)
      {
         RCharacter->Die_Server (DamageCauser, EventInstigator);
      }
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

