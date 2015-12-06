// Copyright 2015 Vagen Ayrapetyan

#pragma once

#include "GameFramework/SaveGame.h"
#include "Item/Inventory.h"
#include "Custom/LevelBlockConstructor.h"
#include "SystemSaveGame.generated.h"


UCLASS()
class RADE_API USystemSaveGame : public USaveGame
{
	GENERATED_BODY()


public:
	USystemSaveGame(const class FObjectInitializer& PCIP);

	// List of Player Inventory Items
	UPROPERTY(VisibleAnywhere, Category = Basic)
		TArray<FItemData> Items;

	// List Data about Level Blocks
	UPROPERTY(VisibleAnywhere, Category = Basic)
		TArray<FBlockData> LevelBlocks;

	// Player Name
	UPROPERTY(VisibleAnywhere, Category = Basic)
		FString PlayerName;

	// Save Slot Name
	UPROPERTY(VisibleAnywhere, Category = Basic)
		FString SaveSlotName;
	// User Index
	UPROPERTY(VisibleAnywhere, Category = Basic)
		uint32 UserIndex;
	
};
