// Copyright 2015-2023 Vagen Ayrapetyan

#include "RSaveInterface.h"
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

void IRSaveInterface::OnSave_Internal ()
{
   if (!IsValid (SaveMgr)) return;
   FBufferArchive ToBinary;

   OnSave (ToBinary);

   // Set binary data to save file
   if (!ensure (SaveMgr->SetBuffer (ObjectSaveId, ToBinary))) return;
}

void IRSaveInterface::OnLoad_Internal ()
{
   if (!IsValid (SaveMgr)) return;
   // Get binary data from save file
   TArray<uint8> BinaryArray;
   if (!ensure (SaveMgr->GetBuffer (ObjectSaveId, BinaryArray))) return;

   FMemoryReader FromBinary = FMemoryReader (BinaryArray, true);
   FromBinary.Seek(0);

   OnLoad (FromBinary);
}
