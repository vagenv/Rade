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
   if (bSaveLoadInventory && bIsServer) {
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
	GetWorld()->GetTimerManager ().ClearTimer (TimerClosestPickup);
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

bool URInventoryComponent::AddItem_Arch (const FRDefaultItem &ItemData)
{
   if (!bIsServer) {
      R_LOG ("Client has no authority to perform this action.");
      return false;
   }

   FRItemData newItem;
   if (!FRItemData::FromRow (ItemData.Arch, newItem)) return false;
   newItem.Count = ItemData.Count;
   return AddItem (newItem);
}

void URInventoryComponent::AddItem_Server_Implementation (FRItemData NewItem)
{
   AddItem (NewItem);
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

void URInventoryComponent::RemoveItem_Server_Implementation (int32 ItemIdx, int32 Count)
{
   RemoveItem (ItemIdx, Count);
}
bool URInventoryComponent::RemoveItem (int32 ItemIdx, int32 Count)
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

   OnInventoryUpdated.Broadcast ();
   return true;
}

void URInventoryComponent::TransferAll_Server_Implementation (URInventoryComponent *DstInventory)
{
   TransferAll (DstInventory);
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

void URInventoryComponent::TransferItem_Server_Implementation (URInventoryComponent *DstInventory,
                                                               int32 SrcItemIdx,
                                                               int32 SrcItemCount)
{
   TransferItem (DstInventory, SrcItemIdx, SrcItemCount);
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
   return RemoveItem (SrcItemIdx, SrcItemCount);
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

void URInventoryComponent::UseItem_Server_Implementation (int32 ItemIdx)
{
   UseItem (ItemIdx);
}
bool URInventoryComponent::UseItem (int32 ItemIdx)
{
   if (!bIsServer) {
      R_LOG ("Client has no authority to perform this action.");
      return false;
   }

   // Valid index
   if (!Items.IsValidIndex (ItemIdx)) {
      R_LOG_PRINTF ("Invalid Inventory Item Index [%d]. Must be [0-%d]",
         ItemIdx, Items.Num ());
      return nullptr;
   }

   FRItemData ItemData = Items[ItemIdx];

   // valid archetype
   if (!ItemData.Action) return false;


   URItemAction *ItemBP = ItemData.Action->GetDefaultObject<URItemAction>();

   if (!ensure (ItemBP)) return false;
   ItemBP->Used (this, ItemData, ItemIdx);
   BP_Used (ItemIdx);

   // Consumable
   return RemoveItem (ItemIdx, 1);
}

void URInventoryComponent::DropItem_Server_Implementation (int32 ItemIdx, int32 Count)
{
   DropItem (ItemIdx, Count);
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
   if (!RemoveItem (ItemIdx, Count)) return nullptr;

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
   }

   // --- Save player Inventory

   // Convert ItemData to array to JSON strings
   TArray<FString> ItemData;
   for (const FRItemData &item : Items) {
      FString res;
      if (FRItemData::ToJSON (item, res)) {
         ItemData.Add (res);
      } else {
         R_LOG_PRINTF ("Failed to save %s", *item.Name);
      }
   }

   // Convert array into buffer
   FBufferArchive ToBinary;
   ToBinary << ItemData;

   FString InventoryUniqueId = GetOwner ()->GetName ();

   // Set binary data to save file
   if (!URSaveMgr::Set (GetWorld (), InventoryUniqueId, ToBinary)) {
      R_LOG_PRINTF ("Failed to save [%s] Inventory.", *InventoryUniqueId);
   }
}

void URInventoryComponent::OnLoad ()
{
   if (!bIsServer) {
      R_LOG ("Client has no authority to perform this action.");
   }
   // --- Load player Inventory

   FString InventoryUniqueId = GetOwner ()->GetName ();

   // Get binary data from save file
   TArray<uint8> BinaryArray;
   if (!URSaveMgr::Get (GetWorld (), InventoryUniqueId, BinaryArray)) {
      R_LOG_PRINTF ("Failed to load [%s] Inventory.", *InventoryUniqueId);
      return;
   }

   // Convert Binary to array of JSON strings
   TArray<FString> ItemsData;
   FMemoryReader FromBinary = FMemoryReader (BinaryArray, true);
   FromBinary.Seek(0);
   FromBinary << ItemsData;

   // Convert JSON strings to ItemData
   TArray<FRItemData> loadedItems;
   for (const FString &ItemData : ItemsData) {
      FRItemData Item;
      if (FRItemData::FromJSON (ItemData, Item)) {
         loadedItems.Add (Item);
      } else {
         R_LOG_PRINTF ("Failed to parse Item string [%s]", *ItemData);
      }
   }

   // Update inventory
   Items = loadedItems;
   OnInventoryUpdated.Broadcast ();
}


/*

// Throw out all Items
void URInventoryComponent::ThrowOutAllItems()
{
   UWorld* const World = TheCharacter->GetWorld();
   for (int32 ItemIndex = 0; ItemIndex < Items.Num(); ItemIndex++) {
      // Inside the list of items
      if (Items.IsValidIndex(ItemIndex) && TheCharacter) {

         if (  World
            && Items[ItemIndex].Archetype
            && Items[ItemIndex].Archetype->GetDefaultObject<AItem>())
         {

            // Get Player Rotation
            FRotator rot      = TheCharacter->GetActorRotation();
            FVector  spawnLoc = TheCharacter->GetActorLocation()
                              + rot.Vector() * 200 + FVector(FMath::RandRange(-200, 200), FMath::RandRange(-200, 200), 50);

            AItemPickup* newPickup;
            AItem* newItem = Items[ItemIndex].Archetype->GetDefaultObject<AItem>();

            // Spawn Item Pickup archetype
            if (newItem && newItem->ItemPickupArchetype)
                 newPickup = World->SpawnActor<AItemPickup>(newItem->ItemPickupArchetype, spawnLoc, rot);

            // Spawn Default pickup
            else newPickup = World->SpawnActor<AItemPickup>(AItemPickup::StaticClass(), spawnLoc, rot);

            if (newPickup) {
               newPickup->SetReplicates(true);
               if (newItem) {
                  //printr("Death Item "+newItem->ItemName);
                  newItem->BP_ItemDroped(newPickup);
                  newItem->ItemCount = Items[ItemIndex].ItemCount;
                  newPickup->Item = newItem->GetClass();

                  if      (newItem->PickupMesh)     newPickup->Mesh->SetStaticMesh(newItem->PickupMesh);
                  else if (newItem->PickupSkelMesh) newPickup->SkeletalMesh->SetSkeletalMesh(newItem->PickupSkelMesh);
               }

               newPickup->bAutoPickup = true;
               newPickup->ActivatePickupPhysics();
               if (newPickup->Mesh && newPickup->Mesh->IsSimulatingPhysics())
                  newPickup->Mesh->AddImpulse(rot.Vector() * 120, NAME_None, true);

               if (newPickup->SkeletalMesh && newPickup->SkeletalMesh->IsSimulatingPhysics())
                  newPickup->SkeletalMesh->AddForce(rot.Vector() * 12000, NAME_None, true);
            }
         }
      }
   }
   Items.Empty();
}
*/

