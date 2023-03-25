// Copyright 2015-2023 Vagen Ayrapetyan

#include "RWorldSaveMgr.h"
#include "RSaveGame.h"

#include "RUtilLib/RLog.h"

//=============================================================================
//                   Static calls
//=============================================================================

URWorldSaveMgr* URWorldSaveMgr::GetInstance (UObject* WorldContextObject)
{
   if (!ensure (WorldContextObject)) return nullptr;

   UWorld *World = WorldContextObject->GetWorld ();
   if (!ensure (World)) return nullptr;

   AGameStateBase *GameState = World->GetGameState ();
   if (!ensure (GameState)) return nullptr;

   URWorldSaveMgr* SaveMgr = GameState->FindComponentByClass<URWorldSaveMgr>();
   return SaveMgr;
}

//=============================================================================
//                   Member calls
//=============================================================================

URWorldSaveMgr::URWorldSaveMgr ()
{
}

void URWorldSaveMgr::CheckSaveFile ()
{
   if (SaveFile) return;

   // Generate default Save File
   SaveFile = Cast<URSaveGame>(UGameplayStatics::CreateSaveGameObject (URSaveGame::StaticClass ()));
}

//=============================================================================
//                   Save
//=============================================================================

bool URWorldSaveMgr::SaveSync ()
{
   CheckSaveFile ();
   R_LOG_PRINTF ("Saving to [%s] [%lld]", *SaveFile->SaveSlotName, SaveFile->UserIndex);
   OnSave.Broadcast ();

   return UGameplayStatics::SaveGameToSlot (SaveFile, SaveFile->SaveSlotName, SaveFile->UserIndex);
}

bool URWorldSaveMgr::SaveASync ()
{
   CheckSaveFile ();
   R_LOG_PRINTF ("Saving to [%s] [%lld]", *SaveFile->SaveSlotName, SaveFile->UserIndex);
   OnSave.Broadcast ();

   // Setup save complete delegate.
   FAsyncSaveGameToSlotDelegate SavedDelegate;
   SavedDelegate.BindUObject (this, &URWorldSaveMgr::SaveComplete);
   UGameplayStatics::AsyncSaveGameToSlot (SaveFile, SaveFile->SaveSlotName, SaveFile->UserIndex, SavedDelegate);
   return true;
}

//=============================================================================
//                   Load
//=============================================================================

bool URWorldSaveMgr::LoadSync ()
{
   CheckSaveFile ();
   R_LOG_PRINTF ("Loading from [%s] [%lld]", *SaveFile->SaveSlotName, SaveFile->UserIndex);
   SaveFile = Cast<URSaveGame>(UGameplayStatics::LoadGameFromSlot (SaveFile->SaveSlotName, SaveFile->UserIndex));
   if (SaveFile != nullptr) OnLoad.Broadcast ();
   return (SaveFile != nullptr);
}

bool URWorldSaveMgr::LoadASync ()
{
   CheckSaveFile ();
   R_LOG_PRINTF ("Loading from [%s] [%lld]", *SaveFile->SaveSlotName, SaveFile->UserIndex);

   FAsyncLoadGameFromSlotDelegate LoadedDelegate;

   // Setup load complete delegate.
   LoadedDelegate.BindUObject (this, &URWorldSaveMgr::LoadComplete);
   UGameplayStatics::AsyncLoadGameFromSlot (SaveFile->SaveSlotName, SaveFile->UserIndex, LoadedDelegate);
   return true;
}

void URWorldSaveMgr::SaveComplete (const FString &SaveSlot, int32 PlayerIndex, bool bSuccess)
{
   if (!bSuccess) {
      R_LOG_PRINTF ("Saving [%s] [%lld] failed", *SaveSlot, PlayerIndex);
      return;
   }
}

void URWorldSaveMgr::LoadComplete (const FString &SaveSlot, int32 PlayerIndex, class USaveGame *SaveGame)
{
   SaveFile = Cast<URSaveGame> (SaveGame);
   if (!SaveFile) {
      R_LOG_PRINTF ("Loading [%s] [%lld] failed", *SaveSlot, PlayerIndex);
      return;
   }
   OnLoad.Broadcast ();
}

//=============================================================================
//                   Data management
//=============================================================================


bool URWorldSaveMgr::Set (const FString &key, const TArray<uint8> &data)
{
   CheckSaveFile ();

   FRSaveData RawData;
   RawData.Data = data;
   SaveFile->RawData.Add (key, RawData);
   return true;
}

bool URWorldSaveMgr::Get (const FString &key, TArray<uint8> &data)
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

