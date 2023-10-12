// Copyright 2015-2023 Vagen Ayrapetyan

#include "RSaveInterface.h"
#include "RSaveGame.h"
#include "RWorldSaveMgr.h"
#include "RUtilLib/RUtil.h"
#include "RUtilLib/RLog.h"
#include "RUtilLib/RCheck.h"

void IRSaveInterface::Init_Save (const UObject* WorldContextObject, const FString &SaveId_)
{
   if (!ensure (WorldContextObject))  return;
   if (!ensure (!SaveId_.IsEmpty ())) return;
   if (URUtil::GetWorld (WorldContextObject) == nullptr) return;

   URWorldSaveMgr *WorldSaveMgr_ = URWorldSaveMgr::GetInstance (WorldContextObject);
   if (!ensure (WorldSaveMgr_)) return;

   SaveId = SaveId_;
   WorldSaveMgr = WorldSaveMgr_;

   WorldSaveMgr->OnSave.AddDynamic (this, &IRSaveInterface::OnSave_Internal);
   WorldSaveMgr->OnLoad.AddDynamic (this, &IRSaveInterface::OnLoad_Internal);

   // Check if save is already loaded => Manually trigger event
   if (WorldSaveMgr->SaveGameObject.IsValid () && WorldSaveMgr->SaveGameObject->IsAlreadyLoaded) {
      OnLoad_Internal (WorldSaveMgr->SaveGameObject.Get ());
   }
}

void IRSaveInterface::OnSave_Internal (URSaveGame* SaveGame)
{
   if (!ensure (SaveGame)) return;
   if (!WorldSaveMgr.IsValid ()) return;


   FBufferArchive ToBinary;
   OnSave (ToBinary);

   SaveGame->SetBuffer (SaveId, ToBinary);
}

void IRSaveInterface::OnLoad_Internal (URSaveGame* SaveGame)
{
   if (!ensure (SaveGame)) return;
   if (!WorldSaveMgr.IsValid ()) return;

   TArray<uint8> Data;
   if (!SaveGame->GetBuffer (SaveId, Data)) return;

   FMemoryReader FromBinary = FMemoryReader (Data, true);
   FromBinary.Seek(0);

   OnLoad (FromBinary);
}

