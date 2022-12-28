// Copyright 2015-2023 Vagen Ayrapetyan

#include "RSaveMgr.h"
#include "RSaveGame.h"

#include "RUtilLib/RLog.h"

//=============================================================================
//                   Static calls
//=============================================================================

URSaveMgr* URSaveMgr::GetInstance (const UWorld *World)
{
   if (!World) return nullptr;

   AGameModeBase *GameMode = World->GetAuthGameMode ();
   if (!GameMode) return nullptr;

   return GameMode->FindComponentByClass<URSaveMgr>();
}

bool URSaveMgr::SaveSync (const UWorld *World)
{
   URSaveMgr* Instance = URSaveMgr::GetInstance (World);
   if (Instance) return Instance->SaveSync ();
   else          return false;
}

bool URSaveMgr::SaveASync (const UWorld *World)
{
   URSaveMgr* Instance = URSaveMgr::GetInstance (World);
   if (Instance) return Instance->SaveASync ();
   else          return false;
}

bool URSaveMgr::LoadSync (const UWorld *World)
{
   URSaveMgr* Instance = URSaveMgr::GetInstance (World);
   if (Instance) return Instance->LoadSync ();
   else          return false;
}

bool URSaveMgr::LoadASync (const UWorld *World)
{
   URSaveMgr* Instance = URSaveMgr::GetInstance (World);
   if (Instance) return Instance->LoadASync ();
   else          return false;
}

bool URSaveMgr::OnSave (const UWorld *World, const FRSaveEvent &Delegate)
{
   URSaveMgr* Instance = URSaveMgr::GetInstance (World);
   if (Instance) return Instance->OnSave (Delegate);
   else          return false;
}

bool URSaveMgr::OnLoad (const UWorld *World, const FRSaveEvent &Delegate)
{
   URSaveMgr* Instance = URSaveMgr::GetInstance (World);
   if (Instance) return Instance->OnLoad (Delegate);
   else          return false;
}

bool URSaveMgr::Get (const UWorld *World, const FString &key, TArray<uint8> &data)
{
   URSaveMgr* Instance = URSaveMgr::GetInstance (World);
   if (Instance) return Instance->Get (key, data);
   else          return false;
}

bool URSaveMgr::Set (const UWorld *World, const FString &key, const TArray<uint8> &data)
{
   URSaveMgr* Instance = URSaveMgr::GetInstance (World);
   if (Instance) return Instance->Set (key, data);
   else          return false;
}


//=============================================================================
//                   Member calls
//=============================================================================

URSaveMgr::URSaveMgr()
{
}

void URSaveMgr::CheckSaveFile ()
{
   if (SaveFile) return;

   // Generate default Save File
   SaveFile = Cast<URSaveGame>(UGameplayStatics::CreateSaveGameObject (URSaveGame::StaticClass()));
}


//=============================================================================
//                   Save/Load
//=============================================================================

bool URSaveMgr::SaveSync ()
{
   CheckSaveFile ();
   BroadcastSave ();
   R_LOG (FString::Printf (TEXT ("Saving to [%s] [%lld]"), *SaveFile->SaveSlotName, SaveFile->UserIndex));
   return UGameplayStatics::SaveGameToSlot (SaveFile, SaveFile->SaveSlotName, SaveFile->UserIndex);
}

bool URSaveMgr::SaveASync ()
{
   CheckSaveFile ();
   BroadcastSave ();
   R_LOG (FString::Printf (TEXT ("Saving to [%s] [%lld]"), *SaveFile->SaveSlotName, SaveFile->UserIndex));

   // Setup save complete delegate.
   FAsyncSaveGameToSlotDelegate SavedDelegate;
   SavedDelegate.BindUObject (this, &URSaveMgr::SaveComplete);
   UGameplayStatics::AsyncSaveGameToSlot (SaveFile, SaveFile->SaveSlotName, SaveFile->UserIndex, SavedDelegate);
   return true;
}

bool URSaveMgr::LoadSync ()
{
   CheckSaveFile ();
   R_LOG (FString::Printf (TEXT ("Loading from [%s] [%lld]"), *SaveFile->SaveSlotName, SaveFile->UserIndex));
   SaveFile = Cast<URSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveFile->SaveSlotName, SaveFile->UserIndex));
   if (SaveFile != nullptr) BroadcastLoad ();
   return (SaveFile != nullptr);
}

bool URSaveMgr::LoadASync ()
{
   CheckSaveFile ();
   R_LOG (FString::Printf (TEXT ("Loading from [%s] [%lld]"), *SaveFile->SaveSlotName, SaveFile->UserIndex));

   FAsyncLoadGameFromSlotDelegate LoadedDelegate;

   // Setup load complete delegate.
   LoadedDelegate.BindUObject (this, &URSaveMgr::LoadComplete);
   UGameplayStatics::AsyncLoadGameFromSlot (SaveFile->SaveSlotName, SaveFile->UserIndex, LoadedDelegate);
   return true;
}


void URSaveMgr::SaveComplete (const FString &SaveSlot, int32 PlayerIndex, bool bSuccess)
{
   if (!bSuccess) {
      R_LOG (FString::Printf (TEXT ("Saving [%s] [%lld] failed")));
      return;
   }
}

void URSaveMgr::LoadComplete (const FString &SaveSlot, int32 PlayerIndex, class USaveGame *SaveGame)
{
   SaveFile = Cast<URSaveGame> (SaveGame);
   if (!SaveFile) {
      R_LOG (FString::Printf (TEXT ("Loading [%s] [%lld] failed")));
      return;
   }
   BroadcastLoad ();
}


//=============================================================================
//                   Callbacks
//=============================================================================


bool URSaveMgr::OnSave (const FRSaveEvent &Delegate)
{
   OnSaveDelegates.Add (Delegate);
   return true;
}


bool URSaveMgr::OnLoad (const FRSaveEvent &Delegate)
{
   OnLoadDelegates.Add (Delegate);
   return true;
}

void URSaveMgr::BroadcastLoad ()
{
  // Broadcast event to all listeners
   for (const FRSaveEvent &event : OnLoadDelegates) {
      event.Broadcast ();
   }
}

void URSaveMgr::BroadcastSave ()
{
  // Broadcast event to all listeners
   for (const FRSaveEvent &event : OnSaveDelegates) {
      event.Broadcast ();
   }
}


//=============================================================================
//                   Data management
//=============================================================================


bool URSaveMgr::Set (const FString &key, const TArray<uint8> &data)
{
   CheckSaveFile ();

   FRSaveData RawData;
   RawData.Data = data;
   SaveFile->RawData.Add (key, RawData);
   return true;
}

bool URSaveMgr::Get (const FString &key, TArray<uint8> &data)
{
   CheckSaveFile ();

   if (!SaveFile->RawData.Contains (key)) return false;
   FRSaveData RawData = SaveFile->RawData[key];
   data = RawData.Data;
   return true;
}

/*
// Read from file

   FVector SaveData;

   TArray<uint8> BinaryArray;

   if (!FFileHelper::LoadFileToArray(BinaryArray, *savePath)){
      rlog("Corrupted File. [FFILEHELPER:>> Invalid File]");
      return ;
   }

   if (BinaryArray.Num() <= 0) return ;


   FMemoryReader FromBinary = FMemoryReader(BinaryArray, true); //true, free data after done
   FromBinary.Seek(0);
   FromBinary << SaveData;

   R_LOG (SaveData.ToString ());


   FromBinary.FlushCache();

   BinaryArray.Empty();
*/

