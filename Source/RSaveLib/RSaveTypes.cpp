// Copyright 2015-2023 Vagen Ayrapetyan

#include "RSaveTypes.h"
#include "RUtilLib/RUtil.h"
#include "RUtilLib/RLog.h"

#include "RUtilLib/RViewCapture.h"

bool FRSaveGameMeta::IsValid () const
{
   return SlotName.IsEmpty ();
}

FRSaveGameMeta FRSaveGameMeta::Create (UObject* WorldContextObject)
{
   FRSaveGameMeta Result;
   Result.SlotName = FDateTime::Now ().ToFormattedString (TEXT ("%y%m%d_%H%M"));
   Result.Date     = FDateTime::Now ().ToFormattedString (TEXT ("%Y-%m-%d %H:%M"));

   if (WorldContextObject) {
      Result.BinaryTexture = ARViewCapture::GetScreenShot (WorldContextObject);
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
      R_LOG_STATIC_PRINTF ("Meta file found: %s", *MetaPath);
   } else {
      R_LOG_STATIC_PRINTF ("Meta file not found: %s", *MetaPath);
   }

   return Result;
}

