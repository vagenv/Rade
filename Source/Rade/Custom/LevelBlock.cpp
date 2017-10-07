// Copyright 2015-2016 Vagen Ayrapetyan

#include "Custom/LevelBlock.h"
#include "Custom/LevelBlockConstructor.h"
#include "Weapon/ConstructorWeapon.h"

#include "Character/RadePlayer.h"
#include "Rade.h"
#include "Item/Inventory.h"


ALevelBlock::ALevelBlock(const class FObjectInitializer& PCIP)
	: Super(PCIP)
{

	MyRoot = PCIP.CreateOptionalDefaultSubobject <USceneComponent>(this, TEXT("MyRoot"));
   SetRootComponent (MyRoot);

	Mesh = PCIP.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("Mesh"));
   Mesh->AttachToComponent(MyRoot, FAttachmentTransformRules::SnapToTargetNotIncludingScale, NAME_None);

	bReplicates = true;
}

void ALevelBlock::BeginPlay()
{
	Super::BeginPlay();
}

// Start the block restore event (Called by weapon)
void ALevelBlock::StartTimedRestore(AConstructorWeapon* newParentWeapon, float Time)
{
	if (newParentWeapon)
	{
		// Assign Weapon that spawned block
		ParentWeapon = newParentWeapon;

		// If the 
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