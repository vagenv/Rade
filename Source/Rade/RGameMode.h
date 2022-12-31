// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "RGameMode.generated.h"

class URSaveMgr;

UCLASS()
class RADE_API ARGameMode : public AGameModeBase
{
   GENERATED_BODY()

public:
   ARGameMode ();

   void BeginPlay () override;

   UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Rade")
      TObjectPtr<URSaveMgr> SaveMgr;
};

