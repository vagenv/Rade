// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "RSaveTypes.generated.h"

USTRUCT(BlueprintType)
struct RSAVELIB_API FRSaveData
{
   GENERATED_BODY()

   UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
      TArray<uint8> Data;
};

USTRUCT(BlueprintType)
struct RSAVELIB_API FRSaveGameMeta
{
   GENERATED_BODY()

   // --- Mandatory info
   UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
      FString SlotName = "";

   // Maybe should be hidden/disabled
   UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
      int32 UserIndex = 0;

   // UI info
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
      FString Map = "???";

   UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
      FString Date = "???";

   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
      TArray<uint8> SceenshotTextureBinary;

   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
      TEnumAsByte<EPixelFormat> SceenshotTextureFormat = EPixelFormat::PF_B8G8R8A8;

   // For serialization of data
   friend FArchive& operator << (FArchive& Ar, FRSaveGameMeta &MetaData) {
      Ar << MetaData.SlotName;
      Ar << MetaData.UserIndex;
      Ar << MetaData.Map;
      Ar << MetaData.Date;
      // Ar << MetaData.SceenshotTextureFormat;
      Ar << MetaData.SceenshotTextureBinary;

      return Ar;
   }

   // Creates a New instance with current date as name.
   static FRSaveGameMeta Create (UObject* WorldContextObject);

   // Loads data from disk if available.
   static FRSaveGameMeta Read   (const FString &SaveDirPath,  const FString &SlotName);

   // Remove save from disk if available.
   static void           Remove (const FRSaveGameMeta &SaveMeta);
};

