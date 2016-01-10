// Copyright 2015 Vagen Ayrapetyan

#pragma once

#include "GameFramework/GameMode.h"
#include "RadeGameMode.generated.h"

// Base Rade Game Mode
UCLASS()
class RADE_API ARadeGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:

	ARadeGameMode(const class FObjectInitializer& PCIP);

	// Override Start of Game
	virtual void BeginPlay()override;

	// Delayed BeginPlay
	virtual void PostBeginPlay();

	// Override End of Game
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;


	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//							Impotant Game Components

	// The Level Block Constructor
	UPROPERTY()
	class ALevelBlockConstructor* TheLevelBlockConstructor;

	// The Save Game file
	UPROPERTY()
	class USystemSaveGame* SaveFile;
	
	
};
