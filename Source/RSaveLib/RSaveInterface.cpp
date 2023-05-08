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
   if (!ensure (World_)) return;
   URWorldSaveMgr *SaveMgr_ = URWorldSaveMgr::GetInstance (WorldContextObject);
   if (!ensure (SaveMgr_)) return;

   ObjectSaveId = SaveId;
   World        = World_;
   SaveMgr      = SaveMgr_;

   SaveMgr->OnSave.AddDynamic (this, &IRSaveInterface::OnSave_Internal);
   SaveMgr->OnLoad.AddDynamic (this, &IRSaveInterface::OnLoad_Internal);
}

void IRSaveInterface::OnSave_Internal (URSaveGame* SaveGame)
{
   if (!IsValid (SaveMgr))  return;
   if (!IsValid (SaveGame)) return;

   FBufferArchive ToBinary;

   OnSave (ToBinary);

   FRSaveData SaveData;

   SaveData.Data = ToBinary;
   SaveGame->RawData.Add (ObjectSaveId, SaveData);
}

void IRSaveInterface::OnLoad_Internal (URSaveGame* SaveGame)
{
   if (!IsValid (SaveMgr))  return;
   if (!IsValid (SaveGame)) return;

   if (!SaveGame->RawData.Contains (ObjectSaveId)) return;

   FMemoryReader FromBinary = FMemoryReader (SaveGame->RawData[ObjectSaveId].Data, true);
   FromBinary.Seek(0);

   OnLoad (FromBinary);
}

