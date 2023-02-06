// Copyright 2015-2023 Vagen Ayrapetyan

#include "RInventoryComponent.h"
#include "RUtilLib/RLog.h"
#include "RSaveLib/RSaveMgr.h"

#include "RItemAction.h"
#include "RItemPickup.h"
#include "Net/UnrealNetwork.h"

//=============================================================================
//                 Core
//=============================================================================

URInventoryComponent::URInventoryComponent ()
{
   SetIsReplicatedByDefault (true);
}

// Replication
void URInventoryComponent::GetLifetimeReplicatedProps (TArray<FLifetimeProperty> &OutLifetimeProps) const
{
   Super::GetLifetimeReplicatedProps (OutLifetimeProps);
   DOREPLIFETIME (URInventoryComponent, SlotsMax);
   DOREPLIFETIME (URInventoryComponent, WeightCurrent);
   DOREPLIFETIME (URInventoryComponent, WeightMax);
   DOREPLIFETIME (URInventoryComponent, Items);
}

void URInventoryComponent::BeginPlay()
{
   Super::BeginPlay();
   const UWorld *world = GetWorld ();
   if (!ensure (world)) return;

   bIsServer = GetOwner ()->HasAuthority ();
   // if (GetLocalRole() >= ROLE_Authority)

   // Save/Load inventory
   if (bSaveLoad && bIsServer) {
      FRSaveEvent SavedDelegate;
      SavedDelegate.AddDynamic (this, &URInventoryComponent::OnSave);
      URSaveMgr::OnSave (world, SavedDelegate);

      FRSaveEvent LoadedDelegate;
      LoadedDelegate.AddDynamic (this, &URInventoryComponent::OnLoad);
      URSaveMgr::OnLoad (world, LoadedDelegate);
   }

   if (bIsServer) {
      for (const auto &itItem : DefaultItems) {
         if (!AddItem_Arch (itItem))
            R_LOG_PRINTF ("Failed to add default item [%s] to [%s]",
               *itItem.Arch.RowName.ToString (), *GetOwner()->GetName ());
      }
   }

   if (bCheckClosestPickup) {
      world->GetTimerManager ().SetTimer (TimerClosestPickup,
         this, &URInventoryComponent::CheckClosestPickup, CheckClosestDelay, true);
   }
}

void URInventoryComponent::EndPlay (const EEndPlayReason::Type EndPlayReason)
{
   Super::EndPlay (EndPlayReason);

	// Ensure the fuze timer is cleared by using the timer handle
	GetWorld ()->GetTimerManager ().ClearTimer (TimerClosestPickup);
}

//=============================================================================
//                 Add Item
//=============================================================================

TArray<FRItemData> URInventoryComponent::GetItems () const
{
   return Items;
}

void URInventoryComponent::OnRep_Items ()
{
   CalcWeight ();
   OnInventoryUpdated.Broadcast ();
}

//=============================================================================
//                 Check if contains
//=============================================================================

bool URInventoryComponent::HasItem_Arch (const FRItemDataHandle &CheckItem) const
{
   // --- Create required item info
   FRItemData requireItem;
   if (!CheckItem.ToItem (requireItem)) return false;
   return HasItem_Data (requireItem);
}

bool URInventoryComponent::HasItem_Data (FRItemData requireItem) const
{
   // --- Iterate over inventory items
   for (const FRItemData &itItem : Items) {
      if (itItem.Name != requireItem.Name) continue;
      requireItem.Count -= itItem.Count;
      if (requireItem.Count <= 0) break;
   }
   return (requireItem.Count <= 0);
}


bool URInventoryComponent::HasItems (const TArray<FRItemDataHandle> &CheckItems) const
{
   // --- Create list of required item infos
   TArray<FRItemData> requiredItems;
   for (const FRItemDataHandle &itItem : CheckItems) {
      FRItemData requireItem;
      if (!itItem.ToItem (requireItem)) return false;
      requiredItems.Add (requireItem);
   }

   // --- Iterate over inventory items
   for (const FRItemData &itItem : Items) {

      // --- Iterate over required items
      for (int i = 0; i < requiredItems.Num (); i++) {
         FRItemData &requireItem = requiredItems[i];
         if (itItem.Name != requireItem.Name) continue;

         requireItem.Count -= itItem.Count;
         if (requireItem.Count <= 0) {
            requiredItems.RemoveAt (i);
            break;
         };
      }
   }

   return requiredItems.Num () == 0;
}

int URInventoryComponent::GetCountItem (const FRItemData &CheckItem) const
{
   return GetCountItem_Name (CheckItem.Name);
}
int URInventoryComponent::GetCountItem_Name (const FString &CheckItemName) const
{
   int Count = 0;
   // --- Find same kind of item
   for (const FRItemData &ItItem : Items) {
      // Not same item type
      if (ItItem.Name != CheckItemName) continue;
      Count += ItItem.Count;
   }

   return Count;
}

//=============================================================================
//                 Add item
//=============================================================================

bool URInventoryComponent::AddItem_Arch (const FRItemDataHandle &ItemHandle)
{
   if (!bIsServer) {
      R_LOG ("Client has no authority to perform this action.");
      return false;
   }
   FRItemData newItem;
   if (!ItemHandle.ToItem (newItem)) return false;
   return AddItem (newItem);
}

void URInventoryComponent::AddItem_Server_Implementation (URInventoryComponent *SrcInventory,
                                                          FRItemData NewItem) const
{
   if (!SrcInventory) {
      R_LOG ("Invalid Inventory pointer");
      return;
   }
   SrcInventory->AddItem (NewItem);
}
bool URInventoryComponent::AddItem (FRItemData NewItem)
{
   if (!bIsServer) {
      R_LOG ("Client has no authority to perform this action.");
      return false;
   }

   // --- Limit weight being used
   float WeightAdd = NewItem.Count * NewItem.Weight;
   if (WeightCurrent + WeightAdd > WeightMax) return false;

   // --- Check if item is stackable
   if (NewItem.MaxCount > 1) {

      // --- Find same kind of item
      for (FRItemData &ItItem : Items) {

         // Not same item type
         if (ItItem.Name != NewItem.Name) continue;

         int32 ItItemCountLeft = ItItem.MaxCount - ItItem.Count;

         // Slot full
         if (ItItemCountLeft <= 0) continue;

         // --- Full fit
         if (NewItem.Count <= ItItemCountLeft) {
            ItItem.Count += NewItem.Count;
            CalcWeight ();
            OnInventoryUpdated.Broadcast ();
            return true;
         }

         // --- Partial fit
          ItItem.Count += ItItemCountLeft;
         NewItem.Count -= ItItemCountLeft;
      }
   }

   // --- Check if overflow. For default items.
   if (NewItem.Count > NewItem.MaxCount) {
      int n = NewItem.Count / NewItem.MaxCount;
      for (int i = 0; i < n; i++) {

         // Check if slot available
         if (Items.Num () >= SlotsMax) break;

         FRItemData it (NewItem);
         it.Count = it.MaxCount;
         NewItem.Count -= it.Count;
         Items.Add (it);
      }
   }

   // Limit number of slots used
   if (Items.Num () >= SlotsMax) {
      CalcWeight ();
      OnInventoryUpdated.Broadcast ();
      R_LOG ("Maximum number of inventory slots reached");
      return false;
   }

   Items.Add (NewItem);
   CalcWeight ();
   OnInventoryUpdated.Broadcast ();
   return true;
}

//=============================================================================
//                 Remove Item index
//=============================================================================

void URInventoryComponent::RemoveItem_Index_Server_Implementation (URInventoryComponent *SrcInventory,
                                                                   int32 ItemIdx, int32 Count) const
{
   if (!SrcInventory) {
      R_LOG ("Invalid Inventory pointer");
      return;
   }
   SrcInventory->RemoveItem_Index (ItemIdx, Count);
}
bool URInventoryComponent::RemoveItem_Index (int32 ItemIdx, int32 Count)
{
   if (!bIsServer) {
      R_LOG ("Client has no authority to perform this action.");
      return false;
   }

   if (!Items.IsValidIndex (ItemIdx)) {
      R_LOG ("Invalid Item index");
      return false;
   }

   if (Items[ItemIdx].Count > Count) {
      Items[ItemIdx].Count -= Count;
   } else {
      Items.RemoveAt (ItemIdx);
   }

   CalcWeight ();
   OnInventoryUpdated.Broadcast ();
   return true;
}

//=============================================================================
//                 Remove Item Arch
//=============================================================================

void URInventoryComponent::RemoveItem_Arch_Server_Implementation (URInventoryComponent *SrcInventory,
                                                                  const FRItemDataHandle &RmItemHandle) const
{
   if (!SrcInventory) {
      R_LOG ("Invalid Inventory pointer");
      return;
   }
   SrcInventory->RemoveItem_Arch (RmItemHandle);
}
bool URInventoryComponent::RemoveItem_Arch (const FRItemDataHandle &RmItemHandle)
{
   if (!bIsServer) {
      R_LOG ("Client has no authority to perform this action.");
      return false;
   }
   FRItemData RmItemData;
   if (!RmItemHandle.ToItem (RmItemData)) return false;
   return RemoveItem_Data (RmItemData);
}

//=============================================================================
//                 Remove Item Data
//=============================================================================

void URInventoryComponent::RemoveItem_Data_Server_Implementation (URInventoryComponent *SrcInventory,
                                                                  FRItemData RmItemData) const
{
   if (!SrcInventory) {
      R_LOG ("Invalid Inventory pointer");
      return;
   }
   SrcInventory->RemoveItem_Data (RmItemData);
}

bool URInventoryComponent::RemoveItem_Data (FRItemData RmItemData)
{
   if (!bIsServer) {
      R_LOG ("Client has no authority to perform this action.");
      return false;
   }
   // Last check that user has required item
   if (!HasItem_Data (RmItemData)) return false;

   // Remove until finished
   while (RmItemData.Count > 0) {
      for (int i = 0; i < Items.Num (); i++) {
         if (Items[i].Name != RmItemData.Name) continue;

         int Count = FMath::Min (Items[i].Count, RmItemData.Count);
         if (!RemoveItem_Index (i, Count)) return false;
         RmItemData.Count -= Count;
      }
   }

   return (RmItemData.Count == 0);
}


//=============================================================================
//                 Transfer
//=============================================================================

void URInventoryComponent::TransferAll_Server_Implementation (URInventoryComponent *SrcInventory,
                                                              URInventoryComponent *DstInventory) const
{
   if (!SrcInventory) {
      R_LOG ("Invalid Source Inventory");
      return;
   }
   if (!DstInventory) {
      R_LOG ("Invalid Destination Inventory");
      return;
   }
   SrcInventory->TransferAll (DstInventory);
}
bool URInventoryComponent::TransferAll (URInventoryComponent *DstInventory)
{
   if (!DstInventory) {
      R_LOG ("Invalid Destination Inventory");
      return false;
   }
   int n = Items.Num ();
   for (int i = 0; i< n; i++) {
      if (!TransferItem (DstInventory, 0, 0)) {
         return false;
      }
   }
   return true;
}

void URInventoryComponent::TransferItem_Server_Implementation (URInventoryComponent *SrcInventory,
                                                               URInventoryComponent *DstInventory,
                                                               int32 SrcItemIdx,
                                                               int32 SrcItemCount) const
{
   if (!SrcInventory) {
      R_LOG ("Invalid Source Inventory");
      return;
   }
   if (!DstInventory) {
      R_LOG ("Invalid Destination Inventory");
      return;
   }
   SrcInventory->TransferItem (DstInventory, SrcItemIdx, SrcItemCount);
}
bool URInventoryComponent::TransferItem (URInventoryComponent *DstInventory,
                                         int32 SrcItemIdx,
                                         int32 SrcItemCount)
{
   if (!DstInventory) {
      R_LOG ("Invalid Destination Inventory");
      return false;
   }

   if (!Items.IsValidIndex (SrcItemIdx)) {
      R_LOG_PRINTF ("Invalid Source Inventory Item Index [%d]. Must be [0-%d]",
         SrcItemIdx, Items.Num ());
      return false;
   }

   FRItemData ItemData = Items[SrcItemIdx];

   // All items
   if (SrcItemCount <= 0) SrcItemCount = ItemData.Count;
   ItemData.Count = SrcItemCount;

   // Try to add item
   if (!DstInventory->AddItem (ItemData)) return false;

   // Cleanup
   return RemoveItem_Index (SrcItemIdx, SrcItemCount);
}

void URInventoryComponent::CalcWeight ()
{
   float WeightNew = 0;
   // Find same kind of item
   for (const FRItemData &ItItem : Items) {
      WeightNew += (ItItem.Count * ItItem.Weight);
   }

   WeightCurrent = WeightNew;
}

//=============================================================================
//                 Item action
//=============================================================================

void URInventoryComponent::UseItem_Server_Implementation (URInventoryComponent *DstInventory,
                                                          int32 ItemIdx) const
{
   if (!DstInventory) {
      R_LOG ("Invalid Destination Inventory");
      return;
   }
   DstInventory->UseItem (ItemIdx);
}
bool URInventoryComponent::UseItem (int32 ItemIdx)
{
   if (!bIsServer) {
      R_LOG ("Client has no authority to perform this action.");
      return false;
   }

   // Valid index
   if (!Items.IsValidIndex (ItemIdx)) {
      R_LOG_PRINTF ("Invalid Item Index [%d]. Must be [0-%d]",
         ItemIdx, Items.Num ());
      return false;
   }

   FRActionItemData ItemData;
   if (!FRActionItemData::Cast (Items[ItemIdx], ItemData)) {
      return false;
   }

   // valid archetype
   if (!ItemData.Action) return false;

   URItemAction *ItemBP = ItemData.Action->GetDefaultObject<URItemAction>();

   if (!ensure (ItemBP)) return false;
   ItemBP->Used (this, ItemData, ItemIdx);
   BP_Used (ItemIdx);

   // Consumable
   if (ItemData.DestroyOnAction) return RemoveItem_Index (ItemIdx, 1);
   return true;
}

void URInventoryComponent::DropItem_Server_Implementation (URInventoryComponent *SrcInventory,
                                                           int32 ItemIdx, int32 Count) const
{
   if (!SrcInventory) {
      R_LOG ("Invalid Source Inventory");
      return;
   }
   SrcInventory->DropItem (ItemIdx, Count);
}
ARItemPickup* URInventoryComponent::DropItem (int32 ItemIdx, int32 Count)
{
   if (!bIsServer) {
      R_LOG ("Client has no authority to perform this action.");
      return nullptr;
   }

   // Valid index
   if (!Items.IsValidIndex (ItemIdx)) {
      R_LOG_PRINTF ("Invalid Inventory Item Index [%d]. Must be [0-%d]",
         ItemIdx, Items.Num ());
      return nullptr;
   }

   FRItemData ItemData = Items[ItemIdx];

   // Everything
   if (Count <= 0)             Count = ItemData.Count;
   // Overflow
   if (Count > ItemData.Count) Count = ItemData.Count;

   ItemData.Count = Count;

   // Error will be logged.
   if (!RemoveItem_Index (ItemIdx, Count)) return nullptr;

   AActor *Player = GetOwner ();

   // Get Player Rotation
   FRotator rot = Player->GetActorRotation();
   FVector forwardVector = rot.Vector() * 300;
           forwardVector.Z = 0;
   FVector spawnLoc = Player->GetActorLocation() + forwardVector + FVector(0, 0, 50);

   // Create pickup
   ARItemPickup *Pickup = nullptr;

   // Custom Pickup Type
   if (ItemData.Pickup) {
      Pickup = GetWorld ()->SpawnActor<ARItemPickup>(ItemData.Pickup, spawnLoc, rot);
   } else {
      // Default Pickup type
      Pickup = GetWorld ()->SpawnActor<ARItemPickup>(ARItemPickup::StaticClass (), spawnLoc, rot);
      // Custom mesh pickup
      if (ItemData.PickupMesh) {
         Pickup->MeshComponent->SetStaticMesh (ItemData.PickupMesh);
      }
   }
   Pickup->SetOwner (GetOwner ());
   Pickup->bAutoPickup  = false;
   Pickup->bAutoDestroy = true;

   Pickup->Inventory->DefaultItems.Empty ();
   Pickup->Inventory->Items.Empty ();

   Pickup->Inventory->Items.Add (ItemData);

   BP_Droped (ItemIdx, Pickup);

   return Pickup;
}

//=============================================================================
//                 Pickup
//=============================================================================

bool URInventoryComponent::Pickup_Add (const ARItemPickup* Pickup)
{
   if (Pickup == nullptr) return false;
   CurrentPickups.Add (Pickup);
   OnPickupListUpdated.Broadcast ();
   return true;
}

bool URInventoryComponent::Pickup_Rm (const ARItemPickup* Pickup)
{
   if (Pickup == nullptr) return false;
   CurrentPickups.RemoveSingle (Pickup);
   OnPickupListUpdated.Broadcast ();
   return true;
}

const ARItemPickup* URInventoryComponent::GetClosestPickup () const
{
   return ClosestPickup;
}

void URInventoryComponent::CheckClosestPickup ()
{
   FVector PlayerLoc = GetOwner ()->GetActorLocation ();

   FVector              newClosestPickupLoc;
   const ARItemPickup * newClosestPickup = nullptr;

   for (const ARItemPickup *itPickup : CurrentPickups) {
      if (!ensure (itPickup)) continue;

      FVector itLoc = itPickup->GetActorLocation ();

      if (!newClosestPickup) {
         newClosestPickup    = itPickup;
         newClosestPickupLoc = itLoc;
      }

      if (FVector::Distance (PlayerLoc, itLoc) < FVector::Distance (PlayerLoc, newClosestPickupLoc)) {
         newClosestPickup    = itPickup;
         newClosestPickupLoc = itLoc;
      }
   }

   if (newClosestPickup != ClosestPickup) {
      ClosestPickup = newClosestPickup;
      OnClosestPickupUpdated.Broadcast ();
   }
}


//=============================================================================
//                 Save / Load
//=============================================================================

void URInventoryComponent::OnSave ()
{
   if (!bIsServer) {
      R_LOG ("Client has no authority to perform this action.");
      return;
   }

   // --- Save player Inventory

   // Convert ItemData to array to JSON strings
   TArray<FString> ItemDataRaw;
   for (FRItemData item : Items) {
      ItemDataRaw.Add (item.GetJSON ());
   }

   // Convert array into buffer
   FBufferArchive ToBinary;
   ToBinary << ItemDataRaw;

   FString SaveUniqueId = GetOwner ()->GetName () + "_Inventory";

   // Set binary data to save file
   if (!URSaveMgr::Set (GetWorld (), SaveUniqueId, ToBinary)) {
      R_LOG_PRINTF ("Failed to save [%s] Inventory.", *SaveUniqueId);
   }
}

void URInventoryComponent::OnLoad ()
{
   if (!bIsServer) {
      R_LOG ("Client has no authority to perform this action.");
      return;
   }

   // --- Load player Inventory
   FString SaveUniqueId = GetOwner ()->GetName () + "_Inventory";

   // Get binary data from save file
   TArray<uint8> BinaryArray;
   if (!URSaveMgr::Get (GetWorld (), SaveUniqueId, BinaryArray)) {
      R_LOG_PRINTF ("Failed to load [%s] Inventory.", *SaveUniqueId);
      return;
   }

   // Convert Binary to array of JSON strings
   TArray<FString> ItemDataRaw;
   FMemoryReader FromBinary = FMemoryReader (BinaryArray, true);
   FromBinary.Seek(0);
   FromBinary << ItemDataRaw;

   // Convert JSON strings to ItemData
   TArray<FRItemData> loadedItems;
   for (const FString &ItemRaw : ItemDataRaw) {

      FRItemData Item;
      Item.SetJSON (ItemRaw);
      if (!Item.ReadJSON ()) {
         R_LOG_PRINTF ("Failed to parse Item string [%s]", *ItemRaw);
         continue;
      }
      loadedItems.Add (Item);
   }

   // Update inventory
   Items = loadedItems;
   OnInventoryUpdated.Broadcast ();
}

