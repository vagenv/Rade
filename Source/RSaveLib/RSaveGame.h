// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "GameFramework/SaveGame.h"
#include "RSaveGame.generated.h"


USTRUCT(BlueprintType)
struct RSAVELIB_API FRSaveData
{
   GENERATED_BODY()
public:
   UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
      TArray<uint8> Data;
};

UCLASS(BlueprintType, ClassGroup=(_Rade))
class RSAVELIB_API URSaveGame : public USaveGame
{
   GENERATED_BODY()
public:

   // Blueprint subclass can add it's on data.

   // --- Only for CPP content
   //UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = "Rade|Save")
   UPROPERTY()
      TMap<FString, FRSaveData> RawData;
};

