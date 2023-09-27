// Copyright 2015-2023 Vagen Ayrapetyan

#include "RSaveSlotTasks.h"
#include "RSaveGame.h"
#include "RWorldSaveMgr.h"
#include "RUtilLib/RUtil.h"
#include "RUtilLib/RCheck.h"
#include "RUtilLib/RLog.h"

#include "RUILib/RViewCapture.h"

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

      // Read save slot screenshot
      if (FileMgr.FileExists (*ImgFilePath)) {
         if (FFileHelper::LoadFileToArray (ImageBinary, *ImgFilePath)){
            success = true;
         }
      }

      // Schedule game thread and pass in result
      AsyncTask (ENamedThreads::GameThread, [this, success] () {

         // Report task end
         Finished.Broadcast (ImageBinary, success);

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

      TArray<FRSaveGameMeta> Result;

      // Get all save slots
      FRSaveGameMeta::List (Result);

      // Schedule game thread and pass in result
      AsyncTask (ENamedThreads::GameThread, [this, Result] () {

         // Report task end
         Finished.Broadcast (Result);
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
	URemoveSaveGameSlotAsync* BlueprintNode = NewObject<URemoveSaveGameSlotAsync>(WorldContextObject);
   BlueprintNode->WorldContextObject = WorldContextObject;
   BlueprintNode->SaveMeta           = SaveMeta;
	return BlueprintNode;
}

void URemoveSaveGameSlotAsync::Activate ()
{
   // Schedule a background lambda thread
   AsyncTask (ENamedThreads::AnyBackgroundThreadNormalTask, [this] () {

      IFileManager& FileMgr = IFileManager::Get ();
      const FString SaveDir = FRSaveGameMeta::GetSaveDir () + SaveMeta.SlotName + "/";

      bool success = false;

      // Delete save slot folder.
      if (FileMgr.DirectoryExists (*SaveDir)) {
         if (FileMgr.DeleteDirectory (*SaveDir, true, true))  {
            success = true;
         }
      }

      // Schedule game thread and pass in result
      AsyncTask (ENamedThreads::GameThread, [this, success] () {

         // Report save game list update
         if (success) {
            if (URWorldSaveMgr* Mgr = URWorldSaveMgr::GetInstance (WorldContextObject)) {
               Mgr->ReportSaveListUpdated ();
            } else {
               R_LOG ("URWorldSaveMgr is not loaded. Can't notify of save list update.");
            }
         }

         // Report task end
         Finished.Broadcast (success);

         // --- Cleanup
         WorldContextObject = nullptr;
      });
   });
}

// ============================================================================
//                   Create Save Game Slot Async Task
// ============================================================================

UCreateSaveGameSlotAsync* UCreateSaveGameSlotAsync::CreateSaveGameSlotAsync (
   UObject* WorldContextObject,
   const FString &SlotName,
   const TMap<FString, FString> &ExtraData)
{
	UCreateSaveGameSlotAsync* BlueprintNode = NewObject<UCreateSaveGameSlotAsync>(WorldContextObject);
   BlueprintNode->WorldContextObject = WorldContextObject;
   BlueprintNode->SlotName  = SlotName;
   BlueprintNode->ExtraData = ExtraData;
	return BlueprintNode;
}

void UCreateSaveGameSlotAsync::Activate ()
{
   URWorldSaveMgr* Mgr = URWorldSaveMgr::GetInstance (WorldContextObject);
   if (!Mgr) {
      return ReportEnd (false);
   }

   // --- Create save object
   TSubclassOf<URSaveGame> const SaveSlotClass = Mgr->SaveClass ? Mgr->SaveClass : TSubclassOf<URSaveGame>(URSaveGame::StaticClass ());

   SaveGameObject = Cast<URSaveGame>(UGameplayStatics::CreateSaveGameObject (SaveSlotClass));
   if (!SaveGameObject) {
      return ReportEnd (false);
   }

   // Gather save information
   Mgr->ReportSave (SaveGameObject);

   // Create screenshot
   ARViewCapture::GetScreenShot (WorldContextObject, ScreenShotData);

   FString                NewSave  = SlotName;
   TMap<FString, FString> NewExtra = ExtraData;

   // Schedule a background lambda thread
   AsyncTask (ENamedThreads::AnyBackgroundThreadNormalTask, [this, NewSave, NewExtra] () {

      // Create Save Meta data
      FRSaveGameMeta SaveMeta;
      SaveMeta.SlotName  = NewSave;
      SaveMeta.ExtraData = NewExtra;
      if (!FRSaveGameMeta::Create (SaveMeta, WorldContextObject)) {
         return ReportEnd (false);
      }

      // Optional. Write screenshot data to disk
      FString SaveFileDir = FRSaveGameMeta::GetSaveDir () + SaveMeta.SlotName + "/";
      if (FFileHelper::SaveArrayToFile (ScreenShotData, *(SaveFileDir + "save.img"))) {
         SaveMeta.ImageFormat = EPixelFormat::PF_B8G8R8A8;
      }

      // Write Save Meta to dissk
      if (!FRSaveGameMeta::Write (SaveMeta)) {
         return ReportEnd (false);
      }

      // Convert Save File Object to memory and write to disk
      TArray<uint8> SaveFileData;
      if (!UGameplayStatics::SaveGameToMemory (SaveGameObject, SaveFileData)) {
         return ReportEnd (false);
      }

      // Write Save Game Data to disk
      if (!FFileHelper::SaveArrayToFile (SaveFileData, *(SaveFileDir + "save.data"))) {
         return ReportEnd (false);
      }

      return ReportEnd (true);
   });
}

void UCreateSaveGameSlotAsync::ReportEnd (bool success)
{
   // Schedule game thread and pass in result
   AsyncTask (ENamedThreads::GameThread, [this, success] () {

      // Report save game list update
      if (success && IsValid (WorldContextObject)) {
         if (URWorldSaveMgr* Mgr = URWorldSaveMgr::GetInstance (WorldContextObject)) {
            Mgr->ReportSaveListUpdated ();
         } else {
            R_LOG ("URWorldSaveMgr is not loaded. Can't notify of save list update.");
         }
      }

      // Report task end
      Finished.Broadcast (success);

      // --- Cleanup
      WorldContextObject = nullptr;
      SaveGameObject     = nullptr;
      ScreenShotData.Empty ();
   });
}

// ============================================================================
//                   Load Save Game Slot Async Task
// ============================================================================

ULoadSaveGameSlotAsync* ULoadSaveGameSlotAsync::LoadSaveGameSlotAsync (
   UObject* WorldContextObject,
   const FRSaveGameMeta &SaveMeta)
{
	ULoadSaveGameSlotAsync* BlueprintNode = NewObject<ULoadSaveGameSlotAsync>(WorldContextObject);
   BlueprintNode->WorldContextObject = WorldContextObject;
   BlueprintNode->SaveMeta           = SaveMeta;
	return BlueprintNode;
}

void ULoadSaveGameSlotAsync::Activate ()
{
   // Schedule a background lambda thread
   AsyncTask (ENamedThreads::AnyBackgroundThreadNormalTask, [this] () {

      // Read Save Slot Data to buffer
      FString SaveFilePath = FRSaveGameMeta::GetSaveDir () + SaveMeta.SlotName + "/save.data";
	   if (!FFileHelper::LoadFileToArray (SaveBinary, *SaveFilePath)) {
		   return ReportEnd (false);
	   }

      // Schedule game thread and pass in result
      AsyncTask (ENamedThreads::GameThread, [this] () {

         // Create Save Game Object
         SaveGameObject = Cast<URSaveGame> (UGameplayStatics::LoadGameFromMemory (SaveBinary));

         // Report
         if (SaveGameObject) ReportEnd (true);
         else                ReportEnd (false);
      });
   });
}

void ULoadSaveGameSlotAsync::ReportEnd (bool success)
{
   // Schedule game thread and pass in result
   AsyncTask (ENamedThreads::GameThread, [this, success] () {

      if (success && IsValid (WorldContextObject)) {
         // Provide save data to all registered objects
         if (URWorldSaveMgr* Mgr = URWorldSaveMgr::GetInstance (WorldContextObject)) {
            Mgr->ReportLoad (SaveGameObject);
         } else {
            R_LOG ("URWorldSaveMgr is not loaded. Can't notify of loaded state.");
         }
      }

      // Report task end
      Finished.Broadcast (success);

      // --- Cleanup
      WorldContextObject = nullptr;
      SaveGameObject = nullptr;
   });
}

