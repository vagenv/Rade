// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "Components/ActorComponent.h"
#include "RSaveTypes.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/BlueprintAsyncActionBase.h"

#include "RWorldSaveMgr.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE (FRSaveEvent);

class USaveGame;
class URSaveGame;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGetSaveSlotsOutputPin, const TArray<FRSaveGameMeta>&, SaveSlotsList);

UCLASS()
class RSAVELIB_API URGetSaveGameSlotsAsync : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable,
             meta = (BlueprintInternalUseOnly = "true",
                     WorldContext = "WorldContextObject"))
	   static URGetSaveGameSlotsAsync* GetAllSaveGameSlotsAsync ();

   // Called when all save game slots have been read
	UPROPERTY(BlueprintAssignable)
	   FGetSaveSlotsOutputPin Loaded;

   // Execution point
	virtual void Activate () override;
};



UCLASS(Blueprintable, BlueprintType, ClassGroup=(_Rade), meta=(BlueprintSpawnableComponent))
class RSAVELIB_API URWorldSaveMgr : public UActorComponent
{
   GENERATED_BODY()
public:

   URWorldSaveMgr ();


   // Get Save slots list
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly,
             Category = "Rade|Save",
               meta = (HidePin          = "WorldContextObject",
                       DefaultToSelf    = "WorldContextObject"))
		static TArray<FRSaveGameMeta> GetAllSaveGameSlotsSync ();

   //==========================================================================
   //                  Save data to disk
   //==========================================================================

   UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Rade|Save")
      bool SaveSync ();
   UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Rade|Save")
      bool SaveASync ();

   //==========================================================================
   //                  Load data from disk
   //==========================================================================

   UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Rade|Save")
      bool LoadSync (const FRSaveGameMeta &SlotMeta);

   UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Rade|Save")
      bool LoadASync (const FRSaveGameMeta &SlotMeta);

   //==========================================================================
   //                  Remove data from disk
   //==========================================================================
   UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Rade|Save")
      void RemoveSync (const FRSaveGameMeta &SlotMeta);

   //==========================================================================
   //                 Get Data
   //==========================================================================

   UFUNCTION(BlueprintCallable, Category = "Rade|Save")
      bool GetBuffer (const FString &key, TArray<uint8> &data);

   UFUNCTION(BlueprintCallable, Category = "Rade|Save")
      bool GetString (const FString &key, TArray<FString> &data);

   //==========================================================================
   //                  Set Data
   //==========================================================================

   UFUNCTION(BlueprintCallable, Category = "Rade|Save")
      bool SetBuffer (const FString &key, const TArray<uint8> &data);

   UFUNCTION(BlueprintCallable, Category = "Rade|Save")
      bool SetString (const FString &key, const TArray<FString> &data);

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
   //                  Events
   //==========================================================================

   // Save started. Data must be set at this point
   UPROPERTY(BlueprintAssignable, Category = "Rade|Save")
      FRSaveEvent OnSave;

   // Load Ended. Data Must be retrieved at this point
   UPROPERTY(BlueprintAssignable, Category = "Rade|Save")
      FRSaveEvent OnLoad;

   // List of Save Slots has changed. (Save/Delete)
   UPROPERTY(BlueprintAssignable, Category = "Rade|Save")
      FRSaveEvent OnSaveListUpdated;

   //==========================================================================
   //                  Async Callback
   //==========================================================================
private:

   // Async Save
   FAsyncSaveGameToSlotDelegate OnAsyncSaveDelegate;

   // Called after write of save file finished
   UFUNCTION()
      void AsyncSaveComplete (const FString &SaveSlot, int32 PlayerIndex, bool bSuccess);

   // Async Load
   FAsyncLoadGameFromSlotDelegate OnAsyncLoadDelegate;

   // Called after read of save file finished
   UFUNCTION()
      void AsyncLoadComplete (const FString &SaveSlot, int32 PlayerIndex, USaveGame *SaveGame);

protected:

   // Current Save/Load file
   UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rade|Save");
      TObjectPtr<URSaveGame> SaveFile;
};

