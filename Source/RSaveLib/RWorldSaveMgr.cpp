// Copyright 2015-2023 Vagen Ayrapetyan

#include "RWorldSaveMgr.h"
#include "RSaveTypes.h"
#include "RSaveGame.h"
#include "RUtilLib/RUtil.h"
#include "RUtilLib/RCheck.h"
#include "RUtilLib/RLog.h"

//=============================================================================
//                   Static calls
//=============================================================================

URWorldSaveMgr* URWorldSaveMgr::GetInstance (const UObject* WorldContextObject)
{
   return URUtil::GetWorldInstance<URWorldSaveMgr> (WorldContextObject);
}

//=============================================================================
//                   Member calls
//=============================================================================

URWorldSaveMgr::URWorldSaveMgr ()
{
   //OnAsyncSaveDelegate.BindUObject (this, &URWorldSaveMgr::AsyncSaveComplete);
   //OnAsyncLoadDelegate.BindUObject (this, &URWorldSaveMgr::AsyncLoadComplete);
}


void URWorldSaveMgr::ReportSave (URSaveGame* SaveGame)
{
   if (!IsValid (SaveGame)) return;
   if (R_IS_VALID_WORLD && OnSave.IsBound ()) OnSave.Broadcast (SaveGame);
}

void URWorldSaveMgr::ReportLoad (URSaveGame* SaveGame)
{
   if (!IsValid (SaveGame)) return;
   if (R_IS_VALID_WORLD && OnLoad.IsBound ()) OnLoad.Broadcast (SaveGame);
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
   FRSaveGameMeta SaveMeta;
   if (!FRSaveGameMeta::Create (SaveMeta, this)) {
      return false;
   }

   // --- Create save object
   UClass* SaveSlotClass = SaveClass ? SaveClass : URSaveGame::StaticClass ();
   SaveGameObject = Cast<URSaveGame>(UGameplayStatics::CreateSaveGameObject (SaveSlotClass));


   ReportSave (SaveGameObject);

   TArray<uint8> ObjectBytes;
   if (!UGameplayStatics::SaveGameToMemory (SaveGameObject, ObjectBytes)) {
      SaveGameObject = nullptr;
      return false;
   }

   FString SaveFilePath = FRSaveGameMeta::GetSaveDir () + SaveMeta.SlotName + "/save.data";

   if (!FFileHelper::SaveArrayToFile (ObjectBytes, *SaveFilePath)) {
      SaveGameObject = nullptr;
      return false;
   }

   ReportSaveListUpdated ();

   SaveGameObject = nullptr;
   return true;
}

bool URWorldSaveMgr::LoadSync (const FRSaveGameMeta &SaveMeta)
{
   FString SaveFilePath = FRSaveGameMeta::GetSaveDir () + SaveMeta.SlotName + "/save.data";

   // TODO: Add save file header checking.

   TArray<uint8> ObjectBytes;
	if (!FFileHelper::LoadFileToArray (ObjectBytes, *SaveFilePath)) {
		return false;
	}

   SaveGameObject = Cast<URSaveGame> (UGameplayStatics::LoadGameFromMemory (ObjectBytes));
   if (!SaveGameObject) {
      return false;
   }

   ReportLoad (SaveGameObject);

   SaveGameObject = nullptr;
   
   return true;
}

bool URWorldSaveMgr::RemoveSync (const FRSaveGameMeta &SaveMeta)
{
   IFileManager& FileMgr = IFileManager::Get ();
   const FString SaveDir = FRSaveGameMeta::GetSaveDir () + SaveMeta.SlotName + "/";

   if (!FileMgr.DirectoryExists (*SaveDir)) return false;
  
   if (!FileMgr.DeleteDirectory (*SaveDir, true, true)) return false;

   ReportSaveListUpdated ();

   return true;
}

