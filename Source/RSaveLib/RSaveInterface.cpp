// Copyright 2015-2023 Vagen Ayrapetyan

#include "RSaveInterface.h"
#include "RSaveGame.h"
#include "RWorldSaveMgr.h"
#include "RUtilLib/RLog.h"

void IRSaveInterface::Init_Save (const UObject* WorldContextObject, const FString &SaveId)
{
   if (!ensure (IsValid (WorldContextObject))) return;
   if (!ensure (!SaveId.IsEmpty ()))           return;

   const UWorld *World_ = WorldContextObject->GetWorld ();
   if (!ensure (IsValid (World_))) return;
   URWorldSaveMgr *SaveMgr_ = URWorldSaveMgr::GetInstance (WorldContextObject);
   if (!ensure (IsValid (SaveMgr_))) return;

   ObjectSaveId = SaveId;
   World        = World_;
   SaveMgr      = SaveMgr_;

   SaveMgr->OnSave.AddDynamic (this, &IRSaveInterface::OnSave_Internal);
   SaveMgr->OnLoad.AddDynamic (this, &IRSaveInterface::OnLoad_Internal);

   // Check if save is already loaded => Manually trigger event
   if (IsValid (SaveMgr->SaveGameObject) && SaveMgr->SaveGameObject->IsAlreadyLoaded) {
      OnLoad_Internal (SaveMgr->SaveGameObject);
   }
}

void IRSaveInterface::OnSave_Internal (URSaveGame* SaveGame)
{
   if (!IsValid (SaveMgr))  return;
   if (!IsValid (SaveGame)) return;

   FBufferArchive ToBinary;
   OnSave (ToBinary);

   SaveGame->SetBuffer (ObjectSaveId, ToBinary);
}

void IRSaveInterface::OnLoad_Internal (URSaveGame* SaveGame)
{
   if (!IsValid (SaveMgr))  return;
   if (!IsValid (SaveGame)) return;


   TArray<uint8> Data;
   if (!SaveGame->GetBuffer (ObjectSaveId, Data)) return;

   FMemoryReader FromBinary = FMemoryReader (Data, true);
   FromBinary.Seek(0);

   OnLoad (FromBinary);
}

