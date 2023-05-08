// Copyright 2015-2023 Vagen Ayrapetyan

#include "RSaveTypes.h"
#include "RUtilLib/RUtil.h"
#include "RUtilLib/RLog.h"
#include "RUtilLib/RJson.h"

#include "RUILib/RViewCapture.h"


bool FRSaveGameMeta::IsValidSave () const
{
   return (
         !SlotName.IsEmpty ()
      && !Map.IsEmpty ()
      && !Date.IsEmpty ()
      );
}

FString FRSaveGameMeta::GetSaveDir ()
{
   IFileManager& FileMgr = IFileManager::Get ();
   const FString RelPath = FPaths::ProjectSavedDir () + "SaveGames/";
   const FString AbsPath = FileMgr.ConvertToAbsolutePathForExternalAppForRead (*(RelPath));
   return AbsPath;
}


bool FRSaveGameMeta::Create (FRSaveGameMeta& SaveMeta, UObject* WorldContextObject)
{
   if (!IsValid (WorldContextObject)) return false;

   SaveMeta.SlotName = FDateTime::Now ().ToFormattedString (TEXT ("%y%m%d_%H%M"));
   SaveMeta.Date     = FDateTime::Now ().ToFormattedString (TEXT ("%Y-%m-%d %H:%M"));
   SaveMeta.Map      = WorldContextObject->GetWorld ()->GetMapName ();

   //SaveMeta.SceenshotTextureBinary = ARViewCapture::GetScreenShot (WorldContextObject);
   //SaveMeta.SceenshotTextureFormat = EPixelFormat::PF_B8G8R8A8;

   FString JsonData;
   if (!RJSON::ToString (SaveMeta, JsonData)) return false;

   // File Mgr
   IFileManager& FileMgr = IFileManager::Get ();
   FString MetaPath = FRSaveGameMeta::GetSaveDir () + SaveMeta.SlotName + "/save.json";

   if (!FFileHelper::SaveStringToFile (JsonData, *MetaPath)) {
      return false;
   }

   return true;
}

bool FRSaveGameMeta::Read (FRSaveGameMeta& SaveMeta, const FString &SlotName)
{
   IFileManager& FileMgr = IFileManager::Get ();
   FString MetaFilePath = FRSaveGameMeta::GetSaveDir () + SlotName + "/save.json";

   if (!FileMgr.FileExists (*MetaFilePath)) return false;

   TArray<uint8> BinaryArray;
   if (!FFileHelper::LoadFileToArray (BinaryArray, *MetaFilePath)){
      return false;
   }

   FString JsonData;
   FFileHelper::BufferToString (JsonData, BinaryArray.GetData (), BinaryArray.Num ());
   if (!RJSON::ToStruct (JsonData, SaveMeta)) {
      return false;
   }

   SaveMeta.SlotName = SlotName;
   return true;
}

void FRSaveGameMeta::List (TArray<FRSaveGameMeta> &Result)
{
   IFileManager& FileMgr = IFileManager::Get ();
   const FString SaveDir = FRSaveGameMeta::GetSaveDir ();

   // Get save slots
   TArray<FString> DirList;
   FileMgr.FindFiles (DirList, *(SaveDir + TEXT ("*")), false, true);

   DirList.Sort ();

   //for (const FString &It : DirList) {
   for (int i = DirList.Num () - 1; i >= 0; i--) {
      const FString &It = DirList[i];

      FFileStatData DataFile = FileMgr.GetStatData (*(SaveDir + It + "/save.data"));
      FFileStatData MetaFile = FileMgr.GetStatData (*(SaveDir + It + "/save.json"));



      if (!DataFile.bIsValid || !DataFile.FileSize) {
         //R_LOG_STATIC_PRINTF ("[%s] Invalid: data file", *It);
         continue;
      }

      if (!MetaFile.bIsValid || !MetaFile.FileSize) {
         //R_LOG_STATIC_PRINTF ("[%s] Invalid: meta file", *It);
         continue;
      }

      FRSaveGameMeta MetaData;
      if (!FRSaveGameMeta::Read (MetaData, It)) {
         //R_LOG_STATIC_PRINTF ("[%s] Corrupt: meta file", *It);
         continue;
      }

      if (!MetaData.IsValidSave ()) {
         //R_LOG_STATIC_PRINTF ("[%s] Invalid save meta data", *It);
         continue;
      }

      Result.Add (MetaData);
   }
}

//=============================================================================
//                   Function Library task
//=============================================================================

void URSaveGameMetaLibrary::GetAllSaveGameSlotsSync (TArray<FRSaveGameMeta> &Result)
{
   FRSaveGameMeta::List (Result);
}

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
      TArray<FRSaveGameMeta> Result;
      URSaveGameMetaLibrary::GetAllSaveGameSlotsSync (Result);

      // Schedule game thread and pass in result
      AsyncTask (ENamedThreads::GameThread, [this, Result] () {

         // Report operation end
         this->Loaded.Broadcast (Result);
      });
   });
}

