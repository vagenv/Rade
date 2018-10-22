// Copyright 2015-2017 Vagen Ayrapetyan

#include "Custom/LevelBlockConstructor.h"
#include "Custom/LevelBlock.h"

#include "Weapon/ConstructorWeapon.h"

#include "RadeGameMode.h"
#include "Rade.h"
#include "System/SystemSaveGame.h"

#include "UnrealNetwork.h"


ALevelBlockConstructor::ALevelBlockConstructor(const FObjectInitializer& PCIP)
	: Super(PCIP)
{

	bReplicates = true;
    bAlwaysRelevant = false;
    PrimaryActorTick.bCanEverTick = false;

    Scene = PCIP.CreateDefaultSubobject<USceneComponent>(this, TEXT("Root"));
    Scene->SetMobility(EComponentMobility::Movable);
    SetRootComponent(Scene);
}

void ALevelBlockConstructor::BeginPlay()
{
	Super::BeginPlay();

	// Get pointer to The Game Mode
	if (GetWorld() && GetWorld()->GetAuthGameMode() && GetWorld()->GetAuthGameMode<ARadeGameMode>())
	{
		GetWorld()->GetAuthGameMode<ARadeGameMode>()->TheLevelBlockConstructor = this;
	}

	// Set to root of world
	SetActorLocation(FVector(0, 0, 0));
}


// Blocks Updated, Called on Client
void ALevelBlockConstructor::ClientBlocksUpdated(){
}

// Blocks Updated , Called on server
void ALevelBlockConstructor::Server_UpdateBlocksStatus()
{
	// Check Validity of each block, Restore them if they don't exist
	for (int32 i = 0;i<CurrentBlocks.Num();i++)
	{
		if (GetWorld() && CurrentBlocks.IsValidIndex(i) && !CurrentBlocks[i].LevelItem && CurrentBlocks[i].Archetype 
			&& CurrentBlocks[i].Archetype.GetDefaultObject()&& Cast<ALevelBlock>(CurrentBlocks[i].Archetype.GetDefaultObject()))
		{	
			ALevelBlock* TheBlock=GetWorld()->SpawnActor<ALevelBlock>(CurrentBlocks[i].Archetype, CurrentBlocks[i].GlobalPosition, FRotator(0));
			
			if (TheBlock)
			{
				CurrentBlocks[i].LevelItem = TheBlock;
				TheBlock->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
			}
		}
	}
}

// Spawn new Block to World
bool ALevelBlockConstructor::AddNewBlock(TSubclassOf <ALevelBlock>  NewBlockArchtype, FVector& Loc, AConstructorWeapon* TheConstructorWeapon)
{
	UWorld* const World = GetWorld();
	if (NewBlockArchtype && World)
	{
		// Calculate Rounded 3D location in Array
		FVector tempV;
		tempV.X = round(Loc.X / 100);
		tempV.Y = round(Loc.Y / 100);
		tempV.Z = round(Loc.Z / 100);

		// Check if position in the world is avaiable.
		for (int32 i = 0; i < CurrentBlocks.Num(); i++)
		{
			if (CurrentBlocks[i].ConstructorPosition == tempV)
			{
				return false;
			}
		}

		// Create new Block Data
		FBlockData newBlockData = FBlockData();

		// Set Block Data
		newBlockData.Archetype = NewBlockArchtype;
		newBlockData.ConstructorPosition = tempV;

		tempV.X *= 100;
		tempV.Y *= 100;
		tempV.Z *= 100;

		newBlockData.GlobalPosition = tempV;


		// Spawn new block actor
		newBlockData.LevelItem = World->SpawnActor<ALevelBlock>(NewBlockArchtype, tempV, FRotator(0));

		if (!newBlockData.LevelItem)
		{
			return false;
		}

		// Set block values
		newBlockData.LevelItem->ParentWeapon = TheConstructorWeapon;
		newBlockData.LevelItem->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
		newBlockData.LevelItem->TheBlockConstructor = this;

		// Auto Destroy Block After some time
		if (TheConstructorWeapon && TheConstructorWeapon->bAutoDestroyBlocks)
		{
			// Restore Ammo After Destory
			if (TheConstructorWeapon->bRestoreAmmoAfterBlockDestroy) newBlockData.LevelItem->bRestoreWeaponAmmo = true;

			// Start Block Destroy Event
			newBlockData.LevelItem->StartTimedRestore(TheConstructorWeapon, TheConstructorWeapon->BlockRestoreTime);

		}

		//  Add Block To List
		CurrentBlocks.Add(newBlockData);

		return true;
	}
	return false;
}

// Destroy Block 
bool ALevelBlockConstructor::DestroyBlock(FVector Loc, AActor* TheInstigator)
{
	UWorld* const World = GetWorld();
	if (World)
	{
		// Calculate Rounded 3D location in Array
		FVector tempV;
		tempV.X = round(Loc.X / 100);
		tempV.Y = round(Loc.Y / 100);
		tempV.Z = round(Loc.Z / 100);

		// Find the block location in world
		for (int32 i = 0; i < CurrentBlocks.Num(); i++)
		{
			// Block position match found
			if ( CurrentBlocks[i].ConstructorPosition == tempV)
			{
				// Destroy Block
				if (CurrentBlocks[i].LevelItem)CurrentBlocks[i].LevelItem->Destroy();

				// Clean Data
				CurrentBlocks.RemoveAt(i);
				return true;
			}
		}
	}
	return false;
}

// Replicate Data
void ALevelBlockConstructor::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ALevelBlockConstructor, CurrentBlocks);
}
