// Copyright 2015-2023 Vagen Ayrapetyan

#include "RWorldSaveMgr.h"
#include "RSaveGame.h"
#include "RUtilLib/RUtil.h"
#include "RUtilLib/RCheck.h"
#include "RUtilLib/RLog.h"

//=============================================================================
//                   Static calls
//=============================================================================

URWorldSaveMgr* URWorldSaveMgr::GetInstance (UObject* WorldContextObject)
{
   return URUtil::GetWorldInstance<URWorldSaveMgr> (WorldContextObject);
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
   if (R_IS_VALID_WORLD) OnSave.Broadcast ();

   return UGameplayStatics::SaveGameToSlot (SaveFile, SaveFile->SaveSlotName, SaveFile->UserIndex);
}

bool URWorldSaveMgr::SaveASync ()
{
   CheckSaveFile ();
   R_LOG_PRINTF ("Saving to [%s] [%lld]", *SaveFile->SaveSlotName, SaveFile->UserIndex);
   if (R_IS_VALID_WORLD) OnSave.Broadcast ();

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
   if (R_IS_VALID_WORLD && SaveFile != nullptr) OnLoad.Broadcast ();
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
   if (R_IS_VALID_WORLD) OnLoad.Broadcast ();
}

//=============================================================================
//                   Data management
//=============================================================================


bool URWorldSaveMgr::SetBuffer (const FString &Key, const TArray<uint8> &Data)
{
   CheckSaveFile ();

   FRSaveData RawData;
   RawData.Data = Data;
   SaveFile->RawData.Add (Key, RawData);
   return true;
}

bool URWorldSaveMgr::SetString (const FString &Key, const TArray<FString> &Data)
{
   CheckSaveFile ();

   // Transform to binary
   TArray<FString> DataCopy (Data);
   FBufferArchive ToBinary;
   ToBinary << DataCopy;

   FRSaveData RawData;
   RawData.Data = ToBinary;
   SaveFile->RawData.Add (Key, RawData);
   return true;
}


bool URWorldSaveMgr::GetBuffer (const FString &Key, TArray<uint8> &Data)
{
   CheckSaveFile ();

   if (!SaveFile->RawData.Contains (Key)) return false;
   FRSaveData RawData = SaveFile->RawData[Key];
   Data = RawData.Data;
   return true;
}

bool URWorldSaveMgr::GetString (const FString &Key, TArray<FString> &Data)
{
   CheckSaveFile ();

   if (!SaveFile->RawData.Contains (Key)) return false;
   FRSaveData RawData = SaveFile->RawData[Key];

   FMemoryReader FromBinary = FMemoryReader (RawData.Data, true);
   FromBinary.Seek(0);

   FromBinary << Data;
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

