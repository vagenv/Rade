// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "GameFramework/SaveGame.h"
#include "RSaveGame.generated.h"

// Wrapper for Data.
USTRUCT(BlueprintType)
struct RSAVELIB_API FRSaveData
{
   GENERATED_BODY()
public:
   UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
      TArray<uint8> Data;
};

// Blueprint subclass can add it's own data types.
UCLASS(BlueprintType, ClassGroup=(_Rade))
class RSAVELIB_API URSaveGame : public USaveGame
{
   GENERATED_BODY()
public:

   UPROPERTY(EditDefaultsOnly, BlueprintReadonly, Category = "Rade|Save")
      bool IsAlreadyLoaded = false;

   // --- String Get/Set

   UFUNCTION(BlueprintCallable, Category = "Rade|Save")
      bool GetString (const FString &Key, FString &Value) const;

   UFUNCTION(BlueprintCallable, Category = "Rade|Save")
      bool SetString (const FString &Key, const FString &Value);

   // --- Buffer Get/Set

   UFUNCTION(BlueprintCallable, Category = "Rade|Save")
      bool GetBuffer (const FString &Key, TArray<uint8> &Value) const;

   UFUNCTION(BlueprintCallable, Category = "Rade|Save")
      bool SetBuffer (const FString &Key, const TArray<uint8> &Value);

protected:
   // Serialized content
   UPROPERTY()
      TMap<FString, FRSaveData> RawData;
};

