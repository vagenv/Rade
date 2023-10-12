// Copyright 2015-2023 Vagen Ayrapetyan

#include "RSaveTypes.h"
#include "RUtilLib/RUtil.h"
#include "RUtilLib/RLog.h"
#include "RUtilLib/RJson.h"

#include "RUILib/RViewCapture.h"

bool FRSaveGameMeta::IsValidSave () const
{
   // --- Verify meta information
   if (SlotName.IsEmpty ()) {
      //R_LOG_STATIC_PRINTF ("[%s] Invalid: SlotName", *SlotName);
      return false;
   }
   if (Map.IsEmpty ()) {
      //R_LOG_STATIC_PRINTF ("[%s] Invalid: Map", *SlotName);
      return false;
   }
   if (Level.IsNull ()) {
      //R_LOG_STATIC_PRINTF ("[%s] Invalid: Level", *SlotName);
      return false;
   }
   if (Date.IsEmpty ()) {
      //R_LOG_STATIC_PRINTF ("[%s] Invalid: Date", *SlotName);
      return false;
   }

   // --- Verify data file
   IFileManager& FileMgr = IFileManager::Get ();
   const FString SaveDir = FRSaveGameMeta::GetSaveDir ();

   FFileStatData DataFile = FileMgr.GetStatData (*(SaveDir + SlotName + "/save.data"));
   if (!DataFile.bIsValid || !DataFile.FileSize) {
      //R_LOG_STATIC_PRINTF ("[%s] Invalid: data file", *SlotName);
      return false;
   }

   return true;
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
   if (!ensure (IsValid (WorldContextObject))) return false;
   UWorld *World = URUtil::GetWorld (WorldContextObject);
   if (!World) return false;

   if (SaveMeta.SlotName.IsEmpty ())
      SaveMeta.SlotName = FDateTime::Now ().ToFormattedString (TEXT ("%y%m%d_%H%M"));

   SaveMeta.Date  = FDateTime::Now ().ToFormattedString (TEXT ("%Y-%m-%d %H:%M"));
   SaveMeta.Map   = World->GetMapName ();
   SaveMeta.Level = World;

   return true;
}

bool FRSaveGameMeta::Write (FRSaveGameMeta& SaveMeta)
{
   FString SaveFileDir = FRSaveGameMeta::GetSaveDir () + SaveMeta.SlotName + "/";

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

   // --- Check that file exists and non-zero in size
   FFileStatData MetaFile = FileMgr.GetStatData (*(MetaFilePath));
   if (!MetaFile.bIsValid || !MetaFile.FileSize) {
      //R_LOG_STATIC_PRINTF ("[%s] Invalid: meta file", *MetaFilePath);
      return false;
   }

   // --- Read binary data
   TArray<uint8> BinaryArray;
   if (!FFileHelper::LoadFileToArray (BinaryArray, *MetaFilePath)){
      return false;
   }

   // --- Parse json data
   FString JsonData;
   FFileHelper::BufferToString (JsonData, BinaryArray.GetData (), BinaryArray.Num ());
   if (!RJSON::ToStruct (JsonData, SaveMeta)) {
      //R_LOG_STATIC_PRINTF ("[%s] Failed to parse json data: [%s]", *MetaFilePath, *JsonData);
      return false;
   }

   SaveMeta.SlotName = SlotName;

   // --- Validate save slot
   if (!SaveMeta.IsValidSave ()) {
      //R_LOG_STATIC_PRINTF ("[%s] Invalid: save file", *MetaFilePath);
      return false;
   }

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
      FRSaveGameMeta MetaData;
      if (!FRSaveGameMeta::Read (MetaData, DirList[i])) {
         //R_LOG_STATIC_PRINTF ("[%s] Corrupt: meta file", *DirList[i]);
         continue;
      }

      Result.Add (MetaData);
   }
}

bool URSaveGameMetaUtilLibrary::IsValidSave (const FRSaveGameMeta& MetaFile) {
   return MetaFile.IsValidSave ();
}

