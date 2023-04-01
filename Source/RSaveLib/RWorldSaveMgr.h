// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "Components/ActorComponent.h"
#include "RWorldSaveMgr.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE (FRSaveEvent);

class USaveGame;
class URSaveGame;

UCLASS(Blueprintable, BlueprintType, ClassGroup=(_Rade), meta=(BlueprintSpawnableComponent) )
class RSAVELIB_API URWorldSaveMgr : public UActorComponent
{
   GENERATED_BODY()
public:

   URWorldSaveMgr();

   UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rade|Save");
      TObjectPtr<URSaveGame> SaveFile;

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
      bool LoadSync ();

   UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Rade|Save")
      bool LoadASync ();

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
      static URWorldSaveMgr* GetInstance (UObject* WorldContextObject);

   //==========================================================================
   //                  Subscribers
   //==========================================================================

   UPROPERTY(BlueprintAssignable, Category = "Rade|Save")
      FRSaveEvent OnSave;

   UPROPERTY(BlueprintAssignable, Category = "Rade|Save")
      FRSaveEvent OnLoad;

protected:


   // Check if save file is valid, if not use default
   void CheckSaveFile ();

   //==========================================================================
   //                  Events
   //==========================================================================

   // Called after write of save file finished
   void SaveComplete (const FString &SaveSlot, int32 PlayerIndex, bool bSuccess);

   // Called after read of save file finished
   void LoadComplete (const FString &SaveSlot, int32 PlayerIndex, USaveGame *SaveGame);
};

