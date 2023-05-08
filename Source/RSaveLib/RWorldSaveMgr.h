// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "Components/ActorComponent.h"
//#include "Kismet/BlueprintAsyncActionBase.h"
#include "RWorldSaveMgr.generated.h"

//class USaveGame;
class URSaveGame;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FRSaveMgrEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRSaveGameEvent, URSaveGame*, SaveGame);


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
   //                  Save data to disk
   //==========================================================================

   UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Rade|Save")
      bool SaveSync ();

   UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Rade|Save")
      bool LoadSync (const FRSaveGameMeta &SlotMeta);
   
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Rade|Save")
		bool RemoveSync (const FRSaveGameMeta &SaveMeta);

   //==========================================================================
   //                  Events
   //==========================================================================

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

   UFUNCTION()
      void ReportSaveListUpdated ();

   // List of Save Slots has changed. (Save/Delete)
   UPROPERTY(BlueprintAssignable, Category = "Rade|Save")
      FRSaveMgrEvent OnSaveListUpdated;
};

