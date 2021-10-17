// Copyright 2015-2021 Vagen Ayrapetyan

#include "Inventory.h"
#include "ItemPickup.h"
#include "../Weapon/Weapon.h"

#include "../RadeGameMode.h"
#include "../Rade.h"
#include "../BaseHUD.h"

#include "../Character/RadePlayer.h"
#include "../Character/RadeAnimInstance.h"

#include "../System/SystemSaveGame.h"
#include "Net/UnrealNetwork.h"

UInventory::UInventory(const class FObjectInitializer& PCIP)
{
	SetIsReplicatedByDefault (true);
}

void UInventory::BeginPlay()
{
	Super::BeginPlay();
}

// Use Item With Index
void UInventory::ActionIndex(int32 ItemIndex)
{
	if (Items.IsValidIndex(ItemIndex) && 
       Items[ItemIndex].Archetype && 
       Items[ItemIndex].Archetype->GetDefaultObject<AItem>())
	{
		Action(Items[ItemIndex].Archetype->GetDefaultObject<AItem>());
	}
}

// Use Item With Ref
void UInventory::Action(AItem* ItemRef)
{
	if (!ItemRef || !TheCharacter) return;

	ARadePlayer* ThePlayer = Cast<ARadePlayer>(TheCharacter);

	// Only Player Can Use Items
	if (ThePlayer) {
		// Call Events in the BP
		ItemRef->InventoryUse(ThePlayer);
		ThePlayer->BP_ItemUsed(ItemRef);

		// If a weapon and player in right state -> equip it
		if (Cast<AWeapon>(ItemRef) && ThePlayer->IsAnimState(EAnimState::Idle_Run))
		{
			AWeapon* TheWeapon = Cast<AWeapon>(ItemRef);

			// Current Weapon Selected, Unequip it
			if (ThePlayer->TheWeapon && ThePlayer->TheWeapon->GetClass() == ItemRef->GetClass())
				ThePlayer->UnEquipCurrentWeapon();

			// New Weapon Selected, Equip It 
			else ThePlayer->EquipWeapon(TheWeapon);
		}
	}
}

// Throw out item with index
void UInventory::ThrowOutIndex(int32 ItemIndex)
{
	// Inside the list of items
	if (  Items.IsValidIndex(ItemIndex)
		&& TheCharacter
		&& TheCharacter->IsAnimState(EAnimState::Idle_Run)) {

		// Check if current weapon, can't throw out weapon that is equiped
		if (  Items[ItemIndex].Archetype && TheCharacter->TheWeapon
			&& TheCharacter->TheWeapon->GetClass() == Items[ItemIndex].Archetype.GetDefaultObject()->GetClass())
		{
			return;
		}

		UWorld* const World = TheCharacter->GetWorld();
		if (  World
			&& Items[ItemIndex].Archetype
			&& Items[ItemIndex].Archetype->GetDefaultObject<AItem>()) {

			// Get Player Rotation
			FRotator rot = TheCharacter->GetActorRotation();
         FVector forwardVector = rot.Vector() * 300;
					  forwardVector.Z = 0;
         FVector spawnLoc = TheCharacter->GetActorLocation() + forwardVector + FVector(0, 0, 50);

			AItemPickup* newPickup;
			AItem* newItem = Items[ItemIndex].Archetype->GetDefaultObject<AItem>();

			// Spawn Item Pickup archetype
			if (newItem && newItem->ItemPickupArchetype)
				newPickup = World->SpawnActor<AItemPickup>(newItem->ItemPickupArchetype, spawnLoc, rot);

			// Spawn Default pickup
			else newPickup = World->SpawnActor<AItemPickup>(AItemPickup::StaticClass(), spawnLoc, rot);

			if (newPickup) {
				newPickup->SetReplicates(true);
            newPickup->SetActorHiddenInGame(true);
				if (newItem) {
					newItem->BP_ItemDroped(newPickup);
					newPickup->Item = newItem->GetClass();
					if      (newItem->PickupMesh)     newPickup->Mesh->SetStaticMesh(newItem->PickupMesh);                    
					else if (newItem->PickupSkelMesh) newPickup->SkeletalMesh->SetSkeletalMesh(newItem->PickupSkelMesh);
				}

				newPickup->bAutoPickup = true;
				if (Cast<AWeapon>(newItem)) {
					newPickup->bOverideItemData = true;
					newPickup->OverideItemData = Items[ItemIndex];
				}

				if (Items[ItemIndex].ItemCount > 1) Items[ItemIndex].ItemCount--;
				else											RemoveItemIndex(ItemIndex);

				newPickup->ActivatePickupPhysics();

				if (  newPickup->Mesh
					&& newPickup->Mesh->IsSimulatingPhysics())
               newPickup->Mesh->AddImpulse(rot.Vector() * 120, NAME_None, true);

				if (newPickup->SkeletalMesh
					&& newPickup->SkeletalMesh->IsSimulatingPhysics())
               newPickup->SkeletalMesh->AddForce(rot.Vector() * 12000, NAME_None, true);

            newPickup->SetActorHiddenInGame(false);
				UpdateInfo();
			}		
		}
	}
}


// Throw out item by pointer
void UInventory::ThrowOut(AItem* ItemRef)
{
	// Find the item from items list
	for (int32 i = 0; i < Items.Num();i++) {
		if (  Items.IsValidIndex(i)
			&& Items[i].Archetype.GetDefaultObject()->GetClass() == ItemRef->GetClass()) {
			ThrowOutIndex(i);
			return;
		}
	}
}

// Throw out all Items
void UInventory::ThrowOutAllItems()
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


// Item Picked up from the pickup
void UInventory::ItemPickedUp(AItemPickup* ThePickup)
{
	if (!ThePickup || !ThePickup->Item) return;
	
	//  Override Item data?
	if (ThePickup->bOverideItemData) {
		// Create New Data
		FItemData* NewData = AddItem(ThePickup->Item);

		// Set The Data
		if (NewData) NewData->SetItemData(ThePickup->OverideItemData);

	// Add Default Item Data
	} else AddItem(ThePickup->Item);
}


// Add subclass of item to inventory 
FItemData* UInventory::AddItem(TSubclassOf<AItem> newItem)
{
	if (newItem == nullptr) return nullptr;
    
	// New Item 
	AItem* newItemBase = newItem->GetDefaultObject<AItem>();
   if (newItemBase == nullptr) return nullptr;

	// Check if same item in inventory 
	if (Items.Num() > 0) {
      AWeapon *newWeapon = Cast<AWeapon>(newItemBase);

		// If new Item is weapon and same type as current weapon
		if (  newWeapon
			&& TheCharacter
			&& TheCharacter->TheWeapon
			&& TheCharacter->TheWeapon->GetClass() == newWeapon->GetClass())
		{
			// Add Ammo to the current weapon equiped
			TheCharacter->TheWeapon->AddAmmo(newWeapon);

			// Find the weapon data and return it.
			for (int32 i = 0; i < Items.Num(); i++) {
				if (  Items.IsValidIndex(i) && Items[i].Archetype.GetDefaultObject()
					&& Items[i].Archetype.GetDefaultObject()->GetClass() == TheCharacter->TheWeapon->GetClass())
					return &Items[i];
			}
			return nullptr;
		}

		// Check current item list if same item exists
		for (int32 i = 0; i < Items.Num(); i++) {
         
			// If same object archetype
			if (  Items[i].Archetype
				&& Items[i].Archetype->GetDefaultObject<AItem>()) 
         {
            AItem *ItrItem = Items[i].Archetype->GetDefaultObject<AItem>();
           
            if (ItrItem->GetClass() == newItemBase->GetClass()) {
				   if (newItemBase) newItemBase->BP_ItemUpdated();
	
				   // IF Weapon add ammo, else add count
				   if (newWeapon)
					   Items[i].MainFireStats.AddAmmo(newWeapon->MainFire.CurrentAmmo, 
                                                 newWeapon->MainFire.ClipNumber);
				   else Items[i].ItemCount++;

				   // Save and Update Item Data
				   SaveInventory(); 
				   UpdateInfo();
				   return &Items[i];
            }
			}
		}
	}
   
	FItemData newData = FItemData(newItem, newItemBase->ItemName, 
                                 newItemBase->ItemIcon, 
                                 newItemBase->Weight, 
                                 newItemBase->ItemCount);                                 

	// Add New item to item list and update inventory
	Items.Add(newData);
	UpdateInfo();
	return &Items[Items.Num()-1];
}

// BP Function. Returns AddItem Result
FItemData UInventory::AddNewItem(TSubclassOf<AItem> newItem)
{
	return *(AddItem(newItem));
}

// Remove item with ID
void UInventory::RemoveItemIndex(int32 ItemID)
{
	if (!Items.IsValidIndex(ItemID)) return;
	Items.RemoveAt(ItemID);
	UpdateInfo();
}

// Remove Item with ref
void UInventory::RemoveItem(AItem* DeleteItem)
{
	if (!DeleteItem) return;

	// Find Item in item list
	for (int32 i = 0; i < Items.Num(); i++) {
		if (Items.IsValidIndex(i) && Items[i].Archetype->GetDefaultObject()->GetClass() == DeleteItem->GetClass())
			RemoveItemIndex(i);
	}
}

// Remove with ID
void UInventory::RemoveItemIndex_Count(int32 ItemID, int32 ItemRemoveCount)
{
	if (!Items.IsValidIndex(ItemID)) return;
	if (Items[ItemID].ItemCount > ItemRemoveCount) Items[ItemID].ItemCount -= ItemRemoveCount;
	else														Items.RemoveAt(ItemID);
	UpdateInfo();
}

// Remove Item with ref
void UInventory::RemoveItem_Count(AItem* DeleteItem, int32 ItemRemoveCount)
{
	if (!DeleteItem) return;

	// Find Item in item list
	for (int32 i = 0; i < Items.Num(); i++) {
		if (Items.IsValidIndex(i) && Items[i].Archetype->GetDefaultObject()->GetClass() == DeleteItem->GetClass())
			RemoveItemIndex_Count(i, ItemRemoveCount);
	}
}

// Inventory Updated
void UInventory::UpdateInfo()
{
	// Event in HUD that Inventory Updated
	if (	TheCharacter
		&& Cast<ARadePlayer>(TheCharacter)
		&& Cast<ARadePlayer>(TheCharacter)->TheHUD) 
		Cast<ARadePlayer>(TheCharacter)->TheHUD->BP_InventoryUpdated();

	if (Items.Num() < 1) return;

	TotalWeight = 0;

	// Calculate weight of each item * itemcound
	for (int i = 0; i < Items.Num(); i++) {
		if (  Items[i].Weight > 0
			&& Items[i].ItemCount > 0)
			TotalWeight += (Items[i].Weight*Items[i].ItemCount);
	}
	SaveInventory();
}

// Update Item List, Called on client
void UInventory::OnRep_ItemListUpdated()
{
	if (  TheCharacter
		&& Cast<ARadePlayer>(TheCharacter)
		&& Cast<ARadePlayer>(TheCharacter)->TheHUD)
		Cast<ARadePlayer>(TheCharacter)->TheHUD->BP_InventoryUpdated();
}

// Load Inventory
void UInventory::LoadInventory()
{
	// Find Game Mode and Save File
	if (GetWorld() && GetWorld()->GetAuthGameMode())
	{
		TheGM = GetWorld()->GetAuthGameMode<ARadeGameMode>();
		if (TheGM && TheGM->SaveFile) Items = TheGM->SaveFile->Items;
	}
	// Error With Game Mode or Save file ref, retry after 0.5 sec
	if (!TheGM || !TheGM->SaveFile) {
		if (TheCharacter) {
			FTimerHandle MyHandle;
			TheCharacter->GetWorldTimerManager().SetTimer(MyHandle, this, &UInventory::LoadInventory, 0.5, false);
		}
	}

	// Set Inventory Items data to items data from save file
	if (TheGM && TheGM->SaveFile)Items = TheGM->SaveFile->Items;
}

// Save Inventory
void UInventory::SaveInventory()
{
	// Save current weapon stats
	if (TheCharacter && TheCharacter->TheWeapon) TheCharacter->TheWeapon->SaveCurrentWeaponStats();

	// Set Save file items
	if (TheGM && TheGM->SaveFile) TheGM->SaveFile->Items = Items;
}

void UInventory::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UInventory, Items);
	DOREPLIFETIME(UInventory, TotalWeight);
	DOREPLIFETIME(UInventory, TheCharacter);
}
