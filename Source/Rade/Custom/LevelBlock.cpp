// Copyright 2015 Vagen Ayrapetyan

#include "Rade.h"

#include "Character/RadeCharacter.h"
#include "Item/Inventory.h"

#include "Custom/LevelBlock.h"
#include "Custom/LevelBlockConstructor.h"
#include "Weapon/Weapon.h"




ALevelBlock::ALevelBlock(const class FObjectInitializer& PCIP)
	: Super(PCIP)
{

	MyRoot = PCIP.CreateOptionalDefaultSubobject <USceneComponent>(this, TEXT("MyRoot"));
	RootComponent = MyRoot;

	Mesh = PCIP.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("Mesh"));
	Mesh->AttachParent = MyRoot;

	bReplicates = true;
}

void ALevelBlock::BeginPlay()
{
	//	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Black, TEXT("Block Started"));
}

void ALevelBlock::StartTimedRestore(AWeapon* newParentWeapon, float Time)
{
	if (newParentWeapon)
	{
		ParentWeapon = newParentWeapon;
		ParentWeaponArchtype = ParentWeapon->GetClass();
		if (Time>0)
		{
			FTimerHandle MyHandle;
			GetWorldTimerManager().SetTimer(MyHandle, this, &ALevelBlock::ReturnBlock, Time, false);

			//GetWorldTimerManager().SetTimer(this, &ALevelBlock::ReturnBlock, Time, false);

		}
	}
}
void ALevelBlock::ReturnBlock()
{

	/*
	if (parentConstructor->player->Weapon->MainFireState != EWeaponState::Idle)
	{

		FTimerHandle MyHandle;
		GetWorldTimerManager().SetTimer(MyHandle, this, &ALevelBlock::ReturnBlock, 0, false, 3);
		//GetWorldTimerManager().SetTimer(this, &ALevelBlock::ReturnBlock, 3, false);
		//if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT(" Postponing return"));
		return;
	}

	if (!parentConstructor->player)
	{
		// Reset player in parent		
		parentConstructor->player = Cast<ARadeCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
		//if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Black, TEXT(" reseting player ref"));

	}

	if (parentConstructor->player->Weapon == ParentWeapon || parentConstructor->player->Weapon->GetClass() == ParentWeapon->GetClass())
	{
		// Same class or object
		ParentWeapon->MainFire.AddAmmo(1, 0);
		//if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor(50,50,50), TEXT("Curret Weapon or Same Type"));
	}
	else if (ParentWeapon->bDestroyed)
	{
		//if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Black, TEXT("Searching in inv"));
		if (parentConstructor->player)
		{
			AInventory* inv = parentConstructor->player->theInventory;
			if (inv)
			{

				for (int32 i = 0; i < inv->Items.Num(); i++)
				{
					if (inv->Items[i].Archetype->GetClass() == weapArchetype)
					{
						//if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("weap found in inv"));
						inv->Items[i].WeaponStats.AddAmmo(1, 0);
						break;
					}
				}
			}
			//	else if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Black, TEXT("inv not found"));
		}
		//else if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Black, TEXT("Player not found"));

	}


	parentConstructor->DestroyBlock(GetActorLocation(), this);
	*/
	/*
	if (!ParentWeapon->bDestroyed && ParentWeapon->GetClass() == parentConstructor->player->Weapon->GetClass() && parentConstructor->player->Weapon->MainFireState == EWeaponState::Idle)
	{
	// Weapon still equiped
	ParentWeapon->MainFire.AddAmmo(1, 0);
	//if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Black, ParentWeapon->GetClass()->GetName());
	//if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Black, ParentWeapon->GetActorLocation().ToString());
	}

	else return;

	//
	if (false)
	{





	// Weapon is unequiped
	// need to find in inventory
	if (parentConstructor->player)
	{
	AInventory* inv = parentConstructor->player->Inventory;
	if (inv)
	{

	bool t = false;
	for (int32 i = 0; i < inv->Items.Num();i++)
	{
	if (inv->Items[i].Archetype->GetClass()==weapArchetype)
	{
	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Black, TEXT("weap found in inv"));
	inv->Items[i].WeaponStats.AddAmmo(1,0);
	t = true;
	break;
	}
	}
	if (!t)
	{
	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Black, TEXT("weapon in inv not found"));
	}
	}
	else if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Black, TEXT("inv not found"));
	}
	else if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Black, TEXT("Player not found"));

	}


	//Destroy();
	*/
}