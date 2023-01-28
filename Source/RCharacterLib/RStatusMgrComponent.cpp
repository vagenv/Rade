// Copyright 2015-2023 Vagen Ayrapetyan

#include "RStatusMgrComponent.h"
#include "RUtilLib/RLog.h"
#include "RSaveLib/RSaveMgr.h"
#include "RCharacter.h"
#include "Net/UnrealNetwork.h"

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

   // // Convert Stastus to array to JSON strings
   // TArray<FString> ItemDataRaw;

   // for (FRItemData item : Items) {

   //    ItemDataRaw.Add (item.GetJSON ());
   //    // FString res;
   //    // if (FRItemData::ToJSON (item, res)) {
   //    //    ItemData.Add (res);
   //    // } else {
   //    //    R_LOG_PRINTF ("Failed to save %s", *item.Name);
   //    // }
   // }

   // // Convert array into buffer
   // FBufferArchive ToBinary;
   // ToBinary << ItemDataRaw;

   // FString StatusUniqueId = GetOwner ()->GetName ();

   // // Set binary data to save file
   // if (!URSaveMgr::Set (GetWorld (), InventoryUniqueId, ToBinary)) {
   //    R_LOG_PRINTF ("Failed to save [%s] Inventory.", *InventoryUniqueId);
   // }
}

void URStatusMgrComponent::OnLoad ()
{
   if (!bIsServer) {
      R_LOG ("Client has no authority to perform this action.");
      return;
   }

   // --- Load player status
   FString StatusUniqueId = GetOwner ()->GetName ();

   // Get binary data from save file
   TArray<uint8> BinaryArray;
   if (!URSaveMgr::Get (GetWorld (), StatusUniqueId, BinaryArray)) {
      R_LOG_PRINTF ("Failed to load [%s] Status.", *StatusUniqueId);
      return;
   }

   // // Convert Binary to array of JSON strings
   // TArray<FString> ItemDataRaw;
   // FMemoryReader FromBinary = FMemoryReader (BinaryArray, true);
   // FromBinary.Seek(0);
   // FromBinary << ItemDataRaw;

   // // Convert JSON strings to ItemData
   // TArray<FRItemData> loadedItems;
   // for (const FString &ItemRaw : ItemDataRaw) {

   //    FRItemData Item;
   //    Item.SetJSON (ItemRaw);
   //    if (!Item.ReadJSON ()) {
   //       R_LOG_PRINTF ("Failed to parse Item string [%s]", *ItemRaw);
   //       continue;
   //    }
   //    loadedItems.Add (Item);
   // }

   // // Update inventory
   // Items = loadedItems;
   // OnInventoryUpdated.Broadcast ();
}

