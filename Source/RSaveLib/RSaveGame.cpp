// Copyright 2015-2023 Vagen Ayrapetyan

#include "RSaveGame.h"
#include "RUtilLib/RLog.h"
#include "Serialization/BufferArchive.h"

bool URSaveGame::GetString (const FString &Key, FString &Value) const
{
   // Get Data
   TArray<uint8> BufferData;
   if (!GetBuffer (Key, BufferData)) return false;

   // --- Convert buffer to string
   FMemoryReader FromBinary = FMemoryReader (BufferData, true);
   FromBinary.Seek(0);
   FromBinary << Value;
   return true;
}

bool URSaveGame::SetString (const FString &Key, const FString &Value)
{
   // --- Convert string to buffer
   FString DataCopy = Value;
   FBufferArchive ToBinary;
   ToBinary << DataCopy;

   // Set Data
   return SetBuffer (Key, ToBinary);
}

bool URSaveGame::GetBuffer (const FString &Key, TArray<uint8> &Value) const
{
   if (!RawData.Contains (Key)) return false;

   const FRSaveData* SaveData = RawData.Find (Key);
   if (!SaveData) return false;

   Value = SaveData->Data;
   return true;
}

bool URSaveGame::SetBuffer (const FString &Key, const TArray<uint8> &Value)
{
   if (RawData.Contains (Key)) return false;

   FRSaveData NewData;
   NewData.Data = Value;
   RawData.Add (Key, NewData);

   return true;
}