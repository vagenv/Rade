// Copyright 2015-2023 Vagen Ayrapetyan

#include "RItemTypes.h"
#include "RUtilLib/Rlog.h"
#include "Json.h"
#include "JsonObjectConverter.h"

bool FRItemData::FromJSON (const FString &src, FRItemData &dst)
{
   return FJsonObjectConverter::JsonObjectStringToUStruct (src, &dst, 0, 0);
}

bool FRItemData::FromRow (const FDataTableRowHandle &src, FRItemData &dst)
{
   const FRItemData *rowData = src.GetRow<FRItemData> ("");
   if (!rowData) return false;
   dst = FRItemData (*rowData);
   return true;
}

bool FRItemData::ToJSON (const FRItemData &src, FString &dst)
{
   return FJsonObjectConverter::UStructToJsonObjectString<FRItemData> (src, dst);
}

