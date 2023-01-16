// Copyright 2015-2023 Vagen Ayrapetyan

#include "RItemTypes.h"
#include "RUtilLib/Rlog.h"
#include "Json.h"
#include "JsonObjectConverter.h"

// ============================================================================
//                      FRItemDataHandle
// ============================================================================

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
   // Item.JsonData = JsonData;
   Item.SetJSON (JsonData);
   dst = Item;
   return true;
}

// ============================================================================
//                      FRItemData
// ============================================================================

bool FRItemData::ReadJSON ()
{
   // Destination of data to be read
   FRItemData dst;
   if (!FJsonObjectConverter::JsonObjectStringToUStruct (JsonData, &dst, 0, 0)) return false;

   // Create reference for direct assignment
   FRItemData &obj = *this;

   // Create a backup of JsonData. It will be overwritten by assignment.
   FString JsonDataBackup = JsonData;

   // Should assign all the member variables
   obj = dst;

   // Restore backup
   obj.JsonData = JsonDataBackup;
   return true;
}

bool FRItemData::WriteJSON ()
{
   FString dst;
   JsonData = "";
   if (!FJsonObjectConverter::UStructToJsonObjectString<FRItemData> (*this, dst)) return false;
   JsonData = dst;
   return true;
}

// bool FRItemData::Cast (const FRItemData &src, FRItemData &dst)
// {
//    return FJsonObjectConverter::JsonObjectStringToUStruct (src.GetJSON (), &dst, 0, 0);
// }

// bool FRItemData::FromJSON (const FString &src, FRItemData &dst)
// {
//    return FJsonObjectConverter::JsonObjectStringToUStruct (src, &dst, 0, 0);
// }

// bool FRItemData::ToJSON (const FRItemData &src, FString &dst)
// {
//    return FJsonObjectConverter::UStructToJsonObjectString<FRItemData> (src, dst);
// }

// ============================================================================
//                      FRActionItemData
// ============================================================================

bool FRActionItemData::ReadJSON ()
{
   // Destination of data to be read
   FRActionItemData dst;
   if (!FJsonObjectConverter::JsonObjectStringToUStruct (JsonData, &dst, 0, 0)) return false;

   // Create reference for direct assignment
   FRActionItemData &obj = *this;

   // Create a backup of JsonData. It will be overwritten by assignment.
   FString JsonDataBackup = JsonData;

   // Should assign all the member variables
   obj = dst;

   // Restore backup
   obj.JsonData = JsonDataBackup;
   return true;
}

bool FRActionItemData::WriteJSON ()
{
   FString dst;
   JsonData = "";
   if (!FJsonObjectConverter::UStructToJsonObjectString<FRActionItemData> (*this, dst)) return false;
   JsonData = dst;
   return true;
}

bool FRActionItemData::Cast (const FRItemData &src, FRActionItemData &dst)
{
   return FJsonObjectConverter::JsonObjectStringToUStruct (src.GetJSON (), &dst, 0, 0);
}

// ============================================================================
//                      FRConsumableItemData
// ============================================================================

bool FRConsumableItemData::Cast (const FRItemData &src, FRConsumableItemData &dst)
{
   return FJsonObjectConverter::JsonObjectStringToUStruct (src.GetJSON (), &dst, 0, 0);
}

// ============================================================================
//                      FREquipmentData
// ============================================================================

bool FREquipmentData::Cast (const FRItemData &src, FREquipmentData &dst)
{
   return FJsonObjectConverter::JsonObjectStringToUStruct (src.GetJSON (), &dst, 0, 0);
}

// ============================================================================
//                      URItemUtilLibrary
// ============================================================================

void URItemUtilLibrary::ItemHandle_To_Item (const FRItemDataHandle &src, FRItemData &dst,
                                            ERActionResult &Branches)
{
   bool res = src.ToItem (dst);
   if (res) Branches = ERActionResult::Success;
   else     Branches = ERActionResult::Failure;
}

void URItemUtilLibrary::Item_To_ActionItem (const FRItemData &src, FRActionItemData &dst,
                                            ERActionResult &Branches)
{
   bool res = FRActionItemData::Cast (src, dst);
   if (res) Branches = ERActionResult::Success;
   else     Branches = ERActionResult::Failure;
}

void URItemUtilLibrary::Item_To_ConsumableItem (const FRItemData &src, FRConsumableItemData &dst,
                                                ERActionResult &Branches)
{
   bool res = FRConsumableItemData::Cast (src, dst);
   if (res) Branches = ERActionResult::Success;
   else     Branches = ERActionResult::Failure;
}

void URItemUtilLibrary::Item_To_EquipmentItem (const FRItemData &src, FREquipmentData &dst,
                                               ERActionResult &Branches)
{
   bool res = FREquipmentData::Cast (src, dst);
   if (res) Branches = ERActionResult::Success;
   else     Branches = ERActionResult::Failure;
}

