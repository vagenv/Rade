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

   // <Path>/SaveGames/<SlotName>
   // e.g YYMMDD_HHMM
   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
      FString SlotName;

   // Map file object
   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
      TSoftObjectPtr<UWorld> Level;

   // Map file name
   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
      FString Map;

   // YYYY-MM-DD HH:MM
   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
      FString Date;

   // Extra data from blueprints
   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
      TMap<FString, FString> ExtraData;

   // Binary data format of <Path>/SaveGames/<SlotName>/save.img
   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
      TEnumAsByte<EPixelFormat> ImageFormat = EPixelFormat::PF_B8G8R8A8;

   // Check Values
   bool IsValidSave () const;

   // Get absolute path to save folder
   static FString GetSaveDir ();


   // --- Long Sync functions

   // Creates a New instance with current date as name.
   static bool Create (FRSaveGameMeta &SaveMeta, UObject* WorldContextObject);

   // Write data to disk
   static bool Write (FRSaveGameMeta &SaveMeta);

   // Loads data from disk
   static bool Read (FRSaveGameMeta &SaveMeta, const FString &SlotName);

   // Get list of all available slots
   static void List (TArray<FRSaveGameMeta> &SaveSlots);
};


// ============================================================================
//                   Save Slot Meta Util Library
// ============================================================================

UCLASS(ClassGroup=(_Rade))
class RSAVELIB_API URSaveGameMetaUtilLibrary : public UBlueprintFunctionLibrary
{
   GENERATED_BODY()
public:

   UFUNCTION(BlueprintCallable, Category = "Rade|Status")
      static bool IsValidSave (const FRSaveGameMeta& MetaFile);
};
