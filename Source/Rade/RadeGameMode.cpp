// Copyright 2015 Vagen Ayrapetyan

#include "Rade.h"
#include "RadeGameMode.h"
#include "Custom/SystemSaveGame.h"
#include "Custom/LevelBlockConstructor.h"


ARadeGameMode::ARadeGameMode(const class FObjectInitializer& PCIP) : Super(PCIP)
{

}

void ARadeGameMode::BeginPlay()
{
	Super::BeginPlay();

	FTimerHandle MyHandle;
	GetWorldTimerManager().SetTimer(MyHandle, this, &ARadeGameMode::PostBeginPlay, 0.1f, false);

}

void ARadeGameMode::PostBeginPlay()
{
	if (Role < ROLE_Authority)return;

	UE_LOG(YourLog, Warning, TEXT("               "));
	UE_LOG(YourLog, Warning, TEXT("Game Started"));
	UE_LOG(YourLog, Warning, TEXT("              "));

	//Load Savegame
	//print("Load Game");

	// Loading Save file
	USystemSaveGame* LoadGameInstance = Cast<USystemSaveGame>(UGameplayStatics::CreateSaveGameObject(USystemSaveGame::StaticClass()));
	if (LoadGameInstance)
	{
		LoadGameInstance = Cast<USystemSaveGame>(UGameplayStatics::LoadGameFromSlot(LoadGameInstance->SaveSlotName, LoadGameInstance->UserIndex));
		if (LoadGameInstance)
		{
			//printr("File Loaded");
		//	printr(FString::FromInt(LoadGameInstance->LevelBlocks.Num()));
			SaveFile = LoadGameInstance;
		}
	}


	if (SaveFile)
	{
		//print("Begin play load");
		//print("------------------------");
		//printg("Save file Blocks   " + FString::FromInt(SaveFile->LevelBlocks.Num()));
		if (TheLevelBlockConstructor && TheLevelBlockConstructor->bLoadBLocks)
		{
			TheLevelBlockConstructor->CurrentBlocks= SaveFile->LevelBlocks;
			TheLevelBlockConstructor->Server_UpdateBlocksStatus();
			//printg("Loading into rade block");
		}

	}

	/*

	if (SaveFile)
	{
	//print("Begin play load");
	//print("------------------------");
	for (int32 i=0; i < SaveFile->LevelBlocks.Num(); i++)
	{
	if (SaveFile->LevelBlocks[i].GlobalPosition == FVector(0))
	{
	SaveFile->LevelBlocks.RemoveAt(i);
	//printr("Empty Object--------------------- Deleted");
	//i--;

	}
	else
	{
	/*
	if (SaveFile->LevelBlocks[i].LevelItem)print(SaveFile->LevelBlocks[i].LevelItem->GetName());
	//print(SaveFile->LevelBlocks[i].ArchetypePath.ToString());
	print(SaveFile->LevelBlocks[i].ConstructorPosition.ToString());
	print(SaveFile->LevelBlocks[i].GlobalPosition.ToString());
	*/

	/*
	}



	}
	//	print("------------------------");
	//printr("Game Mode Inventory Loaded  " + FString::FromInt(SaveFile->Items.Num()));
	//printr("Game Mode Blocks Loaded " + FString::FromInt(SaveFile->LevelBlocks.Num()));
	}
	*/
}


void ARadeGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason) 
{
	
	

	if (Role>=ROLE_Authority)
	{
		
		if (!SaveFile)
		{
			UE_LOG(YourLog, Warning, TEXT("               "));
			UE_LOG(YourLog, Warning, TEXT("Create new Save file"));
			UE_LOG(YourLog, Warning, TEXT("               "));
			USystemSaveGame* SaveFile = Cast<USystemSaveGame>(UGameplayStatics::CreateSaveGameObject(USystemSaveGame::StaticClass()));
			UGameplayStatics::SaveGameToSlot(SaveFile, SaveFile->SaveSlotName, SaveFile->UserIndex);
		}

		if (SaveFile)
		{
			if (TheLevelBlockConstructor && TheLevelBlockConstructor->bSaveBlocks)
			{
				SaveFile->LevelBlocks = TheLevelBlockConstructor->CurrentBlocks;
				UE_LOG(YourLog, Warning, TEXT("               "));
				UE_LOG(YourLog, Warning, TEXT("Blocks Saved"));
				UE_LOG(YourLog, Warning, TEXT("               "));
			}



			//FString t = "Inventory Saved  " + FString::FromInt(SaveFile->Items.Num());
			//UE_LOG(YourLog, Warning, TEXT("Inventory Items : %s"), *t);

			FString t = FString::FromInt(SaveFile->LevelBlocks.Num());
			UE_LOG(YourLog, Warning, TEXT("               "));
			UE_LOG(YourLog, Warning, TEXT("Saving Level Blocks :   %s"), *t);
			UE_LOG(YourLog, Warning, TEXT("               "));
			//printr("Blocks Saved " + FString::FromInt(SaveFile->LevelBlocks.Num()));


			//USystemSaveGame* SaveGameInstance = Cast<USystemSaveGame>(UGameplayStatics::CreateSaveGameObject(USystemSaveGame::StaticClass()));
			//SaveGameInstance->LevelBlocks=
			UGameplayStatics::SaveGameToSlot(SaveFile, SaveFile->SaveSlotName, SaveFile->UserIndex);


		}


	}

	UE_LOG(YourLog, Warning, TEXT("               "));
	UE_LOG(YourLog, Warning, TEXT("Game Mode Ended"));
	UE_LOG(YourLog, Warning, TEXT("               "));
	/*
	*/


	Super::EndPlay(EndPlayReason);
}
