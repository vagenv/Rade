// Copyright 2015-2023 Vagen Ayrapetyan

#include "REquipmentTypes.h"
#include "RUtilLib/RLog.h"
#include "RStatusLib/RStatusEffect.h"
#include "Json.h"
#include "JsonObjectConverter.h"


// ============================================================================
//                      FRConsumableItemData
// ============================================================================

bool FRConsumableItemData::Used (AActor* Owner, URInventoryComponent *Inventory)
{
   for (const TSubclassOf<ARActiveStatusEffect> &ItEffect : ActiveEffects) {
      if (!URStatusEffectUtilLibrary::ApplyStatusEffect_Active (Owner, Owner, ItEffect)) {
         return false;
      }
   }
   return Super::Used (Owner, Inventory);
}

bool FRConsumableItemData::Cast (const FRItemData &src, FRConsumableItemData &dst)
{
   return FJsonObjectConverter::JsonObjectStringToUStruct (src.GetJSON (), &dst, 0, 0, true);
}

bool FRConsumableItemData::ReadJSON ()
{
   // Destination of data to be read
   FRConsumableItemData dst;
   if (!FJsonObjectConverter::JsonObjectStringToUStruct (JsonData, &dst, 0, 0, true)) return false;

   // Create reference for direct assignment
   FRConsumableItemData &obj = *this;

   // Create a backup of JsonData. It will be overwritten by assignment.
   FString JsonDataBackup = JsonData;

   // Should assign all the member variables
   obj = dst;

   // Restore backup
   obj.JsonData = JsonDataBackup;
   return true;
}

bool FRConsumableItemData::WriteJSON ()
{
   FString dst;
   JsonData = "";
   if (!FJsonObjectConverter::UStructToJsonObjectString<FRConsumableItemData> (*this, dst)) return false;
   JsonData = dst;
   return true;
}

// ============================================================================
//                      FREquipmentData
// ============================================================================

bool FREquipmentData::Cast (const FRItemData &src, FREquipmentData &dst)
{
   return FJsonObjectConverter::JsonObjectStringToUStruct<FREquipmentData> (src.GetJSON (), &dst, 0, 0, true);
}

bool FREquipmentData::ReadJSON ()
{
   // Destination of data to be read
   FREquipmentData dst;
   if (!FJsonObjectConverter::JsonObjectStringToUStruct (JsonData, &dst, 0, 0, true)) return false;

   // Create reference for direct assignment
   FREquipmentData &obj = *this;

   // Create a backup of JsonData. It will be overwritten by assignment.
   FString JsonDataBackup = JsonData;

   // Should assign all the member variables
   obj = dst;

   // Restore backup
   obj.JsonData = JsonDataBackup;
   return true;
}

bool FREquipmentData::WriteJSON ()
{
   FString dst;
   JsonData = "";
   if (!FJsonObjectConverter::UStructToJsonObjectString<FREquipmentData> (*this, dst)) return false;
   JsonData = dst;
   return true;
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

