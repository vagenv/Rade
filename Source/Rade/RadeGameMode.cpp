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

	// Start Post begin delay
	FTimerHandle MyHandle;
	GetWorldTimerManager().SetTimer(MyHandle, this, &ARadeGameMode::PostBeginPlay, 0.1f, false);

}

// Post begin Play
void ARadeGameMode::PostBeginPlay()
{
	if (Role < ROLE_Authority)return;


	// Loading Save file
	USystemSaveGame* LoadGameInstance = Cast<USystemSaveGame>(UGameplayStatics::CreateSaveGameObject(USystemSaveGame::StaticClass()));
	if (LoadGameInstance)
	{
		LoadGameInstance = Cast<USystemSaveGame>(UGameplayStatics::LoadGameFromSlot(LoadGameInstance->SaveSlotName, LoadGameInstance->UserIndex));
		if (LoadGameInstance)
		{
			// Save File Found
			SaveFile = LoadGameInstance;
		}
	}


	if (SaveFile)
	{
		// Load leve block that were saved
		if (TheLevelBlockConstructor && TheLevelBlockConstructor->bLoadBlocks)
		{
			TheLevelBlockConstructor->CurrentBlocks = SaveFile->LevelBlocks;
			TheLevelBlockConstructor->Server_UpdateBlocksStatus();
		}

	}
}

// End Game
void ARadeGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason) 
{
	
	if (Role>=ROLE_Authority)
	{
		
		// No Save File, Creating new
		if (!SaveFile)
		{
			USystemSaveGame* SaveFile = Cast<USystemSaveGame>(UGameplayStatics::CreateSaveGameObject(USystemSaveGame::StaticClass()));
			UGameplayStatics::SaveGameToSlot(SaveFile, SaveFile->SaveSlotName, SaveFile->UserIndex);
		}

		if (SaveFile)
		{
			// Saving level block
			if (TheLevelBlockConstructor && TheLevelBlockConstructor->bSaveBlocks)
			{
				SaveFile->LevelBlocks = TheLevelBlockConstructor->CurrentBlocks;
			}
			// Saving Save File
			UGameplayStatics::SaveGameToSlot(SaveFile, SaveFile->SaveSlotName, SaveFile->UserIndex);
		}
	}
	Super::EndPlay(EndPlayReason);
}
