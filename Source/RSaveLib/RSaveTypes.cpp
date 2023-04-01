// Copyright 2015-2023 Vagen Ayrapetyan

#include "RSaveTypes.h"
#include "RUtilLib/RUtil.h"
#include "RUtilLib/RLog.h"

#include "RUtilLib/RViewCapture.h"

FRSaveGameMeta FRSaveGameMeta::Create (UObject* WorldContextObject)
{
   FRSaveGameMeta Result;
   Result.SlotName = FDateTime::Now ().ToFormattedString (TEXT ("%y%m%d_%H%M"));
   Result.Date     = FDateTime::Now ().ToFormattedString (TEXT ("%Y-%m-%d %H:%M"));

   if (WorldContextObject) {
      Result.BinaryTexture = ARViewCapture::GetScreenShot (WorldContextObject);
   }

   // File Mgr
   IFileManager& FileMgr = IFileManager::Get ();
   const FString DirName = "SaveGames";
   const FString RelPath = FPaths::ProjectSavedDir () + DirName;
   const FString AbsPath = FileMgr.ConvertToAbsolutePathForExternalAppForRead (*(RelPath));

   FString MetaPath = AbsPath + "/" + Result.SlotName + ".savmeta";

   FArchive* Writer = FileMgr.CreateFileWriter (*MetaPath);
   if (Writer) {
      (*Writer) << Result;
      Writer->Close ();
   } else {
      R_LOG_STATIC_PRINTF ("Failed to create meta file: %s", *MetaPath);
   }

   return Result;
}

FRSaveGameMeta FRSaveGameMeta::Read (const FString &SaveDirPath, const FString &SlotName)
{
   // File Mgr
   IFileManager& FileMgr = IFileManager::Get ();

   FString MetaPath = SaveDirPath + "/" + SlotName + ".savmeta";

   FRSaveGameMeta Result;
   Result.SlotName = SlotName;

   if (FileMgr.FileExists (*MetaPath)) {

      TArray<uint8> BinaryArray;

      if (!FFileHelper::LoadFileToArray (BinaryArray, *MetaPath)){
         R_LOG_STATIC_PRINTF ("Corrupted File: ", *MetaPath);
         return Result;
      }

      FMemoryReader FromBinary = FMemoryReader (BinaryArray, true); //true, free data after done
      FromBinary.Seek(0);
      FromBinary << Result;
      FromBinary.FlushCache ();
      BinaryArray.Empty ();

   } else {
      R_LOG_STATIC_PRINTF ("Save Meta file not found: %s", *MetaPath);
   }
   return Result;
}

void FRSaveGameMeta::Remove (const FRSaveGameMeta &SaveMeta)
{
   // File Mgr
   IFileManager& FileMgr = IFileManager::Get ();
   const FString DirName = "SaveGames";
   const FString RelPath = FPaths::ProjectSavedDir () + DirName;
   const FString AbsPath = FileMgr.ConvertToAbsolutePathForExternalAppForRead (*(RelPath));

   const FString SavePath = AbsPath + "/" + SaveMeta.SlotName + ".sav";
   const FString MetaPath = SavePath + "meta";
   if (FileMgr.FileExists (*SavePath)) FileMgr.Delete (*SavePath);
   if (FileMgr.FileExists (*MetaPath)) FileMgr.Delete (*MetaPath);
}

