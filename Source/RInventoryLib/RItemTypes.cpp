// Copyright 2015-2023 Vagen Ayrapetyan

#include "RItemTypes.h"
#include "RUtilLib/Rlog.h"
#include "Json.h"
#include "JsonObjectConverter.h"

FRItemData FRItemData::FromJSON (const FString &jsonString)
{
   FRItemData res;
   bool success = FJsonObjectConverter::JsonObjectStringToUStruct (jsonString, &res, 0, 0);
   if (!success) R_LOG_STATIC ("Failed to parse raw JSON data");
   return res;
}

FRItemData FRItemData::FromRow (const FDataTableRowHandle &rowHandle)
{
   FRItemData res;
   const FRItemData *rowData = rowHandle.GetRow<FRItemData> ("");
   if (rowData) {
      res = FRItemData (*rowData);
   } else {
      R_LOG_STATIC (FString::Printf (TEXT ("Failed to read row: %s"), *rowHandle.RowName.ToString ()));
   }

   return res;
}

FString FRItemData::ToJSON () const
{
   FString jsonString;
   bool success = FJsonObjectConverter::UStructToJsonObjectString<FRItemData> (*this, jsonString);
   if (!success) R_LOG_STATIC ("Failed to convert Item object to json string");
   return jsonString;
}

