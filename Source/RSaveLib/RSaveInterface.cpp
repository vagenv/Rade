// Copyright 2015-2023 Vagen Ayrapetyan

#include "RSaveInterface.h"
#include "RSaveMgr.h"
#include "RUtilLib/RLog.h"

void IRSaveInterface::Init_Save (UObject* WorldContextObject, const FString &SaveId)
{
   if (!ensure (WorldContextObject)) return;
   if (!ensure (!SaveId.IsEmpty ())) return;

   UWorld *World_ = WorldContextObject->GetWorld ();
   if (!ensure (World_)) return;
   URSaveMgr *SaveMgr_ = URSaveMgr::GetInstance (WorldContextObject);
   if (!ensure (SaveMgr_)) return;

   ObjectSaveId = SaveId;
   World        = World_;
   SaveMgr      = SaveMgr_;

   SaveMgr->OnSave.AddDynamic (this, &IRSaveInterface::OnSave_Internal);
   SaveMgr->OnLoad.AddDynamic (this, &IRSaveInterface::OnLoad_Internal);
}

void IRSaveInterface::OnSave_Internal ()
{
   if (!SaveMgr) return;
   FBufferArchive ToBinary;

   OnSave (ToBinary);

   // Set binary data to save file
   if (!ensure (SaveMgr->Set (ObjectSaveId, ToBinary))) return;
}

void IRSaveInterface::OnLoad_Internal ()
{
   if (!SaveMgr) return;
   // Get binary data from save file
   TArray<uint8> BinaryArray;
   if (!ensure (SaveMgr->Get (ObjectSaveId, BinaryArray))) return;

   FMemoryReader FromBinary = FMemoryReader (BinaryArray, true);
   FromBinary.Seek(0);

   OnLoad (FromBinary);
}
