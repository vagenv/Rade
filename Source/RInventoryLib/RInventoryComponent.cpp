// Copyright 2015-2023 Vagen Ayrapetyan

#include "RInventoryComponent.h"
#include "RItemTypes.h"
#include "RItem.h"
#include "RItemPickup.h"
#include "RUtilLib/RLog.h"
#include "RSaveLib/RSaveMgr.h"
#include "Net/UnrealNetwork.h"

// ----------------------------------------------------------------------------
//                 Core
// ----------------------------------------------------------------------------

URInventoryComponent::URInventoryComponent ()
{
   SetIsReplicatedByDefault (true);
   Capacity = 10;
}

// Replication
void URInventoryComponent::GetLifetimeReplicatedProps (TArray<FLifetimeProperty> &OutLifetimeProps) const
{
   Super::GetLifetimeReplicatedProps (OutLifetimeProps);

   DOREPLIFETIME (URInventoryComponent, Capacity);
   DOREPLIFETIME (URInventoryComponent, TotalWeight);
   DOREPLIFETIME (URInventoryComponent, Items);
   DOREPLIFETIME (URInventoryComponent, CurrentPickups);
}

void URInventoryComponent::BeginPlay()
{
   Super::BeginPlay();
   bIsServer = GetOwner ()->HasAuthority ();

   if (!bIsServer) return;

   // Save/Load inventory
   if (bSaveLoadInventory) {
      FRSaveEvent SavedDelegate;
      SavedDelegate.AddDynamic (this, &URInventoryComponent::OnSave);
      URSaveMgr::OnSave (GetWorld (), SavedDelegate);

      FRSaveEvent LoadedDelegate;
      LoadedDelegate.AddDynamic (this, &URInventoryComponent::OnLoad);
      URSaveMgr::OnLoad (GetWorld (), LoadedDelegate);
   }

   // Add default items
   for (auto &itItem : DefaultItems) {
      AddItem_Arch (itItem);
   }
}

// ----------------------------------------------------------------------------
//                 Add Item
// ----------------------------------------------------------------------------

const TArray<FRItemData> URInventoryComponent::GetItems () const
{
   return Items;
}

void URInventoryComponent::OnRep_Items ()
{
   OnInventoryUpdated.Broadcast ();
}

bool URInventoryComponent::AddItem (FRItemData ItemData)
{
   if (!bIsServer) return false;

   //rlog ("Add item");
   if (Items.Num () >= Capacity || !ItemData.ItemArch) return nullptr;

   // Check if item is stackable
   if (ItemData.Description.Count < ItemData.Description.MaxCount) {
      for (FRItemData &itData : Items) {

         // Wrong type
         // GetClass returns in all cases 'BlueprintGeneratedClass'
         if (itData.ItemArch != ItemData.ItemArch) continue;

         // Slot full
         if (itData.Description.Count > itData.Description.MaxCount) continue;

         // Full fit
         if (itData.Description.Count + ItemData.Description.Count < ItemData.Description.MaxCount) {

            itData.Description.Count += ItemData.Description.Count;
            OnInventoryUpdated.Broadcast ();
            return true;
         };

         // Partial fit
         int32 addCount = itData.Description.MaxCount - itData.Description.Count;
         if (ItemData.Description.Count < addCount) addCount = ItemData.Description.Count;

           itData.Description.Count += addCount;
         ItemData.Description.Count -= addCount;
      }
   }

   Items.Add (ItemData);
   OnInventoryUpdated.Broadcast ();
   return true;
}


bool URInventoryComponent::AddItem_Arch (TSubclassOf<URItem> Item)
{
   if (!bIsServer) return false;
   //rlog ("Add item arch");
   if (Items.Num () >= Capacity || !Item) return false;

   URItem *ItemBP = Item->GetDefaultObject<URItem>();
   if (!ItemBP) return false;

   FRItemData data = ItemBP->ItemData;
   data.ItemArch = Item;

   return AddItem (data);
}


bool URInventoryComponent::AddItem_Pickup (ARItemPickup *Pickup)
{
   if (!bIsServer) return false;
   //rlog ("Add item pickup");
   if (!Pickup || !Pickup->Inventory) return false;

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
   //rlog ("rm item");
   if (!Items.IsValidIndex (ItemIdx)) return false;

   if (Items[ItemIdx].Description.Count > Count) {
      Items[ItemIdx].Description.Count -= Count;
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

   //rlog ("Transfer item");
   if (!FromInventory->Items.IsValidIndex (FromItemIdx)) return false;

   FRItemData ItemData = FromInventory->Items[FromItemIdx];
   ItemData.Description.Count = FromItemCount;

   FromInventory->RemoveItem (FromItemIdx, FromItemCount);
     ToInventory->AddItem    (ItemData);

   return true;
}


// -------------------------------------------------------------------------
//                 Item action
// -------------------------------------------------------------------------


bool URInventoryComponent::UseItem (int32 ItemIdx)
{
   if (!bIsServer) return false;

   //rlog ("use item");
   // valid idx
   if (!Items.IsValidIndex (ItemIdx)) return false;
   // valid archetype
   if (!Items[ItemIdx].ItemArch) return false;
   URItem *ItemBP = Items[ItemIdx].ItemArch->GetDefaultObject<URItem>();

   BP_Used (ItemBP);
   ItemBP->Used (GetOwner(), this);

   return true;
}

ARItemPickup* URInventoryComponent::DropItem (int32 ItemIdx, int32 Count)
{
   if (!bIsServer) return nullptr;

   //rlog ("drop item");
   // valid idx
   if (!Items.IsValidIndex (ItemIdx)) return false;
   // valid archetype
   if (!Items[ItemIdx].ItemArch) return false;
   URItem *ItemBP = Items[ItemIdx].ItemArch->GetDefaultObject<URItem>();
   if (!ItemBP) return false;

   FRItemData ItemData = Items[ItemIdx];
   if (Count > ItemData.Description.Count) Count = ItemData.Description.Count;
   if (!RemoveItem (ItemIdx, Count)) return false;


   AActor *Player = GetOwner ();
   // Get Player Rotation
   FRotator rot = Player->GetActorRotation();
   FVector forwardVector = rot.Vector() * 300;
           forwardVector.Z = 0;
   FVector spawnLoc = Player->GetActorLocation() + forwardVector + FVector(0, 0, 50);

   // Create pickup
   ARItemPickup *Pickup = nullptr;

   if (ItemBP->PickupArch) {
      Pickup = GetWorld()->SpawnActor<ARItemPickup>(ItemBP->PickupArch, spawnLoc, rot);

   } else {
      Pickup = GetWorld()->SpawnActor<ARItemPickup>(ARItemPickup::StaticClass (), spawnLoc, rot);
      Pickup->InitEmpty ();
   }
   Pickup->bAutoPickup  = false;
   Pickup->bAutoDestroy = true;

   Pickup->Inventory->DefaultItems.Empty ();
   Pickup->Inventory->Items.Empty ();

   ItemData.Description.Count = Count;
   Pickup->Inventory->Items.Add (ItemData);

   BP_Droped (ItemBP, Pickup);
   ItemBP->Droped (GetOwner(), this, Pickup);

   return Pickup;
}


// ----------------------------------------------------------------------------
//                 Server
// ----------------------------------------------------------------------------

void URInventoryComponent::UseItem_Server_Implementation (int32 ItemIdx)
{
   UseItem (ItemIdx);
}

void URInventoryComponent::DropItem_Server_Implementation (int32 ItemIdx, int32 Count)
{
   DropItem (ItemIdx, Count);
}

void URInventoryComponent::RemoveItem_Server_Implementation (int32 ItemIdx, int32 Count)
{
   //rlog ("action");
   RemoveItem (ItemIdx, Count);
}

void URInventoryComponent::AddItem_Server_Implementation (FRItemData ItemData)
{
   //rlog ("action");
   AddItem (ItemData);
}

void URInventoryComponent::AddItem_Pickup_Server_Implementation (ARItemPickup *Pickup)
{
   //rlog ("action");
   AddItem_Pickup (Pickup);
}

void URInventoryComponent::TransferItem_Server_Implementation (URInventoryComponent *FromInventory,
                                                               URInventoryComponent *ToInventory,
                                                               int32 FromItemIdx,
                                                               int32 FromItemCount)
{
   if (!bIsServer) return;
   URInventoryComponent::TransferItem (FromInventory, ToInventory, FromItemIdx, FromItemCount);
   /*
   if (!FromInventory) return;
   if (!ToInventory)   return;
   if (!bIsServer)     return;

   //rlog ("Transfer item");
   if (!FromInventory->Items.IsValidIndex (FromItemIdx)) return;

   FItemData ItemData = FromInventory->Items[FromItemIdx];
   ItemData.Description.Count = FromItemCount;

   FromInventory->RemoveItem (FromItemIdx, FromItemCount);
     ToInventory->AddItem    (ItemData);
   */
}



// ----------------------------------------------------------------------------
//                 Pickup
// ----------------------------------------------------------------------------

void URInventoryComponent::OnRep_CurrentPickups ()
{
   OnPickupsUpdated.Broadcast ();
}

ARItemPickup* URInventoryComponent::GetClosestPickup ()
{
   ARItemPickup *ClosestPickup = nullptr;

   FVector PlayerLoc = GetOwner ()->GetActorLocation ();
   FVector PickupLoc;

   for (ARItemPickup *itPickup : CurrentPickups) {
      if (!itPickup) continue;

      FVector itLoc = itPickup->GetActorLocation ();

      if (!ClosestPickup) {
         ClosestPickup = itPickup;
         PickupLoc = itLoc;
      }

      if (FVector::Distance (PlayerLoc, itLoc) < FVector::Distance (PlayerLoc, PickupLoc)) {
         ClosestPickup = itPickup;
         PickupLoc = itLoc;
      }
   }

   return ClosestPickup;
}


// -------------------------------------------------------------------------
//                 Save / Load
// -------------------------------------------------------------------------

void URInventoryComponent::OnSave ()
{
   // --- Save player Inventory

   // Convert ItemData to array of JSON strings
   TArray<FString> ItemData;
   for (FRItemData &item : Items) {
      ItemData.Add (item.ToJSON ());
   }

   // Push to buffer
   FBufferArchive ToBinary;
   ToBinary << ItemData;

   FString InventoryUniqueId = GetOwner ()->GetName ();

   // Set
   bool res = URSaveMgr::Set (GetWorld (), InventoryUniqueId, ToBinary);

   if (!res) rlog ("Failed to save player inventory");
}

void URInventoryComponent::OnLoad ()
{
   // --- Load player Inventory

   FString InventoryUniqueId = GetOwner ()->GetName ();

   TArray<uint8> BinaryArray;
   bool res = URSaveMgr::Get (GetWorld (), InventoryUniqueId, BinaryArray);
   if (!res) {
      rlog ("Failed to load player inventory");
      return;
   }

   // Convert Binary to array of JSON strings
   TArray<FString> ItemsData;
   FMemoryReader FromBinary = FMemoryReader (BinaryArray, true);
   FromBinary.Seek(0);
   FromBinary << ItemsData;

   // Convert JSON strings to ItemData
   TArray<FRItemData> loadedItems;
   for (FString &ItemData : ItemsData) {
      FRItemData Item;
      Item.FromJSON (ItemData);
      loadedItems.Add (Item);
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

