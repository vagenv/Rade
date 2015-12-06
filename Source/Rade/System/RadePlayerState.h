// Copyright 2015 Vagen Ayrapetyan

#pragma once

#include "GameFramework/PlayerState.h"
#include "RadePlayerState.generated.h"

/**
 * 
 */
UCLASS()
class RADE_API ARadePlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	virtual void BeginPlay()override;

	UPROPERTY(Replicated,EditAnywhere, BlueprintReadWrite, Category = "")
		int32 DeathCount=0;


	UPROPERTY(Replicated,EditAnywhere, BlueprintReadWrite, Category = "")
		int32 KillCount=0;
	
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "")
		FLinearColor PlayerColor = FLinearColor::White;

};
