// Copyright 2015-2022 Vagen Ayrapetyan

#pragma once

#include "GameFramework/PlayerState.h"
#include "RPlayerState.generated.h"

// Custom Player State Infor
UCLASS()
class RADESESSIONMANAGER_API ARPlayerState : public APlayerState
{
   GENERATED_BODY()

public:
   virtual void BeginPlay()override;

   // Number of Times Player Died
   UPROPERTY(Replicated,EditAnywhere, BlueprintReadWrite, Category = "")
      int32 nDeaths = 0;

   // Number of Timer Player Killed
   UPROPERTY(Replicated,EditAnywhere, BlueprintReadWrite, Category = "")
      int32 nKills = 0;
   
   // Player Color
   UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "")
      FLinearColor PlayerColor = FLinearColor::White;
};