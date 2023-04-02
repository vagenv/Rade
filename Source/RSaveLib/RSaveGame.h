// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "GameFramework/SaveGame.h"
#include "RSaveTypes.h"
#include "RSaveGame.generated.h"

UCLASS(BlueprintType, ClassGroup=(_Rade))
class RSAVELIB_API URSaveGame : public USaveGame
{
   GENERATED_BODY()
public:

   UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = "Rade|Save")
      TMap<FString, FRSaveData> RawData;
};

