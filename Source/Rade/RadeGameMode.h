// Copyright 2015 Vagen Ayrapetyan

#pragma once

#include "GameFramework/GameMode.h"
#include "RadeGameMode.generated.h"

/**
 * 
 */
UCLASS()
class RADE_API ARadeGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:

	ARadeGameMode(const class FObjectInitializer& PCIP);

	virtual void BeginPlay()override;
	virtual void PostBeginPlay();
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY()
	class ALevelBlockConstructor* TheLevelBlockConstructor;
	UPROPERTY()
	class USystemSaveGame* SaveFile;
	
	
};
