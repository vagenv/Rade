// Copyright 2015-2016 Vagen Ayrapetyan

#include "Rade.h"
#include "RadeGameMode.h"

#include "System/SystemSaveGame.h"
#include "System/RadeGameState.h"
#include "System/RadePlayerState.h"

#include "Custom/LevelBlockConstructor.h"


ARadeGameMode::ARadeGameMode(const class FObjectInitializer& PCIP) : Super(PCIP)
{
	// Overriden Player State Class
	PlayerStateClass = ARadePlayerState::StaticClass();

	// Overriden Game State Class
	GameStateClass = ARadeGameState::StaticClass();
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
			// The Save File Found
			SaveFile = LoadGameInstance;
		}
	}

	if (SaveFile)
	{
		// Load level block data that was saved
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
			// Saving level block data
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
