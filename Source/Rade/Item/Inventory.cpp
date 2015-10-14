// Copyright 2015 Vagen Ayrapetyan

#include "Rade.h"

#include "RadeGameMode.h"
#include "BaseHUD.h"

#include "Character/RadeCharacter.h"
#include "Character/RadeAnimInstance.h"

#include "Item/Inventory.h"
#include "Item/ItemPickup.h"
#include "Weapon/Weapon.h"

#include "Custom/SystemSaveGame.h"
#include "UnrealNetwork.h"


AInventory::AInventory(const class FObjectInitializer& PCIP)
{
	RootComponent = PCIP.CreateDefaultSubobject<USceneComponent>(this, TEXT("RootComponent"));
	bReplicates = true;
}


void AInventory::BeginPlay()
{
	Super::BeginPlay();
}

// Use Item With Index
void AInventory::ActionIndex(int32 ItemIndex)
{
	if (Items.IsValidIndex(ItemIndex) && Items[ItemIndex].Archetype && Items[ItemIndex].Archetype->GetDefaultObject<AItem>())
	{
		Action(Items[ItemIndex].Archetype->GetDefaultObject<AItem>());
	}
}

// Use Item With Ref
void AInventory::Action(AItem* ItemRef)
{
	if (!ItemRef || !ThePlayer)
		return;
		
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


// Thow out item with index
void AInventory::ThrowOutIndex(int32 ItemIndex)
{
	// Inside the list of items
	if (Items.IsValidIndex(ItemIndex) && ThePlayer && ThePlayer->IsAnimState(EAnimState::Idle_Run))
	{
		// Check if current weapon, can't throw out weapon that is equiped
		if (Items[ItemIndex].Archetype && ThePlayer->TheWeapon 
			&& ThePlayer->TheWeapon->GetClass() == Items[ItemIndex].Archetype.GetDefaultObject()->GetClass())
		{
			return;
		}
		

		UWorld* const World = ThePlayer->GetWorld();
		if (World && Items[ItemIndex].Archetype && Items[ItemIndex].Archetype->GetDefaultObject<AItem>())
		{
			// Get Player Rotation
			FRotator rot = ThePlayer->GetActorRotation();
			FVector spawnLoc = rot.Vector() * 200 + ThePlayer->FirstPersonCameraComponent->GetComponentLocation()+ FVector(0, 0, -50);


			AItemPickup* newPickup;
			AItem* newItem=Items[ItemIndex].Archetype->GetDefaultObject<AItem>();

			// Spawn Item Pickup archtype
			if (newItem && newItem->ItemPickupArchetype)
				newPickup = World->SpawnActor<AItemPickup>(newItem->ItemPickupArchetype, spawnLoc, rot);

			// Spawn Default pickup
			else newPickup = World->SpawnActor<AItemPickup>(AItemPickup::StaticClass(), spawnLoc, rot);

			if (newPickup)
			{
				newPickup->SetReplicates(true);
				if (newItem)
				{
					newItem->BP_ItemDroped(newPickup);

					newPickup->Item = newItem->GetClass();
					if (newItem->PickupMesh)
					{
						newPickup->Mesh->SetStaticMesh(newItem->PickupMesh);
					}
					else if (newItem->PickupSkelMesh)
					{
						newPickup->SkeletalMesh->SetSkeletalMesh(newItem->PickupSkelMesh);
					}

				}
				if (newPickup->Mesh)
				{
					newPickup->Mesh->SetSimulatePhysics(true);
					newPickup->Mesh->WakeRigidBody();
				
				}
				else if (newPickup->SkeletalMesh)
				{
					newPickup->SkeletalMesh->SetSimulatePhysics(true);
					newPickup->SkeletalMesh->WakeRigidBody();
				}
				newPickup->bAutoPickup = true;
				if (Cast<AWeapon>(newItem))
				{
					newPickup->bOverideItemData = true;
					newPickup->OverideItemData = Items[ItemIndex];

				}


				if (Items[ItemIndex].ItemCount > 1)
				{
					Items[ItemIndex].ItemCount--;
				}
				else RemoveItemIndex(ItemIndex);

				
				newPickup->ActivatePickupPhysics();
				if (newPickup->Mesh)
				{
					newPickup->Mesh->AddImpulse(rot.Vector(), NAME_None, true);
				}
				if (newPickup->SkeletalMesh)
				{
					newPickup->SkeletalMesh->AddForce(rot.Vector() * 16000, NAME_None, true);
				}
		
				UpdateInfo();
			}		
		}
	}
}


// Throw ou item by pointer
void AInventory::ThrowOut(AItem* ItemRef)
{
	// Find the item from items list
	for (int32 i = 0; i < Items.Num();i++)
	{
		if (Items.IsValidIndex(i) && Items[i].Archetype.GetDefaultObject()->GetClass() == ItemRef->GetClass())
		{
			ThrowOutIndex(i);
			return;
		}
	}
}

// Item Picked up from the pickup
void AInventory::ItemPickedUp(AItemPickup* ThePickup)
{
	if (!ThePickup || !ThePickup->Item)
		return;
	
	//  Override Item data?
	if (ThePickup->bOverideItemData)
	{
		// Create New Data
		FItemData* NewData = AddItem(ThePickup->Item);

		// Set The Data
		if (NewData)
			NewData->SetItemData(ThePickup->OverideItemData);
	}
	// Add Default Item Data
	else 
		AddItem(ThePickup->Item);
}


// Add subclass of item to inventory 
FItemData* AInventory::AddItem(TSubclassOf<AItem> newItem)
{
	if (newItem == NULL)
		return NULL;


	// Check if same item in inventory 
	if (Items.Num()>0)
	{

		// If new Item is weapon and same type as current weapon
		if (newItem->GetDefaultObject<AWeapon>() && ThePlayer && ThePlayer->TheWeapon && ThePlayer->TheWeapon->GetClass() == newItem->GetDefaultObject<AWeapon>()->GetClass())
		{
			// Add Ammo to the current weapon equiped
			ThePlayer->TheWeapon->AddAmmo(newItem->GetDefaultObject<AWeapon>());

			// Find the weapon data and return it.
			for (int32 i = 0; i < Items.Num(); i++)
			{
				if (Items.IsValidIndex(i) && Items[i].Archetype.GetDefaultObject()->GetClass() == ThePlayer->TheWeapon->GetClass())
					return &Items[i];
			}
			return NULL;
		}



		// Check current item list if same item exists
		for (int32 i = 0; i < Items.Num(); i++)
		{

			// If same object archetype
			if (Items[i].Archetype->GetDefaultObject()->GetClass() == newItem->GetDefaultObject()->GetClass())
			{

				if (newItem->GetDefaultObject<AItem>())newItem->GetDefaultObject<AItem>()->BP_ItemUpdated();
	
				// IF Weapon add ammo, else add count
				if (Cast<AWeapon>(newItem->GetDefaultObject<AItem>()))
				{
					Items[i].MainFireStats.AddAmmo(newItem->GetDefaultObject<AWeapon>()->MainFire.CurrentAmmo, newItem->GetDefaultObject<AWeapon>()->MainFire.ClipNumber);
				}
				else Items[i].ItemCount++;

				// Save and Update Item Data
				SaveInventory(); 
				UpdateInfo();
				return &Items[i];
			}
		}
	}

	// New Item 
	AItem* newItemBase = newItem->GetDefaultObject<AItem>();
	FItemData newData = FItemData(newItem, newItemBase->ItemName, newItemBase->ItemIcon, newItemBase->Weight, newItemBase->ItemCount);

	// Add New item to item list and update inventory
	Items.Add(newData);
	UpdateInfo();

	return &Items[Items.Num()-1];
}

// Same as AddItem but with value struct return
FItemData AInventory::AddNewItem(TSubclassOf<AItem> newItem)
{
	return *(AddItem(newItem));
}
// Remp with ID
void AInventory::RemoveItemIndex(int32 ItemID)
{
	if (!Items.IsValidIndex(ItemID))return;
	Items.RemoveAt(ItemID);
	UpdateInfo();
}

// Remove Item with ref
void AInventory::RemoveItem(AItem* DeleteItem)
{
	if (!DeleteItem)return;

	// Find Item in item list
	for (int32 i = 0; i < Items.Num(); i++)
	{
		if (!Items.IsValidIndex(i) && Items[i].Archetype->GetDefaultObject()->GetClass() == DeleteItem->GetClass())
			RemoveItemIndex(i);
	}
}


// Inventory Updated
void AInventory::UpdateInfo()
{

	// Event in HUD that Inventory Updated
	if (ThePlayer && ThePlayer->TheHUD)  ThePlayer->TheHUD->BP_InventoryUpdated();

	if (Items.Num()<1)return;

	TotalWeight = 0;

	// Calculate weight of each item * itemcound
	for (int i = 0; i < Items.Num(); i++)
	{
		if (Items[i].Weight>0 && Items[i].ItemCount>0)
			TotalWeight += (Items[i].Weight*Items[i].ItemCount);
	}
	SaveInventory();
}

// Update Item List, Called on client
void AInventory::ClientReceiveUpdatedItemList()
{
	if (ThePlayer && ThePlayer->TheHUD)  
		ThePlayer->TheHUD->BP_InventoryUpdated();
}

// Load Inventory
void AInventory::LoadInventory()
{
	// Find Game Mode and Save FIle
	if (GetWorld() && GetWorld()->GetAuthGameMode())
	{
		TheGM=GetWorld()->GetAuthGameMode<ARadeGameMode>();
		if (TheGM && TheGM->SaveFile)
		{
			Items = TheGM->SaveFile->Items;
		}
	}
	// Error With Game Mode or Save file ref, retry after 0.5 sec
	if (!TheGM || !TheGM->SaveFile)
	{
		FTimerHandle MyHandle;
		GetWorldTimerManager().SetTimer(MyHandle, this, &AInventory::LoadInventory, 0.5, false);
	}

	// Set Inventory Items data to items data from save file
	if (TheGM && TheGM->SaveFile)Items = TheGM->SaveFile->Items;
}

// Save Inventory
void AInventory::SaveInventory()
{
	// Save current weapon stats
	if (ThePlayer && ThePlayer->TheWeapon)ThePlayer->TheWeapon->SaveCurrentWeaponStats();

	// Set Savefile items
	if (TheGM && TheGM->SaveFile)TheGM->SaveFile->Items = Items;
}

void AInventory::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AInventory, Items);
	DOREPLIFETIME(AInventory, TotalWeight);
	DOREPLIFETIME(AInventory, ThePlayer);
}