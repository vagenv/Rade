// Copyright 2015-2023 Vagen Ayrapetyan

#include "REquipmentTypes.h"
#include "RUtilLib/Rlog.h"
#include "Json.h"
#include "JsonObjectConverter.h"


// ============================================================================
//                      FRConsumableItemData
// ============================================================================

bool FRConsumableItemData::Cast (const FRItemData &src, FRConsumableItemData &dst)
{
   return FJsonObjectConverter::JsonObjectStringToUStruct (src.GetJSON (), &dst, 0, 0, true);
}

// ============================================================================
//                      FREquipmentData
// ============================================================================

bool FREquipmentData::Cast (const FRItemData &src, FREquipmentData &dst)
{
   return FJsonObjectConverter::JsonObjectStringToUStruct<FREquipmentData> (src.GetJSON (), &dst, 0, 0, true);
}

// ============================================================================
//                      UREquipmentUtilLibrary
// ============================================================================

void UREquipmentUtilLibrary::Item_To_ConsumableItem (const FRItemData &src, FRConsumableItemData &dst,
                                                ERActionResult &Branches)
{
   bool res = FRConsumableItemData::Cast (src, dst);
   if (res) Branches = ERActionResult::Success;
   else     Branches = ERActionResult::Failure;
}

void UREquipmentUtilLibrary::Item_To_EquipmentItem (const FRItemData &src, FREquipmentData &dst,
                                               ERActionResult &Branches)
{
   bool res = FREquipmentData::Cast (src, dst);
   if (res) Branches = ERActionResult::Success;
   else     Branches = ERActionResult::Failure;
}

// ============================================================================
//                      UREquipmentSlot
// ============================================================================
