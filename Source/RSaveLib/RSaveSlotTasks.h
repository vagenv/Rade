// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "Components/ActorComponent.h"
#include "RSaveTypes.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "RSaveSlotTasks.generated.h"

class URSaveGame;

// ============================================================================
//                   Get Save Slot Image binary data Async Task
// ============================================================================

UCLASS()
class RSAVELIB_API UGetSaveGameSlotImageAsync : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable,
             Category = "Rade|Save",
             meta = (BlueprintInternalUseOnly = "true",
                     WorldContext = "WorldContextObject"))
	   static UGetSaveGameSlotImageAsync* GetSaveGameSlotImageAsync (const FRSaveGameMeta &SlotMeta);


   DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRReadSaveSlotImageEvent, const TArray<uint8>&, ImageBinary, bool, success);

   // Called when save game screenshot has been read
	UPROPERTY(BlueprintAssignable)
	   FRReadSaveSlotImageEvent Finished;

   // Execution point
	virtual void Activate () override;

protected:
   UPROPERTY()
      FRSaveGameMeta SlotMeta;

   UPROPERTY()
      TArray<uint8>  ImageBinary;
};

// ============================================================================
//                   List Save Game Slots Async Task
// ============================================================================

UCLASS()
class RSAVELIB_API URListSaveGameSlotsAsync : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable,
             Category = "Rade|Save",
             meta = (BlueprintInternalUseOnly = "true",
                     WorldContext = "WorldContextObject"))
	   static URListSaveGameSlotsAsync* ListSaveGameSlotsAsync ();


   DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRGetSaveSlotsEvent, const TArray<FRSaveGameMeta>&, SaveSlotsList);

   // Called when all save game slots meta information has been read
	UPROPERTY(BlueprintAssignable)
	   FRGetSaveSlotsEvent Finished;

   // Execution point
	virtual void Activate () override;
};

// ============================================================================
//                   Remove Save Game Slot Async Task
// ============================================================================

UCLASS()
class RSAVELIB_API URemoveSaveGameSlotAsync : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable,
             Category = "Rade|Save",
             meta = (BlueprintInternalUseOnly = "true",
                     HidePin      = "WorldContextObject",
                     WorldContext = "WorldContextObject"))
	   static URemoveSaveGameSlotAsync* RemoveSaveGameSlotAsync (UObject* WorldContextObject,
                                                                const FRSaveGameMeta &SaveMeta);


   DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRRemoveSaveSlotsEvent, bool, Success);

   // Called when all content associated with save slot has been removed
	UPROPERTY(BlueprintAssignable)
	   FRRemoveSaveSlotsEvent Finished;

   // Execution point
	virtual void Activate () override;

protected:
   UPROPERTY()
      FRSaveGameMeta SaveMeta;

   UPROPERTY()
      UObject* WorldContextObject = nullptr;
};


// ============================================================================
//                   Create Save Game Slot Async Task
// ============================================================================

UCLASS()
class RSAVELIB_API UCreateSaveGameSlotAsync : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable,
             Category = "Rade|Save",
             meta = (BlueprintInternalUseOnly = "true",
                     HidePin      = "WorldContextObject",
                     WorldContext = "WorldContextObject"))
	   static UCreateSaveGameSlotAsync* CreateSaveGameSlotAsync (UObject* WorldContextObject);


   DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRCreateSaveSlotsEvent, bool, Success);

   // Called when save game slots have been created
	UPROPERTY(BlueprintAssignable)
	   FRCreateSaveSlotsEvent Finished;

   // Execution point
	virtual void Activate () override;

protected:

   UPROPERTY()
      UObject* WorldContextObject = nullptr;

   UPROPERTY();
      TObjectPtr<URSaveGame> SaveGameObject;

   UPROPERTY();
      TArray<uint8> ScreenShotData;

   UFUNCTION()
      void ReportEnd (bool succes);
};

// ============================================================================
//                   Load Save Game Slot Async Task
// ============================================================================

UCLASS()
class RSAVELIB_API ULoadSaveGameSlotAsync : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable,
             Category = "Rade|Save",
             meta = (BlueprintInternalUseOnly = "true",
                     HidePin      = "WorldContextObject",
                     WorldContext = "WorldContextObject"))
	   static ULoadSaveGameSlotAsync* LoadSaveGameSlotAsync (UObject* WorldContextObject,
                                                            const FRSaveGameMeta &SaveMeta);


   DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRLoadSaveSlotsEvent, bool, Success);

   // Called when save game slot content has been read
	UPROPERTY(BlueprintAssignable)
	   FRLoadSaveSlotsEvent Finished;

   // Execution point
	virtual void Activate () override;

protected:
   UPROPERTY()
      FRSaveGameMeta SaveMeta;

   UPROPERTY()
      UObject* WorldContextObject = nullptr;

   UPROPERTY()
      TArray<uint8> SaveBinary;
   
   UPROPERTY()
      TObjectPtr<URSaveGame> SaveGameObject;

   UFUNCTION()
      void ReportEnd (bool succes);
};

