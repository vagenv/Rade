// Copyright 2015-2023 Vagen Ayrapetyan

#include "RInventoryComponent.h"
#include "RItemPickup.h"

#include "RUtilLib/RLog.h"
#include "RUtilLib/RUtil.h"
#include "RUtilLib/RCheck.h"
#include "RUtilLib/RTimer.h"
#include "RUtilLib/RWorldAssetMgr.h"
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

   if (R_IS_NET_ADMIN) {
      for (const FRItemDataHandle &ItItem : DefaultItems) {
         if (!AddItem_Handle (ItItem))
            R_LOG_PRINTF ("Failed to add default item [%s] to [%s]",
               *ItItem.Arch.RowName.ToString (), *GetOwner()->GetName ());
      }
   }
   ConnectToSaveMgr ();
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


bool URInventoryComponent::HasItem_ID (const FString &ItemId, int32 Count) const
{
   return GetItemCount_ID (ItemId) >= Count;
}

bool URInventoryComponent::HasItem_Data (const FRItemData &ItemData) const
{
   if (!ItemData.IsValid ()) return false;
   return HasItem_ID (ItemData.ID, ItemData.Count);
}

bool URInventoryComponent::HasItem_Handle (const FRItemDataHandle &ItemHandle) const
{
   FRItemData ItemData;
   if (!ItemHandle.ToItem (ItemData)) return false;
   return HasItem_Data (ItemData);
}

bool URInventoryComponent::HasItems_Handles (const TArray<FRItemDataHandle> &ItemHandles) const
{
   TMap<FString, int> RequiredItems;

   // Merge the list into ID + Total Count
   for (const FRItemDataHandle &ItItem : ItemHandles) {
      FRItemData ItemData;
      if (!ItItem.ToItem (ItemData)) return false;
      if (RequiredItems.Contains (ItemData.ID)) RequiredItems[ItemData.ID] += ItemData.Count;
      else                                          RequiredItems.Add (ItemData.ID, ItemData.Count);
   }

   for (const auto &ItItem : RequiredItems) {
      if (!HasItem_ID (ItItem.Key, ItItem.Value)) return false;
   }

   return true;
}

//=============================================================================
//                 Get Count
//=============================================================================

int URInventoryComponent::GetItemCount_ID (const FString &ItemId) const
{
   int Count = 0;
   // --- Find same kind of item
   for (const FRItemData &ItItem : Items) {
      // Not same item type
      if (ItItem.ID != ItemId) continue;
      Count += ItItem.Count;
   }

   return Count;
}

int URInventoryComponent::GetItemCount_Data (const FRItemData &ItemData) const
{
   if (!ItemData.IsValid ()) return 0;
   return GetItemCount_ID (ItemData.ID);
}

int URInventoryComponent::GetItemCount_Handle (const FRItemDataHandle &ItemHandle) const
{
   FRItemData ItemData;
   if (!ItemHandle.ToItem (ItemData)) return 0;
   return GetItemCount_Data (ItemData);
}

//=============================================================================
//                 Add item
//=============================================================================

bool URInventoryComponent::AddItem_Handle (const FRItemDataHandle &ItemHandle)
{
   R_RETURN_IF_NOT_ADMIN_BOOL;
   FRItemData ItemData;
   if (!ItemHandle.ToItem (ItemData)) return false;
   return AddItem_Data (ItemData);
}

void URInventoryComponent::AddItem_Data_Server_Implementation (
   URInventoryComponent *SrcInventory,
   FRItemData            ItemData) const
{
   if (!ensure (SrcInventory)) return;
   SrcInventory->AddItem_Data (ItemData);
}
bool URInventoryComponent::AddItem_Data (FRItemData NewItem)
{
   R_RETURN_IF_NOT_ADMIN_BOOL;

   if (!NewItem.IsValid ()) return false;

   // --- Limit weight being used
   int32 WeightAdd = NewItem.Count * NewItem.Weight;
   if (WeightCurrent + WeightAdd > WeightMax) return false;

   // --- Check if item is stackable
   if (NewItem.MaxCount > 1) {

      // --- Find same kind of item
      for (FRItemData &ItItem : Items) {

         // Not same item type
         if (ItItem.ID != NewItem.ID) continue;

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
//                 Remove Item
//=============================================================================

void URInventoryComponent::RemoveItem_Index_Server_Implementation (URInventoryComponent *SrcInventory,
                                                                   int32 ItemIdx, int32 Count) const
{
   if (!ensure (SrcInventory)) return;
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


void URInventoryComponent::RemoveItem_Handle_Server_Implementation (
   URInventoryComponent   *SrcInventory,
   const FRItemDataHandle &ItemHandle) const
{
   if (!ensure (SrcInventory)) return;
   SrcInventory->RemoveItem_Handle (ItemHandle);
}
bool URInventoryComponent::RemoveItem_Handle (const FRItemDataHandle &ItemHandle)
{
   R_RETURN_IF_NOT_ADMIN_BOOL;
   FRItemData ItemData;
   if (!ItemHandle.ToItem (ItemData)) return false;
   return RemoveItem_Data (ItemData);
}

void URInventoryComponent::RemoveItem_Data_Server_Implementation (
   URInventoryComponent *SrcInventory,
   FRItemData            ItemData) const
{
   if (!ensure (SrcInventory)) return;
   SrcInventory->RemoveItem_Data (ItemData);
}

bool URInventoryComponent::RemoveItem_Data (FRItemData ItemData)
{
   R_RETURN_IF_NOT_ADMIN_BOOL;

   if (!ItemData.IsValid ()) return false;

   // Last check that user has required item
   if (!HasItem_Data (ItemData)) return false;

   // Remove until finished
   while (ItemData.Count > 0) {
      for (int iItem = 0; iItem < Items.Num (); iItem++) {
         if (Items[iItem].ID != ItemData.ID) continue;

         int Count = FMath::Min (Items[iItem].Count, ItemData.Count);
         if (!RemoveItem_Index (iItem, Count)) return false;
         ItemData.Count -= Count;
      }
   }

   return (ItemData.Count == 0);
}


//=============================================================================
//                 Transfer
//=============================================================================

void URInventoryComponent::TransferAll_Server_Implementation (URInventoryComponent *SrcInventory,
                                                              URInventoryComponent *DstInventory) const
{
   if (!ensure (SrcInventory)) return;
   if (!ensure (DstInventory)) return;
   SrcInventory->TransferAll (DstInventory);
}
bool URInventoryComponent::TransferAll (URInventoryComponent *DstInventory)
{
   R_RETURN_IF_NOT_ADMIN_BOOL;
   if (!ensure (DstInventory)) return false;
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

void URInventoryComponent::TransferItem_Server_Implementation (
   URInventoryComponent *SrcInventory,
   URInventoryComponent *DstInventory,
   int32                 SrcItemIdx,
   int32                 SrcItemCount) const
{
   if (!ensure (SrcInventory)) return;
   if (!ensure (DstInventory)) return;
   SrcInventory->TransferItem (DstInventory, SrcItemIdx, SrcItemCount);
}
bool URInventoryComponent::TransferItem (URInventoryComponent *DstInventory,
                                         int32 SrcItemIdx,
                                         int32 SrcItemCount)
{
   R_RETURN_IF_NOT_ADMIN_BOOL;
   if (!ensure (DstInventory)) return false;

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
   if (!DstInventory->AddItem_Data (ItemData)) return false;

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
//                 Break Item
//=============================================================================

void URInventoryComponent::BreakItem_Index_Server_Implementation (
   URInventoryComponent *SrcInventory,
   int32                 ItemIdx,
   const UDataTable     *BreakItemTable) const
{
   if (!ensure (SrcInventory)) return;
   SrcInventory->BreakItem_Index (ItemIdx, BreakItemTable);
}

bool URInventoryComponent::BreakItem_Index (int32 ItemIdx,
                                            const UDataTable* BreakItemTable)
{
   R_RETURN_IF_NOT_ADMIN_BOOL;
   // Valid index
   if (!Items.IsValidIndex (ItemIdx)) {
      R_LOG_PRINTF ("Invalid Item Index [%d]. Must be [0-%d]",
         ItemIdx, Items.Num ());
      return false;
   }

   if (!IsValid (BreakItemTable)) return false;

   FRItemData BreakItem = Items[ItemIdx];

   FRCraftRecipe Recipe;
   if (!URItemUtilLibrary::Item_GetRecipe (BreakItemTable, BreakItem, Recipe)) return false;

   // Remove break item and add parts
   RemoveItem_Index (ItemIdx);
   for (const auto &ItAddItem : Recipe.RequiredItems) {
      AddItem_Handle (ItAddItem);
   }
   return true;

}

bool URInventoryComponent::BreakItem_Data (const FRItemData &ItemData,
                                           const UDataTable* BreakItemTable)
{
   R_RETURN_IF_NOT_ADMIN_BOOL;
   if (!ItemData.IsValid ()) return false;

   for (int iItem = 0; iItem < Items.Num (); iItem++) {
      if (Items[iItem].ID == ItemData.ID) {
         return BreakItem_Index (iItem, BreakItemTable);
      }
   }
   return false;
}

//=============================================================================
//                 Craft Item
//=============================================================================

bool URInventoryComponent::CanCraftItem (const FDataTableRowHandle& CraftItem)
{
   FString ContextString;
   FRCraftRecipe* Recipe = CraftItem.GetRow<FRCraftRecipe>(ContextString);

   if (!Recipe) return false;

   FRItemData ToCraftItem;
   if (!Recipe->CreateItem.ToItem (ToCraftItem)) return false;

   if (!HasItems_Handles (Recipe->RequiredItems)) return false;
   return true;
}

void URInventoryComponent::CraftItem_Server_Implementation (
         URInventoryComponent      *SrcInventory,
         const FDataTableRowHandle &CraftItem) const
{
   if (!ensure (SrcInventory)) return;
   SrcInventory->CraftItem (CraftItem);
}

bool URInventoryComponent::CraftItem (const FDataTableRowHandle &CraftItem)
{
   R_RETURN_IF_NOT_ADMIN_BOOL;

   FString ContextString;
   FRCraftRecipe* Recipe = CraftItem.GetRow<FRCraftRecipe> (ContextString);

   if (!Recipe) return false;

   if (!HasItems_Handles (Recipe->RequiredItems)) return false;

   for (const auto &ItRmItem : Recipe->RequiredItems) {
      RemoveItem_Handle (ItRmItem);
   }

   AddItem_Handle (Recipe->CreateItem);

   return true;
}

//=============================================================================
//                 Use item
//=============================================================================

void URInventoryComponent::UseItem_Index_Server_Implementation (
   URInventoryComponent *DstInventory,
   int32                 ItemIdx) const
{
   if (!ensure (DstInventory)) return;
   DstInventory->UseItem_Index (ItemIdx);
}
bool URInventoryComponent::UseItem_Index (int32 ItemIdx)
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

   // Consumable
   if (ItemData.DestroyOnAction) return RemoveItem_Index (ItemIdx, 1);
   return true;
}

bool URInventoryComponent::UseItem_Data (const FRItemData &ItemData)
{
   R_RETURN_IF_NOT_ADMIN_BOOL;
   if (!ItemData.IsValid ()) return false;

   for (int iItem = 0; iItem < Items.Num (); iItem++) {
      if (Items[iItem].ID == ItemData.ID) {
         return UseItem_Index (iItem);
      }
   }
   return false;
}

//=============================================================================
//                 Drop item
//=============================================================================


void URInventoryComponent::DropItem_Index_Server_Implementation (
   URInventoryComponent *SrcInventory,
   int32                 ItemIdx,
   int32                 Count) const
{
   if (!ensure (SrcInventory)) return;
   SrcInventory->DropItem_Index (ItemIdx, Count);
}

bool URInventoryComponent::DropItem_Data (const FRItemData &ItemData)
{
   R_RETURN_IF_NOT_ADMIN_BOOL;
   if (!ItemData.IsValid ()) return false;

   for (int iItem = 0; iItem < Items.Num (); iItem++) {
      if (Items[iItem].ID == ItemData.ID) {
         return DropItem_Index (iItem);
      }
   }
   return false;
}

bool URInventoryComponent::DropItem_Index (int32 ItemIdx, int32 Count)
{
   R_RETURN_IF_NOT_ADMIN_BOOL;

   // Valid index
   if (!Items.IsValidIndex (ItemIdx)) {
      R_LOG_PRINTF ("Invalid Inventory Item Index [%d]. Must be [0-%d]",
         ItemIdx, Items.Num ());
      return false;
   }

   FRItemData ItemData = Items[ItemIdx];

   // No Item drop type set
   if (ItemData.Pickup.IsNull () && ItemData.PickupMesh.IsNull () && FallbackDropItemClass.IsNull ()) {
      R_LOG_PRINTF ("Neither pickup class or mesh specified for %s", *ItemData.Description.Label);
      return false;
   }

   // Everything
   if (Count <= 0)             Count = ItemData.Count;
   // Overflow
   if (Count > ItemData.Count) Count = ItemData.Count;

   ItemData.Count = Count;

   // Get async load object
   URWorldAssetMgr* WorldAssetMgr = URWorldAssetMgr::GetInstance (this);
   if (!WorldAssetMgr) return false;

   // Check that async load is not already in process
   if (PickupLoadHandle.IsValid ()) return false;

   // Error will be logged.
   if (!RemoveItem_Index (ItemIdx, Count)) return false;

   // Load pickup class async
   if (!ItemData.Pickup.IsNull ()) {

      PickupLoadHandle = WorldAssetMgr->StreamableManager.RequestAsyncLoad (ItemData.Pickup.GetUniqueID (),
         [this, ItemData] () {
            if (!PickupLoadHandle.IsValid ()) return;
            if (PickupLoadHandle->HasLoadCompleted ()) {
               if (UObject* Obj = PickupLoadHandle->GetLoadedAsset ()) {
                  if (UClass* PickupClass = Cast<UClass> (Obj)) {
                     SpawnPickup (PickupClass, ItemData);
                  }
               }
            }

            // Release data from memory
            PickupLoadHandle->ReleaseHandle ();
            PickupLoadHandle.Reset ();
         });

   // Load pickup model async
   } else if (!ItemData.PickupMesh.IsNull ()) {

      PickupLoadHandle = WorldAssetMgr->StreamableManager.RequestAsyncLoad (ItemData.PickupMesh.GetUniqueID (),
         [this, ItemData] () {
            if (!PickupLoadHandle.IsValid ()) return;
            if (PickupLoadHandle->HasLoadCompleted ()) {
               if (UObject* Obj = PickupLoadHandle->GetLoadedAsset ()) {
                  if (UStaticMesh* PickupMesh = Cast<UStaticMesh> (Obj)) {
                     ARItemPickup* Pickup = SpawnPickup (ARItemPickup::StaticClass (), ItemData);
                     if (Pickup) Pickup->MeshComponent->SetStaticMesh (PickupMesh);
                  }
               }
            }

            // Release data from memory
            PickupLoadHandle->ReleaseHandle ();
            PickupLoadHandle.Reset ();
         });

   // Load custom inventory drop pickup class async
   } else if (!FallbackDropItemClass.IsNull ()) {

      PickupLoadHandle = WorldAssetMgr->StreamableManager.RequestAsyncLoad (FallbackDropItemClass.GetUniqueID (),
         [this, ItemData] () {
            if (!PickupLoadHandle.IsValid ()) return;
            if (PickupLoadHandle->HasLoadCompleted ()) {
               if (UObject* Obj = PickupLoadHandle->GetLoadedAsset ()) {
                  if (UClass* PickupClass = Cast<UClass> (Obj)) {
                     SpawnPickup (PickupClass, ItemData);
                  }
               }
            }

            // Release data from memory
            PickupLoadHandle->ReleaseHandle ();
            PickupLoadHandle.Reset ();
         });
   }

   return true;
}

ARItemPickup* URInventoryComponent::SpawnPickup (TSubclassOf<ARItemPickup> PickupClass, FRItemData ItemData)
{
   UWorld* World = URUtil::GetWorld (this);
   if (!World) return nullptr;

   AActor *Player = GetOwner ();

   // Get Player Rotation
   FRotator rot = Player->GetActorRotation ();
   FVector forwardVector   = rot.Vector () * 300;
           forwardVector.Z = 0;
   FVector spawnLoc = Player->GetActorLocation () + forwardVector + FVector(0, 0, 50);

   // Create pickup
   ARItemPickup *Pickup = World->SpawnActor<ARItemPickup> (PickupClass, spawnLoc, rot);
   if (!Pickup) return nullptr;

   // Set pickup info
   Pickup->SetOwner (Player);
   Pickup->bAutoPickup  = false;
   Pickup->bAutoDestroy = true;

   Pickup->Inventory->DefaultItems.Empty ();
   Pickup->Inventory->Items.Empty ();
   Pickup->Inventory->Items.Add (ItemData);

   return Pickup;
}

//=============================================================================
//                 Save / Load
//=============================================================================

void URInventoryComponent::ConnectToSaveMgr ()
{
	if (!bSaveLoad || !R_IS_NET_ADMIN) return;

   // Careful with collision of 'UniqueSaveId'
   FString UniqueSaveId = GetOwner ()->GetName () + "_InventoryMgr";

	if (!InitSaveInterface (this, UniqueSaveId)) {
		FTimerHandle RetryHandle;
		RTIMER_START (RetryHandle, this, &URInventoryComponent::ConnectToSaveMgr, 1, false);
	}
}

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

