// Copyright 2015 Vagen Ayrapetyan

#pragma once

#include "GameFramework/SaveGame.h"

#include "Item/Inventory.h"
#include "Custom/LevelBlockConstructor.h"

#include "SystemSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class RADE_API USystemSaveGame : public USaveGame
{
	GENERATED_BODY()


public:
	USystemSaveGame(const class FObjectInitializer& PCIP);

	UPROPERTY(VisibleAnywhere, Category = Basic)
		TArray<FItemData> Items;

	UPROPERTY(VisibleAnywhere, Category = Basic)
		TArray<FBlockData> LevelBlocks;
	//FBlockData

	UPROPERTY(VisibleAnywhere, Category = Basic)
		FString PlayerName;

	UPROPERTY(VisibleAnywhere, Category = Basic)
		FString SaveSlotName;

	UPROPERTY(VisibleAnywhere, Category = Basic)
		uint32 UserIndex;
	/*
	*/
	
	
};
