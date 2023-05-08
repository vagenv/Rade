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

// ============================================================================
//                   Get Save Slot Image binary data Async Task
// ============================================================================

UGetSaveGameSlotImageAsync* UGetSaveGameSlotImageAsync::GetSaveGameSlotImageAsync (const FRSaveGameMeta &SlotMeta)
{
	UGetSaveGameSlotImageAsync* BlueprintNode = NewObject<UGetSaveGameSlotImageAsync>();
   BlueprintNode->SlotMeta = SlotMeta;
	return BlueprintNode;
}

void UGetSaveGameSlotImageAsync::Activate ()
{
   // Schedule a background lambda thread
   AsyncTask (ENamedThreads::AnyBackgroundThreadNormalTask, [this] () {

      IFileManager& FileMgr = IFileManager::Get ();
      FString SaveFileDir = FRSaveGameMeta::GetSaveDir () + SlotMeta.SlotName + "/";
      FString ImgFilePath = SaveFileDir + "save.img";

      bool success = false;
      if (FileMgr.FileExists (*ImgFilePath)) {
         if (FFileHelper::LoadFileToArray (ImageBinary, *ImgFilePath)){
            success = true;
         }
      }
      // Schedule game thread and pass in result
      AsyncTask (ENamedThreads::GameThread, [this, success] () {

         // Report task end
         Loaded.Broadcast (ImageBinary, success);

         // --- Cleanup
         ImageBinary.Empty ();
      });
   });
}

// ============================================================================
//                   List Save Game Slots Async Task
// ============================================================================

URListSaveGameSlotsAsync* URListSaveGameSlotsAsync::ListSaveGameSlotsAsync ()
{
	URListSaveGameSlotsAsync* BlueprintNode = NewObject<URListSaveGameSlotsAsync>();
	return BlueprintNode;
}

void URListSaveGameSlotsAsync::Activate ()
{
   // Schedule a background lambda thread
   AsyncTask (ENamedThreads::AnyBackgroundThreadNormalTask, [this] () {

      // Perform long sync operation
      TArray<FRSaveGameMeta> Result;
      FRSaveGameMeta::List (Result);

      // Schedule game thread and pass in result
      AsyncTask (ENamedThreads::GameThread, [this, Result] () {

         // Report task end
         this->Loaded.Broadcast (Result);
      });
   });
}

// ============================================================================
//                   Remove Save Game Slot Async Task
// ============================================================================

URemoveSaveGameSlotAsync* URemoveSaveGameSlotAsync::RemoveSaveGameSlotAsync (
   UObject* WorldContextObject,
   const FRSaveGameMeta &SaveMeta)
{
	URemoveSaveGameSlotAsync* BlueprintNode = NewObject<URemoveSaveGameSlotAsync>();
   BlueprintNode->WorldContextObject = WorldContextObject;
   BlueprintNode->SaveMeta           = SaveMeta;
	return BlueprintNode;
}

void URemoveSaveGameSlotAsync::Activate ()
{
   // Schedule a background lambda thread
   AsyncTask (ENamedThreads::AnyBackgroundThreadNormalTask, [this] () {

      // Perform long sync operation
      IFileManager& FileMgr = IFileManager::Get ();
      const FString SaveDir = FRSaveGameMeta::GetSaveDir () + SaveMeta.SlotName + "/";

      bool success = false;
      if (FileMgr.DirectoryExists (*SaveDir)) {
         if (FileMgr.DeleteDirectory (*SaveDir, true, true))  {
            success = true;
         }
      }

      // Schedule game thread and pass in result
      AsyncTask (ENamedThreads::GameThread, [this, success] () {

         if (success) {
            if (URWorldSaveMgr* Mgr = URWorldSaveMgr::GetInstance (WorldContextObject)) {
               Mgr->ReportSaveListUpdated ();
            }
         }

         // Report task end
         Loaded.Broadcast (success);

         // --- Cleanup
         WorldContextObject = nullptr;
      });
   });
}

