// Copyright 2015 Vagen Ayrapetyan

#include "Rade.h"
#include "RadeGameMode.h"

#include "Custom/LevelBlockConstructor.h"
#include "Custom/SystemSaveGame.h"
#include "Custom/LevelBlock.h"
#include "Weapon/ConstructorWeapon.h"

#include "UnrealNetwork.h"


ALevelBlockConstructor::ALevelBlockConstructor(const FObjectInitializer& PCIP)
	: Super(PCIP)
{

	bReplicates = true;
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
				TheBlock->AttachRootComponentToActor(this);
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

		FBlockData temp = FBlockData();

		temp.Archetype = NewBlockArchtype;
		temp.ConstructorPosition = tempV;

		tempV.X *= 100;
		tempV.Y *= 100;
		tempV.Z *= 100;

		temp.GlobalPosition = tempV;
		// Spawn new block actor
		temp.LevelItem = World->SpawnActor<ALevelBlock>(NewBlockArchtype, tempV, FRotator(0));

		if (!temp.LevelItem)
		{
			return false;
		}

		// Set block values
		temp.LevelItem->ParentWeapon = TheConstructorWeapon;
		temp.LevelItem->AttachRootComponentToActor(this);
		temp.LevelItem->TheBlockConstructor = this;

		// Auto Destroy Block After some time
		if (TheConstructorWeapon && TheConstructorWeapon->bAutoDestroyBlocks)
		{
			// Restore Ammo After Destory
			if (TheConstructorWeapon->bRestoreAmmoAfterBlockDestroy) temp.LevelItem->bRestoreWeaponAmmo = true;

			// Start Block Destroy Event
			temp.LevelItem->StartTimedRestore(TheConstructorWeapon, TheConstructorWeapon->BlockRestoreTime);

		}

		//  Add Block To List
		CurrentBlocks.Add(temp);

		return true;
	}
	return false;
}

bool ALevelBlockConstructor::DestroyBlock(FVector Loc, AActor* TheInstigator)
{
	UWorld* const World = GetWorld();
	if (World)
	{
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

void ALevelBlockConstructor::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ALevelBlockConstructor, CurrentBlocks);
}