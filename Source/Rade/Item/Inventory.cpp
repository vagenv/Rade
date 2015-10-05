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

void AInventory::ActionIndex(int32 ItemIndex)
{
	if (Items.IsValidIndex(ItemIndex) && Items[ItemIndex].Archetype && Items[ItemIndex].Archetype->GetDefaultObject<AItem>())
	{
		//printg("Index Use");
		Action(Items[ItemIndex].Archetype->GetDefaultObject<AItem>());
	}
}





void AInventory::Action(AItem* ItemRef)
{
	//print("Start Action");
	if (!ItemRef || !ThePlayer)
		return;
		
	//printg(ItemRef->GetName());
	
	ItemRef->InventoryUse(ThePlayer);
	ThePlayer->BP_ItemUsed(ItemRef);

	// Is a weapon
	if (Cast<AWeapon>(ItemRef))
	{
		AWeapon* TheWeapon = Cast<AWeapon>(ItemRef);
		

		// Is in state To Equip/deequip weapon
		if (ThePlayer->ArmsAnimInstance && !ThePlayer->ArmsAnimInstance->IsAnimState(EAnimState::Idle_Run))return;


		// Selected to Unequip Current Weapon
		if (ThePlayer->TheWeapon && ThePlayer->TheWeapon->GetClass() == ItemRef->GetClass())
		{
			//printr("Unequip Current Weapon");
			ThePlayer->UnEquipCurrentWeapon();

		}

		// Selected to equip New Weapon
		else 
		{
			//printg("Equip Weapon");
			ThePlayer->EquipWeapon(TheWeapon);
		}
		
	
	}
}



void AInventory::ThrowOutIndex(int32 ItemIndex)
{

//	printr("Drop item");
	if (Items.IsValidIndex(ItemIndex) && ThePlayer)
	{

		/// Modify

		if (Items[ItemIndex].Archetype && ThePlayer->TheWeapon 
			&& ThePlayer->TheWeapon->GetClass() == Items[ItemIndex].Archetype.GetDefaultObject()->GetClass())
		{

			//print("Weapon equiped");
			return;
		}
		
		//&& Items[ItemIndex].Archetype == player->Weapon->SpawedItemArchtype
	

		UWorld* const World = ThePlayer->GetWorld();
		if (World && Items[ItemIndex].Archetype && Items[ItemIndex].Archetype->GetDefaultObject<AItem>())
		{

			FRotator rot = ThePlayer->GetActorRotation();
		
			FVector spawnLoc = rot.Vector() * 200 + ThePlayer->FirstPersonCameraComponent->GetComponentLocation()+ FVector(0, 0, -50);
			AItemPickup* newPickup;
			AItem* newItem=Items[ItemIndex].Archetype->GetDefaultObject<AItem>();
			// ThePlayer->GetActorLocation() 

			if (newItem && newItem->ItemPickupArchetype)
				newPickup = World->SpawnActor<AItemPickup>(newItem->ItemPickupArchetype, spawnLoc, rot);
			else newPickup = World->SpawnActor<AItemPickup>(AItemPickup::StaticClass(), spawnLoc, rot);

			if (newPickup)
			{
				newPickup->SetReplicates(true);
				// Item Will be throwed	
				if (newItem)
				{

					newItem->ItemDroped(newPickup);

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

				if (Items[ItemIndex].ItemCount > 1)
				{
					Items[ItemIndex].ItemCount--;
				}
				else RemoveItemIndex(ItemIndex);

				
				newPickup->ActivatePickup();
				//newPickup->GetRootComponent()->
				if (newPickup->Mesh)
				{
					newPickup->Mesh->AddImpulse(rot.Vector(), NAME_None, true);
				}
				if (newPickup->SkeletalMesh)
				{
					//newPickup->SkeletalMesh->AddImpulse(rot.Vector()*1000000, NAME_None, true);
					newPickup->SkeletalMesh->AddForce(rot.Vector() * 16000, NAME_None, true);
					//newPickup->SkeletalMesh->ComponentVelocity = rot.Vector() * 1000000;
				}
				UpdateInfo();
			}		
		}
		//else printr("Error with refs");
	}
}


void AInventory::ThrowOut(AItem* ItemRef)
{

}



// Basic Inventory Operations
void AInventory::AddItem(TSubclassOf<AItem> newItem)
{
	if (newItem == NULL || ThePlayer == NULL)return;


	//printr("Add item");

	// If same weapon that is currently equiped
	if (ThePlayer->TheWeapon && ThePlayer->TheWeapon->GetClass()== newItem->GetClass())
	{
		ThePlayer->TheWeapon->AddAmmo(newItem->GetDefaultObject<AWeapon>());
		return;
	}
	else
	{
		for (int32 i = 0; i < Items.Num(); i++)
		{

			// If same object archetype
			if (Items[i].Archetype.GetDefaultObject()->GetClass() == newItem.GetDefaultObject()->GetClass())
			{

				if (newItem->GetDefaultObject<AItem>())newItem->GetDefaultObject<AItem>()->ItemUpdated();
	
				// IF Weapon add ammo, else add cound
				if (Cast<AWeapon>(newItem->GetDefaultObject<AItem>()))
				{
					//print("ADD AMMO");
					Items[i].WeaponStats.AddAmmo(newItem->GetDefaultObject<AWeapon>()->MainFire.CurrentAmmo, newItem->GetDefaultObject<AWeapon>()->MainFire.ClipNumber);
				}
				else Items[i].ItemCount++;


				//UpdateInfo();
				SaveInventory();
				return;
			}
		}
	}


	// New Item
	AItem* newItemBase = newItem->GetDefaultObject<AItem>();
	//newItemBase->SpawedItemArchtype = newItem;
	Items.Add(FItemData(newItem, newItemBase->ItemName, newItemBase->ItemIcon, newItemBase->Weight, newItemBase->ItemCount));
	//printg("Add New Item");
	UpdateInfo();
}
void AInventory::RemoveItem(AItem* DeleteItem)
{
	if (DeleteItem == NULL)return;
	//Items.Remove(DeleteItem);
	UpdateInfo();
}
void AInventory::RemoveItemIndex(int32 ItemId)
{
	if (ItemId > Items.Num())return;
	Items.RemoveAt(ItemId);
	UpdateInfo();
}



// Update Inventory Weight
void AInventory::UpdateInfo()
{
	SaveInventory();

	if (ThePlayer && ThePlayer->TheHUD)  ThePlayer->TheHUD->BP_InventoryUpdated();

	if (Items.Num()<1)return;
	TotalWeight = 0;
	for (int i = 0; i < Items.Num(); i++)
	{
		if (Items[i].Weight>0 && Items[i].ItemCount>0)
			TotalWeight += (Items[i].Weight*Items[i].ItemCount);
	}

}

void AInventory::ClientReceiveUpdatedItemList()
{
	if (ThePlayer && ThePlayer->TheHUD)  ThePlayer->TheHUD->BP_InventoryUpdated();
}


void AInventory::LoadInventory()
{

	if (GetWorld() && GetWorld()->GetAuthGameMode())
	{
		TheGM=GetWorld()->GetAuthGameMode<ARadeGameMode>();
		if (TheGM && TheGM->SaveFile)
		{
			Items = TheGM->SaveFile->Items;
		}
	}

	if (!TheGM || !TheGM->SaveFile)
	{
		FTimerHandle MyHandle;
		GetWorldTimerManager().SetTimer(MyHandle, this, &AInventory::LoadInventory, 0.5, false);
	}
	if (TheGM && TheGM->SaveFile)Items = TheGM->SaveFile->Items;
}


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