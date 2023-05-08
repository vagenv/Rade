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

      // Perform long sync operation
      TArray<FRSaveGameMeta> Result;
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
   UObject* WorldContextObject)
{
	UCreateSaveGameSlotAsync* BlueprintNode = NewObject<UCreateSaveGameSlotAsync>(WorldContextObject);
   BlueprintNode->WorldContextObject = WorldContextObject;
	return BlueprintNode;
}

void UCreateSaveGameSlotAsync::Activate ()
{
   URWorldSaveMgr* Mgr = URWorldSaveMgr::GetInstance (WorldContextObject);
   if (!Mgr) {
      return ReportEnd (false);
   }

   // --- Create save object
   UClass* SaveSlotClass = Mgr->SaveClass ? Mgr->SaveClass : URSaveGame::StaticClass ();
   SaveGameObject = Cast<URSaveGame>(UGameplayStatics::CreateSaveGameObject (SaveSlotClass));
   if (!SaveGameObject) {
      return ReportEnd (false);
   }

   // --- Gather save information
   Mgr->ReportSave (SaveGameObject);


   // Create screenshot
   ARViewCapture::GetScreenShot (WorldContextObject, ScreenShotData);


   // Schedule a background lambda thread
   AsyncTask (ENamedThreads::AnyBackgroundThreadNormalTask, [this] () {

      // Create Save Meta data
      FRSaveGameMeta SaveMeta;
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

      if (!FFileHelper::SaveArrayToFile (SaveFileData, *(SaveFileDir + "save.data"))) {
         return ReportEnd (false);
      }
      
      return ReportEnd (true);
   });
}

void UCreateSaveGameSlotAsync::ReportEnd (bool succes)
{
   // Schedule game thread and pass in result
   AsyncTask (ENamedThreads::GameThread, [this, succes] () {

      if (succes && IsValid (WorldContextObject)) {
         if (URWorldSaveMgr* Mgr = URWorldSaveMgr::GetInstance (WorldContextObject)) {
            Mgr->ReportSaveListUpdated ();
         }
      }

      // Report task end
      Finished.Broadcast (succes);

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

      bool ReadSuccess = false;

      FString SaveFilePath = FRSaveGameMeta::GetSaveDir () + SaveMeta.SlotName + "/save.data";

      // TODO: Add save file header checking.

	   if (FFileHelper::LoadFileToArray (SaveBinary, *SaveFilePath)) {
		   ReadSuccess = true;
	   }


      // Schedule game thread and pass in result
      AsyncTask (ENamedThreads::GameThread, [this, ReadSuccess] () {
         bool CreateSuccess = false;

         if (ReadSuccess) {

            SaveGameObject = Cast<URSaveGame> (UGameplayStatics::LoadGameFromMemory (SaveBinary));
            if (SaveGameObject) {
               CreateSuccess = true;

            if (URWorldSaveMgr* Mgr = URWorldSaveMgr::GetInstance (WorldContextObject)) {
                  Mgr->ReportLoad (SaveGameObject);
               }
            }
         }

         // Report task end
         Finished.Broadcast (CreateSuccess);

         // --- Cleanup
         WorldContextObject = nullptr;
         SaveBinary.Empty ();
         SaveGameObject = nullptr;
      });
   });
}

