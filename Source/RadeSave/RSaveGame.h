// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "GameFramework/SaveGame.h"
#include "RSaveGame.generated.h"

USTRUCT(BlueprintType)
struct RADESAVE_API FRSaveData
{
   GENERATED_BODY()
public:

   UPROPERTY(VisibleAnywhere, Category = "Rade")
      TArray<uint8> Data;
};

// Save Game File
UCLASS()
class RADESAVE_API URSaveGame : public USaveGame
{
   GENERATED_BODY()
public:

   URSaveGame (const class FObjectInitializer& PCIP);

   // --- Default Save Slot Info

   // Save Slot Name
   UPROPERTY(VisibleAnywhere, Category = "Rade")
      FString SaveSlotName;
   // User Index
   UPROPERTY(VisibleAnywhere, Category = "Rade")
      uint32 UserIndex;

   // --- Save Data
   UPROPERTY(VisibleAnywhere, Category = "Rade")
      TMap<FString, FRSaveData > RawData;
};
