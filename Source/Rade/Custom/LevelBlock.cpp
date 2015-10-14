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
	Super::BeginPlay();
}

// Start the block restore event (Called by weapon)
void ALevelBlock::StartTimedRestore(AWeapon* newParentWeapon, float Time)
{
	if (newParentWeapon)
	{
		ParentWeapon = newParentWeapon;
		if (Time>0)
		{
			FTimerHandle MyHandle;
			GetWorldTimerManager().SetTimer(MyHandle, this, &ALevelBlock::ReturnBlock, Time, false);
		}
	}
}

// Restore The block back into weapon
void ALevelBlock::ReturnBlock()
{
	if (ParentWeapon && TheBlockConstructor)
	{
		// Add the ammo back
		if (bRestoreWeaponAmmo)ParentWeapon->MainFire.AddAmmo(ParentWeapon->MainFire.FireCost, 0);

		BP_BlockReturned();

		// Tell constructor to destroy block and clean data
		TheBlockConstructor->DestroyBlock(GetActorLocation(), this);
	}
}