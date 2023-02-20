// Copyright 2015-2023 Vagen Ayrapetyan

#include "RSaveInterface.h"
#include "RSaveMgr.h"
#include  "RUtilLib/RLog.h"

void IRSaveInterface::Init_Save (UWorld *World_, const FString &SaveId)
{
   if (!ensure (World_))             return;
   if (!ensure (!SaveId.IsEmpty ())) return;

   ObjectSaveId = SaveId;
   World = World_;

   FRSaveEvent SavedDelegate;
   SavedDelegate.AddDynamic (this, &IRSaveInterface::OnSave_Internal);
   URSaveMgr::OnSave (World, SavedDelegate);

   FRSaveEvent LoadedDelegate;
   LoadedDelegate.AddDynamic (this, &IRSaveInterface::OnLoad_Internal);
   URSaveMgr::OnLoad (World, LoadedDelegate);
}

void IRSaveInterface::OnSave_Internal ()
{
   FBufferArchive ToBinary;

   OnSave (ToBinary);

   // Set binary data to save file
   if (!ensure (URSaveMgr::Set (World, ObjectSaveId, ToBinary))) return;
}

void IRSaveInterface::OnLoad_Internal ()
{
   // Get binary data from save file
   TArray<uint8> BinaryArray;
   if (!ensure (URSaveMgr::Get (World, ObjectSaveId, BinaryArray))) return;

   FMemoryReader FromBinary = FMemoryReader (BinaryArray, true);
   FromBinary.Seek(0);

   OnLoad (FromBinary);
}
