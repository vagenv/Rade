// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "Kismet/BlueprintAsyncActionBase.h"
#include "RSaveTypes.generated.h"


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

UCLASS()
class RSAVELIB_API URSaveGameMetaLibrary : public UBlueprintFunctionLibrary
{
   GENERATED_BODY()
public:
   
   // Get Save slots list
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Rade|Save")
		static void GetAllSaveGameSlotsSync (TArray<FRSaveGameMeta> &Result);


   // Get Save slots list
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Rade|Save")
		static bool ReadSaveGameSlotImageSync (const FRSaveGameMeta &SlotMeta, TArray<uint8> &ImageBinary);
};

UCLASS()
class RSAVELIB_API UReadSaveGameSlotImageAsync : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable,
             meta = (BlueprintInternalUseOnly = "true",
                     WorldContext = "WorldContextObject"))
	   static UReadSaveGameSlotImageAsync* ReadSaveGameSlotImageAsync (const FRSaveGameMeta &SlotMeta);


   DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FReadSaveSlotImageEvent, const TArray<uint8>&, ImageBinary, bool, success);

   // Called when all save game slots image has been read
	UPROPERTY(BlueprintAssignable)
	   FReadSaveSlotImageEvent Loaded;

   // Execution point
	virtual void Activate () override;

protected:
   FRSaveGameMeta SlotMeta;
   bool           success = false;
   TArray<uint8>  Result;
};


UCLASS()
class RSAVELIB_API URGetSaveGameSlotsAsync : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable,
             meta = (BlueprintInternalUseOnly = "true",
                     WorldContext = "WorldContextObject"))
	   static URGetSaveGameSlotsAsync* GetAllSaveGameSlotsAsync ();


   DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGetSaveSlotsEvent, const TArray<FRSaveGameMeta>&, SaveSlotsList);

   // Called when all save game slots have been read
	UPROPERTY(BlueprintAssignable)
	   FGetSaveSlotsEvent Loaded;

   // Execution point
	virtual void Activate () override;
};

