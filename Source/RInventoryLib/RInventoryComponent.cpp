// Copyright 2015-2023 Vagen Ayrapetyan

#include "RInventoryComponent.h"
#include "RItemPickup.h"

#include "RUtilLib/RLog.h"
#include "RUtilLib/RCheck.h"
#include "RSaveLib/RWorldSaveMgr.h"

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

   if (R_IS_NET_ADMIN) {

      // Save/Load inventory
      if (bSaveLoad) {
         // Careful with collision of 'UniqueSaveId'
         FString UniqueSaveId = GetOwner ()->GetName () + "_Inventory";
         Init_Save (this, UniqueSaveId);
      }

      for (const FRItemDataHandle &ItItem : DefaultItems) {
         if (!AddItem_Arch (ItItem))
            R_LOG_PRINTF ("Failed to add default item [%s] to [%s]",
               *ItItem.Arch.RowName.ToString (), *GetOwner()->GetName ());
      }
   }
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
   ReportInventoryUpdate ();
}

void URInventoryComponent::ReportInventoryUpdate () const
{
   if (R_IS_VALID_WORLD && OnInventoryUpdated.IsBound ()) OnInventoryUpdated.Broadcast ();
}

//=============================================================================
//                 Check if contains
//=============================================================================

bool URInventoryComponent::HasItem_Arch (const FRItemDataHandle &CheckItem) const
{
   // --- Create required item info
   FRItemData RequireItem;
   if (!CheckItem.ToItem (RequireItem)) return false;
   return HasItem_Data (RequireItem);
}

bool URInventoryComponent::HasItem_Data (FRItemData RequireItem) const
{
   // --- Iterate over inventory items
   for (const FRItemData &ItItem : Items) {
      if (ItItem.Name != RequireItem.Name) continue;
      RequireItem.Count -= ItItem.Count;
      if (RequireItem.Count <= 0) break;
   }
   return (RequireItem.Count <= 0);
}


bool URInventoryComponent::HasItems (const TArray<FRItemDataHandle> &CheckItems) const
{
   // --- Create list of required item infos
   TArray<FRItemData> RequiredItems;
   for (const FRItemDataHandle &ItItem : CheckItems) {
      FRItemData RequireItem;
      if (!ItItem.ToItem (RequireItem)) return false;
      RequiredItems.Add (RequireItem);
   }

   // --- Iterate over inventory items
   for (const FRItemData &ItItem : Items) {

      // --- Iterate over required items
      for (int iRequireItem = 0; iRequireItem < RequiredItems.Num (); iRequireItem++) {
         FRItemData &RequireItem = RequiredItems[iRequireItem];
         if (ItItem.Name != RequireItem.Name) continue;

         RequireItem.Count -= ItItem.Count;
         if (RequireItem.Count <= 0) {
            RequiredItems.RemoveAt (iRequireItem);
            break;
         };
      }
   }

   return RequiredItems.Num () == 0;
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
   R_RETURN_IF_NOT_ADMIN_BOOL;
   FRItemData newItem;
   if (!ItemHandle.ToItem (newItem)) return false;
   return AddItem (newItem);
}

void URInventoryComponent::AddItem_Server_Implementation (URInventoryComponent *SrcInventory,
                                                          FRItemData NewItem) const
{
   if (!ensure (IsValid (SrcInventory))) return;
   SrcInventory->AddItem (NewItem);
}
bool URInventoryComponent::AddItem (FRItemData NewItem)
{
   R_RETURN_IF_NOT_ADMIN_BOOL;

   // --- Limit weight being used
   int32 WeightAdd = NewItem.Count * NewItem.Weight;
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
            ReportInventoryUpdate ();
            return true;
         }

         // --- Partial fit
          ItItem.Count += ItItemCountLeft;
         NewItem.Count -= ItItemCountLeft;
      }
   }

   // --- Check if overflow. For default items.
   if (NewItem.Count > NewItem.MaxCount) {
      int nNewInstance = NewItem.Count / NewItem.MaxCount - 1;
      for (int iNewInstance = 0; iNewInstance < nNewInstance; iNewInstance++) {

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
      ReportInventoryUpdate ();
      return false;
   }

   Items.Add (NewItem);
   CalcWeight ();
   ReportInventoryUpdate ();
   return true;
}

//=============================================================================
//                 Remove Item index
//=============================================================================

void URInventoryComponent::RemoveItem_Index_Server_Implementation (URInventoryComponent *SrcInventory,
                                                                   int32 ItemIdx, int32 Count) const
{
   if (!ensure (IsValid (SrcInventory))) return;
   SrcInventory->RemoveItem_Index (ItemIdx, Count);
}
bool URInventoryComponent::RemoveItem_Index (int32 ItemIdx, int32 Count)
{
   R_RETURN_IF_NOT_ADMIN_BOOL;

   if (!Items.IsValidIndex (ItemIdx)) {
      R_LOG_PRINTF ("Invalid Inventory Item Index [%d]. Must be [0-%d]",
         ItemIdx, Items.Num ());
      return false;
   }

   if (Items[ItemIdx].Count > Count) {
      Items[ItemIdx].Count -= Count;
   } else {
      Items.RemoveAt (ItemIdx);
   }

   CalcWeight ();
   ReportInventoryUpdate ();
   return true;
}

//=============================================================================
//                 Remove Item Arch
//=============================================================================

void URInventoryComponent::RemoveItem_Arch_Server_Implementation (URInventoryComponent *SrcInventory,
                                                                  const FRItemDataHandle &RmItemHandle) const
{
   if (!ensure (IsValid (SrcInventory))) return;
   SrcInventory->RemoveItem_Arch (RmItemHandle);
}
bool URInventoryComponent::RemoveItem_Arch (const FRItemDataHandle &RmItemHandle)
{
   R_RETURN_IF_NOT_ADMIN_BOOL;
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
   if (!ensure (IsValid (SrcInventory))) return;
   SrcInventory->RemoveItem_Data (RmItemData);
}

bool URInventoryComponent::RemoveItem_Data (FRItemData RmItemData)
{
   R_RETURN_IF_NOT_ADMIN_BOOL;
   // Last check that user has required item
   if (!HasItem_Data (RmItemData)) return false;

   // Remove until finished
   while (RmItemData.Count > 0) {
      for (int iItem = 0; iItem < Items.Num (); iItem++) {
         if (Items[iItem].Name != RmItemData.Name) continue;

         int Count = FMath::Min (Items[iItem].Count, RmItemData.Count);
         if (!RemoveItem_Index (iItem, Count)) return false;
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
   if (!ensure (IsValid (SrcInventory))) return;
   if (!ensure (IsValid (DstInventory))) return;
   SrcInventory->TransferAll (DstInventory);
}
bool URInventoryComponent::TransferAll (URInventoryComponent *DstInventory)
{
   R_RETURN_IF_NOT_ADMIN_BOOL;
   if (!ensure (IsValid (DstInventory))) return false;
   int nItems = Items.Num ();

   bool success = true;
   int32 SrcItemIdx = 0;
   for (int iItem = 0; iItem < nItems; iItem++) {
      if (!TransferItem (DstInventory, SrcItemIdx, 0)) {
         success = false;
         SrcItemIdx++; // Move to next one
      }
   }
   return success;
}

void URInventoryComponent::TransferItem_Server_Implementation (URInventoryComponent *SrcInventory,
                                                               URInventoryComponent *DstInventory,
                                                               int32 SrcItemIdx,
                                                               int32 SrcItemCount) const
{
   if (!ensure (IsValid (SrcInventory))) return;
   if (!ensure (IsValid (DstInventory))) return;
   SrcInventory->TransferItem (DstInventory, SrcItemIdx, SrcItemCount);
}
bool URInventoryComponent::TransferItem (URInventoryComponent *DstInventory,
                                         int32 SrcItemIdx,
                                         int32 SrcItemCount)
{
   R_RETURN_IF_NOT_ADMIN_BOOL;
   if (!ensure (IsValid (DstInventory))) return false;

   if (!Items.IsValidIndex (SrcItemIdx)) {
      R_LOG_PRINTF ("Invalid Source Inventory Item Index [%d]. Must be [0-%d]",
         SrcItemIdx, Items.Num ());
      return false;
   }

   FRItemData ItemData = Items[SrcItemIdx];

   // All items
   if (SrcItemCount <= 0) {
      SrcItemCount = ItemData.Count;

      // Downscale if not enough space
      int32 WeightAvailable = DstInventory->WeightMax - DstInventory->WeightCurrent ;
      int32 WeightRequired  = ItemData.Count * ItemData.Weight;
      if (WeightAvailable < WeightRequired) {
         SrcItemCount = WeightAvailable / ItemData.Weight;
      }
   }

   // Can't fit anything
   if (SrcItemCount == 0) return false;

   ItemData.Count = SrcItemCount;

   // Try to add item
   if (!DstInventory->AddItem (ItemData)) return false;

   // Cleanup
   return RemoveItem_Index (SrcItemIdx, SrcItemCount);
}

void URInventoryComponent::CalcWeight ()
{
   R_RETURN_IF_NOT_ADMIN;
   int32 WeightNew = 0;
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
   if (!ensure (IsValid (DstInventory))) return;
   DstInventory->UseItem (ItemIdx);
}
bool URInventoryComponent::UseItem (int32 ItemIdx)
{
   R_RETURN_IF_NOT_ADMIN_BOOL;
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

   if (ItemData.Used (GetOwner (), this)) return false;

   BP_Used (ItemIdx);

   // Consumable
   if (ItemData.DestroyOnAction) return RemoveItem_Index (ItemIdx, 1);
   return true;
}

void URInventoryComponent::DropItem_Server_Implementation (URInventoryComponent *SrcInventory,
                                                           int32 ItemIdx, int32 Count) const
{
   if (!ensure (IsValid (SrcInventory))) return;
   SrcInventory->DropItem (ItemIdx, Count);
}
ARItemPickup* URInventoryComponent::DropItem (int32 ItemIdx, int32 Count)
{
   R_RETURN_IF_NOT_ADMIN_NULL;

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
         // TODO
         //Pickup->MeshComponent->SetStaticMesh (ItemData.PickupMesh);
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
//                 Save / Load
//=============================================================================

void URInventoryComponent::OnSave (FBufferArchive &SaveData)
{
   // Convert ItemData to array to JSON strings
   TArray<FString> ItemDataRaw;
   for (FRItemData ItItem : Items) {
      ItemDataRaw.Add (ItItem.GetJSON ());
   }
   SaveData << ItemDataRaw;
}

void URInventoryComponent::OnLoad (FMemoryReader &LoadData)
{
   TArray<FString> ItemDataRaw;
   LoadData << ItemDataRaw;

   // Convert JSON strings to ItemData
   TArray<FRItemData> LoadedItems;
   for (const FString &ItItemRaw : ItemDataRaw) {

      FRItemData Item;
      Item.SetJSON (ItItemRaw);
      if (!Item.ReadJSON ()) {
         R_LOG_PRINTF ("Failed to parse Item string [%s]", *ItItemRaw);
         continue;
      }
      LoadedItems.Add (Item);
   }

   // Update inventory
   Items = LoadedItems;
   ReportInventoryUpdate ();
}

