// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "RGameMode.generated.h"

UCLASS(minimalapi)
class ARGameMode : public AGameModeBase
{
   GENERATED_BODY()

public:
   ARGameMode();

   void BeginPlay () override;
};

