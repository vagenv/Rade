// Copyright 2015-2023 Vagen Ayrapetyan

#include "RWorldSaveMgr.h"
#include "RSaveTypes.h"
#include "RSaveGame.h"
#include "RUtilLib/RUtil.h"
#include "RUtilLib/RCheck.h"
#include "RUtilLib/RLog.h"

//=============================================================================
//                   Async task
//=============================================================================

URGetSaveGameSlotsAsync* URGetSaveGameSlotsAsync::GetAllSaveGameSlotsAsync ()
{
	URGetSaveGameSlotsAsync* BlueprintNode = NewObject<URGetSaveGameSlotsAsync>();
	return BlueprintNode;
}

void URGetSaveGameSlotsAsync::Activate ()
{
   // Schedule a background lambda thread
   AsyncTask (ENamedThreads::AnyBackgroundThreadNormalTask, [this] () {

      // Perform long sync operation
      TArray<FRSaveGameMeta> Result = URWorldSaveMgr::GetAllSaveGameSlotsSync ();

      // Schedule game thread and pass in result
      AsyncTask (ENamedThreads::GameThread, [this, Result] () {

         // Report operation end
         this->Loaded.Broadcast (Result);
      });
   });
}

//=============================================================================
//                   Static calls
//=============================================================================

URWorldSaveMgr* URWorldSaveMgr::GetInstance (const UObject* WorldContextObject)
{
   return URUtil::GetWorldInstance<URWorldSaveMgr> (WorldContextObject);
}

TArray<FRSaveGameMeta> URWorldSaveMgr::GetAllSaveGameSlotsSync ()
{
   TArray<FRSaveGameMeta> Result;

   // File Mgr
   IFileManager& FileMgr = IFileManager::Get ();

   // Const variables
   const FString Empty      = TEXT ("");
   const FString DirName    = "SaveGames";
   const FString RelPath    = FPaths::ProjectSavedDir () + DirName;
   const FString AbsPath    = FileMgr.ConvertToAbsolutePathForExternalAppForRead (*(RelPath));
   const FString FileExt    = "sav";
   const FString FileExtDot = TEXT (".") + FileExt;

   // Get save slots
   TArray<FString> SlotNames;
   FileMgr.FindFiles (SlotNames, *AbsPath, *FileExt);
   SlotNames.Sort ();

   // --- Iterate in reverse order
   for (int i = SlotNames.Num () - 1; i >= 0; i--) {
      FString &ItFile = SlotNames[i];
      ItFile = ItFile.Replace (*FileExtDot, *Empty);
      Result.Add (FRSaveGameMeta::Read (AbsPath, ItFile));
   }
   return Result;
}

//=============================================================================
//                   Member calls
//=============================================================================

URWorldSaveMgr::URWorldSaveMgr ()
{
   OnAsyncSaveDelegate.BindUObject (this, &URWorldSaveMgr::AsyncSaveComplete);
   OnAsyncLoadDelegate.BindUObject (this, &URWorldSaveMgr::AsyncLoadComplete);
}

void URWorldSaveMgr::ReportSave ()
{
   if (R_IS_VALID_WORLD && OnSave.IsBound ()) OnSave.Broadcast ();
}

void URWorldSaveMgr::ReportLoad ()
{
   if (R_IS_VALID_WORLD && OnLoad.IsBound ()) OnLoad.Broadcast ();
}

void URWorldSaveMgr::ReportSaveListUpdated ()
{
   if (R_IS_VALID_WORLD && OnSaveListUpdated.IsBound ()) OnSaveListUpdated.Broadcast ();
}


//=============================================================================
//                   Save
//=============================================================================

bool URWorldSaveMgr::SaveSync ()
{
   // Generate new Save File
   SaveFile = Cast<URSaveGame>(UGameplayStatics::CreateSaveGameObject (URSaveGame::StaticClass ()));

   FRSaveGameMeta SaveMeta = FRSaveGameMeta::Create (this);
   ReportSave ();
   bool Result = UGameplayStatics::SaveGameToSlot (SaveFile, SaveMeta.SlotName, SaveMeta.UserIndex);
   return Result;
}

bool URWorldSaveMgr::SaveASync ()
{
   // Generate new Save File
   SaveFile = Cast<URSaveGame>(UGameplayStatics::CreateSaveGameObject (URSaveGame::StaticClass ()));

   FRSaveGameMeta SaveMeta = FRSaveGameMeta::Create (this);
   ReportSave ();

   UGameplayStatics::AsyncSaveGameToSlot (SaveFile, SaveMeta.SlotName, SaveMeta.UserIndex, OnAsyncSaveDelegate);
   return true;
}

void URWorldSaveMgr::AsyncSaveComplete (const FString &SaveSlot, int32 PlayerIndex, bool bSuccess)
{
   if (!bSuccess) {
      R_LOG_PRINTF ("Saving [%s] [%lld] failed", *SaveSlot, PlayerIndex);
      return;
   }
   ReportSaveListUpdated ();
}
//=============================================================================
//                   Load
//=============================================================================

bool URWorldSaveMgr::LoadSync (const FRSaveGameMeta &SlotMeta)
{
   SaveFile = Cast<URSaveGame>(UGameplayStatics::LoadGameFromSlot (SlotMeta.SlotName, SlotMeta.UserIndex));
   ReportLoad ();
   return (SaveFile != nullptr);
}

bool URWorldSaveMgr::LoadASync (const FRSaveGameMeta &SlotMeta)
{
   UGameplayStatics::AsyncLoadGameFromSlot (SlotMeta.SlotName, SlotMeta.UserIndex, OnAsyncLoadDelegate);
   return true;
}

void URWorldSaveMgr::AsyncLoadComplete (const FString &SaveSlot, int32 PlayerIndex, class USaveGame *SaveGame)
{
   SaveFile = Cast<URSaveGame> (SaveGame);
   if (!SaveFile) {
      R_LOG_PRINTF ("Loading [%s] [%lld] failed", *SaveSlot, PlayerIndex);
      return;
   }
   ReportLoad ();
}

//=============================================================================
//                   Remove
//=============================================================================

void URWorldSaveMgr::RemoveSync (const FRSaveGameMeta &SlotMeta)
{
   FRSaveGameMeta::Remove (SlotMeta);
   ReportSaveListUpdated ();
}

//=============================================================================
//                   Data management
//=============================================================================


bool URWorldSaveMgr::SetBuffer (const FString &Key, const TArray<uint8> &Data)
{
   if (!ensure (IsValid (SaveFile))) return false;

   FRSaveData RawData;
   RawData.Data = Data;
   SaveFile->RawData.Add (Key, RawData);
   return true;
}

bool URWorldSaveMgr::SetString (const FString &Key, const TArray<FString> &Data)
{
   if (!ensure (IsValid (SaveFile))) return false;

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
   if (!ensure (IsValid (SaveFile))) return false;

   if (!SaveFile->RawData.Contains (Key)) return false;
   FRSaveData RawData = SaveFile->RawData[Key];
   Data = RawData.Data;
   return true;
}

bool URWorldSaveMgr::GetString (const FString &Key, TArray<FString> &Data)
{
   if (!ensure (IsValid (SaveFile))) return false;

   if (!SaveFile->RawData.Contains (Key)) return false;
   FRSaveData RawData = SaveFile->RawData[Key];

   FMemoryReader FromBinary = FMemoryReader (RawData.Data, true);
   FromBinary.Seek(0);

   FromBinary << Data;
   return true;
}

