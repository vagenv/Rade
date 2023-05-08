// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "Components/ActorComponent.h"
#include "RSaveTypes.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "RWorldSaveMgr.generated.h"

class URSaveGame;


UCLASS(Blueprintable, BlueprintType, ClassGroup=(_Rade), meta=(BlueprintSpawnableComponent))
class RSAVELIB_API URWorldSaveMgr : public UActorComponent
{
   GENERATED_BODY()
public:

   URWorldSaveMgr ();

   //==========================================================================
   //                  Save Data
   //==========================================================================

   UPROPERTY(EditDefaultsOnly, BlueprintReadonly, Category = "Rade|Save")
      TSubclassOf<URSaveGame> SaveClass;

protected:

   // Current Save/Load file
   UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rade|Save");
      TObjectPtr<URSaveGame> SaveGameObject;
public:

   //==========================================================================
   //                  Get instance -> GameState component
   //==========================================================================

   UFUNCTION(BlueprintCallable, BlueprintPure,
             Category = "Rade|Save",
             meta = (HidePin          = "WorldContextObject",
                     DefaultToSelf    = "WorldContextObject",
                     DisplayName      = "World Save Mgr",
                     CompactNodeTitle = "World Save Mgr"))
      static URWorldSaveMgr* GetInstance (const UObject* WorldContextObject);


   //==========================================================================
   //                  Save
   //==========================================================================

   UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Rade|Save")
      bool SaveSync ();

   UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Rade|Save")
      bool LoadSync (const FRSaveGameMeta &SlotMeta);

   //==========================================================================
   //                  Events
   //==========================================================================

   DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRSaveGameEvent, URSaveGame*, SaveGame);

   UFUNCTION()
      void ReportSave (URSaveGame* SaveGame);

   // Save started. Data must be set at this point
   UPROPERTY(BlueprintAssignable, Category = "Rade|Save")
      FRSaveGameEvent OnSave;

   UFUNCTION()
      void ReportLoad (URSaveGame* SaveGame);

   // Load Ended. Data Must be retrieved at this point
   UPROPERTY(BlueprintAssignable, Category = "Rade|Save")
      FRSaveGameEvent OnLoad;


      
   DECLARE_DYNAMIC_MULTICAST_DELEGATE(FRSaveListEvent);

   UFUNCTION()
      void ReportSaveListUpdated ();

   // List of Save Slots has changed. (Save/Delete)
   UPROPERTY(BlueprintAssignable, Category = "Rade|Save")
      FRSaveListEvent OnSaveListUpdated;
};


// ============================================================================
//                   Get Save Slot Image binary data Async Task
// ============================================================================

UCLASS()
class RSAVELIB_API UGetSaveGameSlotImageAsync : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable,
             meta = (BlueprintInternalUseOnly = "true",
                     WorldContext = "WorldContextObject"))
	   static UGetSaveGameSlotImageAsync* GetSaveGameSlotImageAsync (const FRSaveGameMeta &SlotMeta);


   DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRReadSaveSlotImageEvent, const TArray<uint8>&, ImageBinary, bool, success);

   // Called when all save game slots image has been read
	UPROPERTY(BlueprintAssignable)
	   FRReadSaveSlotImageEvent Loaded;

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
             meta = (BlueprintInternalUseOnly = "true",
                     WorldContext = "WorldContextObject"))
	   static URListSaveGameSlotsAsync* ListSaveGameSlotsAsync ();


   DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRGetSaveSlotsEvent, const TArray<FRSaveGameMeta>&, SaveSlotsList);

   // Called when all save game slots have been read
	UPROPERTY(BlueprintAssignable)
	   FRGetSaveSlotsEvent Loaded;

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
             Category = "Rade|UI",
             meta = (BlueprintInternalUseOnly = "true",
                     HidePin      = "WorldContextObject",
                     WorldContext = "WorldContextObject"))
	   static URemoveSaveGameSlotAsync* RemoveSaveGameSlotAsync (UObject* WorldContextObject,
                                                                const FRSaveGameMeta &SaveMeta);


   DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRRemoveSaveSlotsEvent, bool, Success);

   // Called when all save game slots have been read
	UPROPERTY(BlueprintAssignable)
	   FRRemoveSaveSlotsEvent Loaded;

   // Execution point
	virtual void Activate () override;

protected:
   UPROPERTY()
      FRSaveGameMeta SaveMeta;

   UPROPERTY()
      UObject* WorldContextObject = nullptr;
};

