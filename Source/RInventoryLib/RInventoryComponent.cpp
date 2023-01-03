// Copyright 2015-2023 Vagen Ayrapetyan

#include "RInventoryComponent.h"
#include "RUtilLib/RLog.h"
#include "RSaveLib/RSaveMgr.h"

// #include "RItem.h"
#include "RItemPickup.h"
#include "Net/UnrealNetwork.h"


//=============================================================================
//                 Editor
//=============================================================================


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

   // DOREPLIFETIME (URInventoryComponent, Capacity);
   // DOREPLIFETIME (URInventoryComponent, TotalWeight);
   // DOREPLIFETIME (URInventoryComponent, Items);
   // DOREPLIFETIME (URInventoryComponent, CurrentPickups);
}

void URInventoryComponent::BeginPlay()
{
   Super::BeginPlay();
   const UWorld *world = GetWorld ();
   if (!ensure (world)) return;

   bIsServer = GetOwner ()->HasAuthority ();

   if (!bIsServer) return;

   // Save/Load inventory
   if (bSaveLoadInventory) {
      FRSaveEvent SavedDelegate;
      SavedDelegate.AddDynamic (this, &URInventoryComponent::OnSave);
      URSaveMgr::OnSave (world, SavedDelegate);

      FRSaveEvent LoadedDelegate;
      LoadedDelegate.AddDynamic (this, &URInventoryComponent::OnLoad);
      URSaveMgr::OnLoad (world, LoadedDelegate);
   }

   for (const auto &itItem : DefaultItems) {
      if (!AddItem_Arch (itItem))
         R_LOG (FString::Printf (TEXT ("Failed to add default item [%s] to [%s]"),
                *itItem.Arch.RowName.ToString (), *GetOwner()->GetName ()));
   }

   if (bCheckClosestPickup) {
      world->GetTimerManager ().SetTimer (TimerClosestPickup,
         this, &URInventoryComponent::CheckClosestPickup, CheckClosestDelay, true);
   }

}

//=============================================================================
//                 Add Item
//=============================================================================

TArray<FRItemData> URInventoryComponent::GetItems () const
{
   return Items;
}

// void URInventoryComponent::OnRep_Items ()
// {
//    OnInventoryUpdated.Broadcast ();
// }

bool URInventoryComponent::AddItem_Arch (const FRDefaultItem &ItemData)
{
   FRItemData newItem;
   if (!FRItemData::FromRow (ItemData.Arch, newItem)) return false;
   newItem.Count = ItemData.Count;
   return AddItem (newItem);
}

bool URInventoryComponent::AddItem (FRItemData NewItem)
{
   if (!bIsServer) return false;

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
      return false;
   }

   Items.Add (NewItem);
   CalcWeight ();
   OnInventoryUpdated.Broadcast ();
   return true;
}


bool URInventoryComponent::AddItem_Pickup (ARItemPickup *Pickup)
{
   if (!bIsServer) return false;
   // //R_LOG ("Add item pickup");
   if (!ensure (Pickup))            return false;
   if (!ensure (Pickup->Inventory)) return false;

   TArray<FRItemData> ItemsLeft;
   for (const FRItemData &ItItem : Pickup->Inventory->Items) {
      bool res = AddItem (ItItem);
      if (!res) ItemsLeft.Add (ItItem);
   }

   Pickup->Inventory->Items = ItemsLeft;
   Pickup->PickedUp (GetOwner());

   return true;
}

bool URInventoryComponent::RemoveItem (int32 ItemIdx, int32 Count)
{
   if (!bIsServer) return false;
   if (!Items.IsValidIndex (ItemIdx)) return false;

   if (Items[ItemIdx].Count > Count) {
      Items[ItemIdx].Count -= Count;
   } else {
      Items.RemoveAt (ItemIdx);
   }

   OnInventoryUpdated.Broadcast ();
   return true;
}

bool URInventoryComponent::TransferItem (URInventoryComponent *FromInventory,
                                         URInventoryComponent *ToInventory,
                                         int32 FromItemIdx,
                                         int32 FromItemCount)
{
   if (!FromInventory) return false;
   if (!ToInventory)   return false;

   if (!FromInventory->Items.IsValidIndex (FromItemIdx)) return false;

   FRItemData ItemData = FromInventory->Items[FromItemIdx];

   // All items
   if (FromItemCount <= 0) FromItemCount = ItemData.Count;
   ItemData.Count = FromItemCount;

   // Try to add item
   if (!ToInventory->AddItem (ItemData)) return false;

   // Cleanup
   return FromInventory->RemoveItem (FromItemIdx, FromItemCount);
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


// //=============================================================================
// //                 Item action
// //=============================================================================


// bool URInventoryComponent::UseItem (int32 ItemIdx)
// {
//    if (!bIsServer) return false;

//    //R_LOG ("use item");
//    // valid idx
//    if (!Items.IsValidIndex (ItemIdx)) return false;
//    // valid archetype
//    if (!Items[ItemIdx].ItemArch) return false;
//    URItem *ItemBP = Items[ItemIdx].ItemArch->GetDefaultObject<URItem>();

//    BP_Used (ItemBP);
//    ItemBP->Used (GetOwner(), this);

//    return true;
// }

// ARItemPickup* URInventoryComponent::DropItem (int32 ItemIdx, int32 Count)
// {
//    if (!bIsServer) return nullptr;

//    //R_LOG ("drop item");
//    // valid idx
//    if (!Items.IsValidIndex (ItemIdx)) return false;
//    // valid archetype
//    if (!Items[ItemIdx].ItemArch) return false;
//    URItem *ItemBP = Items[ItemIdx].ItemArch->GetDefaultObject<URItem>();
//    if (!ItemBP) return false;

//    FRItemData ItemData = Items[ItemIdx];
//    if (Count > ItemData.Description.Count) Count = ItemData.Description.Count;
//    if (!RemoveItem (ItemIdx, Count)) return false;


//    AActor *Player = GetOwner ();
//    // Get Player Rotation
//    FRotator rot = Player->GetActorRotation();
//    FVector forwardVector = rot.Vector() * 300;
//            forwardVector.Z = 0;
//    FVector spawnLoc = Player->GetActorLocation() + forwardVector + FVector(0, 0, 50);

//    // Create pickup
//    ARItemPickup *Pickup = nullptr;

//    if (ItemBP->PickupArch) {
//       Pickup = GetWorld()->SpawnActor<ARItemPickup>(ItemBP->PickupArch, spawnLoc, rot);

//    } else {
//       Pickup = GetWorld()->SpawnActor<ARItemPickup>(ARItemPickup::StaticClass (), spawnLoc, rot);
//       Pickup->InitEmpty ();
//    }
//    Pickup->bAutoPickup  = false;
//    Pickup->bAutoDestroy = true;

//    Pickup->Inventory->DefaultItems.Empty ();
//    Pickup->Inventory->Items.Empty ();

//    ItemData.Description.Count = Count;
//    Pickup->Inventory->Items.Add (ItemData);

//    BP_Droped (ItemBP, Pickup);
//    ItemBP->Droped (GetOwner(), this, Pickup);

//    return Pickup;
// }


// //=============================================================================
// //                 Server
// //=============================================================================

// void URInventoryComponent::UseItem_Server_Implementation (int32 ItemIdx)
// {
//    UseItem (ItemIdx);
// }

// void URInventoryComponent::DropItem_Server_Implementation (int32 ItemIdx, int32 Count)
// {
//    DropItem (ItemIdx, Count);
// }

// void URInventoryComponent::RemoveItem_Server_Implementation (int32 ItemIdx, int32 Count)
// {
//    //R_LOG ("action");
//    RemoveItem (ItemIdx, Count);
// }

// void URInventoryComponent::AddItem_Server_Implementation (FRItemData ItemData)
// {
//    //R_LOG ("action");
//    AddItem (ItemData);
// }

// void URInventoryComponent::AddItem_Pickup_Server_Implementation (ARItemPickup *Pickup)
// {
//    //R_LOG ("action");
//    AddItem_Pickup (Pickup);
// }

// void URInventoryComponent::TransferItem_Server_Implementation (URInventoryComponent *FromInventory,
//                                                                URInventoryComponent *ToInventory,
//                                                                int32 FromItemIdx,
//                                                                int32 FromItemCount)
// {
//    if (!bIsServer) return;
//    URInventoryComponent::TransferItem (FromInventory, ToInventory, FromItemIdx, FromItemCount);
//    /*
//    if (!FromInventory) return;
//    if (!ToInventory)   return;
//    if (!bIsServer)     return;

//    //R_LOG ("Transfer item");
//    if (!FromInventory->Items.IsValidIndex (FromItemIdx)) return;

//    FItemData ItemData = FromInventory->Items[FromItemIdx];
//    ItemData.Description.Count = FromItemCount;

//    FromInventory->RemoveItem (FromItemIdx, FromItemCount);
//      ToInventory->AddItem    (ItemData);
//    */
// }



// //=============================================================================
// //                 Pickup
// //=============================================================================

// void URInventoryComponent::OnRep_CurrentPickups ()
// {
//    OnPickupsUpdated.Broadcast ();
// }

bool URInventoryComponent::Pickup_Add (const ARItemPickup* Pickup)
{
   if (Pickup == nullptr) return false;
   CurrentPickups.Add (Pickup);
   OnPickupListUpdated.Broadcast ();
   return true;
}

bool URInventoryComponent::Pickup_Rm  (const ARItemPickup* Pickup)
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
   // --- Save player Inventory

   // Convert ItemData to array of JSON strings
   // TArray<FString> ItemData;
   // for (FRItemData &item : Items) {
   //    ItemData.Add (item.ToJSON ());
   // }

   // // Push to buffer
   // FBufferArchive ToBinary;
   // ToBinary << ItemData;

   // FString InventoryUniqueId = GetOwner ()->GetName ();

   // // Set
   // bool res = URSaveMgr::Set (GetWorld (), InventoryUniqueId, ToBinary);

   // if (!res) R_LOG ("Failed to save player inventory");
}

void URInventoryComponent::OnLoad ()
{
   // --- Load player Inventory

   // FString InventoryUniqueId = GetOwner ()->GetName ();

   // TArray<uint8> BinaryArray;
   // bool res = URSaveMgr::Get (GetWorld (), InventoryUniqueId, BinaryArray);
   // if (!res) {
   //    R_LOG ("Failed to load player inventory");
   //    return;
   // }

   // // Convert Binary to array of JSON strings
   // TArray<FString> ItemsData;
   // FMemoryReader FromBinary = FMemoryReader (BinaryArray, true);
   // FromBinary.Seek(0);
   // FromBinary << ItemsData;

   // // Convert JSON strings to ItemData
   // TArray<FRItemData> loadedItems;
   // for (FString &ItemData : ItemsData) {
   //    FRItemData Item;
   //    Item.FromJSON (ItemData);
   //    loadedItems.Add (Item);
   // }

   // // Update inventory
   // Items = loadedItems;
   // OnInventoryUpdated.Broadcast ();
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

