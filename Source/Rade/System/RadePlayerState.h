// Copyright 2015-2017 Vagen Ayrapetyan

#pragma once

#include "GameFramework/PlayerState.h"
#include "RadePlayerState.generated.h"

// Custom Player State Infor
UCLASS()
class RADE_API ARadePlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	virtual void BeginPlay()override;

	// Number of Times Player Died
	UPROPERTY(Replicated,EditAnywhere, BlueprintReadWrite, Category = "")
		int32 DeathCount=0;

	// Number of Timer Player Killed
	UPROPERTY(Replicated,EditAnywhere, BlueprintReadWrite, Category = "")
		int32 KillCount=0;
	
	// Player Color
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "")
		FLinearColor PlayerColor = FLinearColor::White;

};
