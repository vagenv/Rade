// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "RSaveTypes.generated.h"

// ============================================================================
//                   Save Slot Meta information
// ============================================================================

USTRUCT(BlueprintType)
struct RSAVELIB_API FRSaveGameMeta
{
   GENERATED_BODY()

   // --- Mandatory info
   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
      FString SlotName;

   // Maybe should be hidden/disabled
   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
      int32 UserIndex = 0;

   // UI info
  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
      FString Map;

   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
      FString Date;

   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
      TEnumAsByte<EPixelFormat> ImageFormat = EPixelFormat::PF_B8G8R8A8;


   // Check Values
   bool IsValidSave () const;
   
   // Get absolute path to save folder
   static FString GetSaveDir ();


   // --- Long Sync functions

   // Creates a New instance with current date as name.
   static bool Create (FRSaveGameMeta &SaveMeta, UObject* WorldContextObject);

   // Loads data from disk,.
   static bool Read (FRSaveGameMeta &SaveMeta, const FString &SlotName);

   // Get list of all available slots
   static void List (TArray<FRSaveGameMeta> &SaveSlots);
};

