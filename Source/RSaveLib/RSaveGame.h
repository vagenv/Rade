// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "GameFramework/SaveGame.h"
#include "RSaveGame.generated.h"

USTRUCT(BlueprintType)
struct RSAVELIB_API FRSaveData
{
   GENERATED_BODY()

   UPROPERTY(VisibleAnywhere, Category = "Rade")
      TArray<uint8> Data;
};

// Save Game File
UCLASS()
class RSAVELIB_API URSaveGame : public USaveGame
{
   GENERATED_BODY()
public:

   URSaveGame ();

   // --- Default Save Slot Info

   // Save Slot Name
   UPROPERTY(EditAnywhere, Category = "Rade|Save")
      FString SaveSlotName;

   // User Index
   UPROPERTY(EditAnywhere, Category = "Rade|Save")
      uint32 UserIndex;

   // --- Save Data
   UPROPERTY(EditAnywhere, Category = "Rade|Save")
      TMap<FString, FRSaveData> RawData;
};

