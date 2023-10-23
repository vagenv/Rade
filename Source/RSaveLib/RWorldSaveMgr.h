// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "Components/ActorComponent.h"
#include "RWorldSaveMgr.generated.h"

class URSaveGame;

UCLASS(Blueprintable, BlueprintType, ClassGroup=(_Rade), meta=(BlueprintSpawnableComponent))
class RSAVELIB_API URWorldSaveMgr : public UActorComponent
{
   GENERATED_BODY()
public:

   //==========================================================================
   //                  Save Data
   //==========================================================================

   UPROPERTY(EditDefaultsOnly, BlueprintReadonly, Category = "Rade|Save")
      TSubclassOf<URSaveGame> SaveClass;


   UPROPERTY(EditDefaultsOnly, BlueprintReadonly, Category = "Rade|Save")
      TWeakObjectPtr<URSaveGame> SaveGameObject = nullptr;

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

