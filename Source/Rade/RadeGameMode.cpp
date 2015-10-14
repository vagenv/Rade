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



	// Loading Save file
	USystemSaveGame* LoadGameInstance = Cast<USystemSaveGame>(UGameplayStatics::CreateSaveGameObject(USystemSaveGame::StaticClass()));
	if (LoadGameInstance)
	{
		LoadGameInstance = Cast<USystemSaveGame>(UGameplayStatics::LoadGameFromSlot(LoadGameInstance->SaveSlotName, LoadGameInstance->UserIndex));
		if (LoadGameInstance)
		{
			//printr("File Loaded");

			SaveFile = LoadGameInstance;
		}
	}


	if (SaveFile)
	{
		if (TheLevelBlockConstructor && TheLevelBlockConstructor->bLoadBlocks)
		{
			TheLevelBlockConstructor->CurrentBlocks = SaveFile->LevelBlocks;
			TheLevelBlockConstructor->Server_UpdateBlocksStatus();
		}

	}
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

			FString t = FString::FromInt(SaveFile->LevelBlocks.Num());
			UE_LOG(YourLog, Warning, TEXT("               "));
			UE_LOG(YourLog, Warning, TEXT("Saving Level Blocks :   %s"), *t);
			UE_LOG(YourLog, Warning, TEXT("               "));

			UGameplayStatics::SaveGameToSlot(SaveFile, SaveFile->SaveSlotName, SaveFile->UserIndex);


		}


	}

	UE_LOG(YourLog, Warning, TEXT("               "));
	UE_LOG(YourLog, Warning, TEXT("Game Mode Ended"));
	UE_LOG(YourLog, Warning, TEXT("               "));
	Super::EndPlay(EndPlayReason);
}
