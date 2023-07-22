// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "RPlayerController.generated.h"


UCLASS()
class RSESSIONLIB_API ARPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:

	virtual void PawnLeavingGame () override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Network")
		bool DestroyPawnThenUnpossess = false;
};

