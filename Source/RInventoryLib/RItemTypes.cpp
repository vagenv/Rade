// Copyright 2015-2023 Vagen Ayrapetyan

#include "RItemTypes.h"
#include "RUtilLib/Rlog.h"
#include "Json.h"
#include "JsonObjectConverter.h"

bool FRItemDataHandle::ToItem (FRItemData &dst) const
{
   // /Script/RInventoryLib.RItemData == Arch.DataTable->RowStructPathName.ToString ()
   //                       RItemData == Arch.DataTable->RowStruct->GetName ();



   if (!ensure (Arch.DataTable    )) return false;
   if (!ensure (Arch.RowName != "")) return false;

   const FRItemData *RowData = Arch.GetRow<FRItemData> ("");

   if (!ensure (RowData)) return false;

   // Get data as base struct
   FRItemData Item (*RowData);
   Item.Count = Count;

   // Copy link to original type
   // Item.Arch = Arch;

   // Get Data Type
   UScriptStruct* RowType = Arch.DataTable->RowStruct;

   // JSON data destination
   FString JsonData;

   // Convert
   bool res = FJsonObjectConverter::UStructToJsonObjectString (RowType, RowData, JsonData, 0, 0, 0);

   if (!ensure (res)) return false;

   // Set RAW data
   Item.JsonData = JsonData;
   dst = Item;
   return true;
}

bool FRItemData::FromJSON (const FString &src, FRItemData &dst)
{
   return FJsonObjectConverter::JsonObjectStringToUStruct (src, &dst, 0, 0);
}

bool FRItemData::ToJSON (const FRItemData &src, FString &dst)
{
   return FJsonObjectConverter::UStructToJsonObjectString<FRItemData> (src, dst);
}

bool FRActionItemData::FromJSON (const FString &src, FRActionItemData &dst)
{
   return FJsonObjectConverter::JsonObjectStringToUStruct (src, &dst, 0, 0);
}

