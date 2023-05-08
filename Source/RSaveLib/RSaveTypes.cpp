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

   FString SaveFileDir = FRSaveGameMeta::GetSaveDir () + SaveMeta.SlotName + "/";

   TArray<uint8>ScreenShotData = ARViewCapture::GetScreenShot (WorldContextObject);
   if (!ScreenShotData.IsEmpty ()) {
      if (FFileHelper::SaveArrayToFile (ScreenShotData, *(SaveFileDir + "save.img"))) {
         SaveMeta.ImageFormat = EPixelFormat::PF_B8G8R8A8;
      }
   }

   FString JsonData;
   if (!RJSON::ToString (SaveMeta, JsonData)) return false;

   FString MetaPath = SaveFileDir + "save.json";
   if (!FFileHelper::SaveStringToFile (JsonData, *MetaPath)) {
      return false;
   }

   return true;
}

bool FRSaveGameMeta::Read (FRSaveGameMeta& SaveMeta, const FString &SlotName)
{
   IFileManager& FileMgr = IFileManager::Get ();
   FString SaveFileDir = FRSaveGameMeta::GetSaveDir () + SlotName + "/";
   FString MetaFilePath = SaveFileDir + "save.json";

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

   // Iterate in reverse order
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
