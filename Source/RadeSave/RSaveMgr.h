// Copyright 2015-2022 Vagen Ayrapetyan

#pragma once

#include "Components/ActorComponent.h"
#include "RSaveMgr.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE (FRSaveEvent);

UCLASS()
class RADESAVE_API URSaveMgr : public UActorComponent
{
   GENERATED_BODY()
public:

   URSaveMgr();

   UPROPERTY();
      class URSaveGame* SaveFile;

   // -- Sync calls
   static bool SaveSync (const UWorld *World);
          bool SaveSync ();
   static bool LoadSync (const UWorld *World);
          bool LoadSync ();
 
   // -- Async calls
   static bool SaveASync (const UWorld *World);
          bool SaveASync ();
   static bool LoadASync (const UWorld *World);
          bool LoadASync ();


   // -- Callbacks
   // Before save file is written to disk
   static bool OnSave (const UWorld *World, const FRSaveEvent &Delegate);
          bool OnSave (const FRSaveEvent &Delegate);

   // After save file is read from disk
   static bool OnLoad (const UWorld *World, const FRSaveEvent &Delegate);
          bool OnLoad (const FRSaveEvent &Delegate);


   // -- Data management
   static bool Get (const UWorld *World, const FString &key,       TArray<uint8> &data);     
          bool Get (                     const FString &key,       TArray<uint8> &data);
   static bool Set (const UWorld *World, const FString &key, const TArray<uint8> &data);   
          bool Set (                     const FString &key, const TArray<uint8> &data);

   // Check if save file is valid, if not use default
   void CheckSaveFile ();

   static URSaveMgr * GetInstance (const UWorld *World); 

protected:

   TArray<FRSaveEvent> OnSaveDelegates;
   TArray<FRSaveEvent> OnLoadDelegates;

   void BroadcastSave ();
   void BroadcastLoad ();

   // Called after write of save file finished
   void SaveComplete (const FString &SaveSlot, int32 PlayerIndex, bool bSuccess);

   // Called after read of save file finished
   void LoadComplete (const FString &SaveSlot, int32 PlayerIndex, class USaveGame *SaveGame);
};
