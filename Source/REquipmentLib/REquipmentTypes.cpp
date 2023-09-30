// Copyright 2015-2023 Vagen Ayrapetyan

#include "REquipmentTypes.h"
#include "RUtilLib/RUtil.h"
#include "RUtilLib/RLog.h"
#include "RUtilLib/RJson.h"
#include "RStatusLib/RActiveStatusEffect.h"
#include "RStatusLib/RStatusMgrComponent.h"


// ============================================================================
//                      FRConsumableItemData
// ============================================================================

FRConsumableItemData::FRConsumableItemData ()
{
   Type = FString (__func__);
   CastType.AddUnique (Type);
}

bool FRConsumableItemData::Used (AActor* Owner, URInventoryComponent *Inventory)
{
   if (!ensure (Owner))     return false;
   if (!ensure (Inventory)) return false;
   URStatusMgrComponent* StatusMgr = URUtil::GetComponent<URStatusMgrComponent> (Owner);
   if (!ensure (StatusMgr)) return false;

   for (const TSoftClassPtr<URActiveStatusEffect> &ItEffect : ActiveEffects) {
      if (!StatusMgr->AddActiveStatusEffect (Owner, ItEffect)) {
         return false;
      }
   }
   if (!Action.IsNull ()) return Super::Used (Owner, Inventory);
   return true;
}

bool FRConsumableItemData::CanCast (const FRItemData &src)
{
   return src.CastType.Contains (FRConsumableItemData().Type);
}

bool FRConsumableItemData::Cast (const FRItemData &src, FRConsumableItemData &dst)
{
   if (!src.IsValid ()) return false;
   if (!CanCast (src)) return false;
   bool res = RJSON::ToStruct (src.GetJSON (), dst);
   dst.ID = src.ID;
   return res;
}

bool FRConsumableItemData::ReadJSON ()
{
   // Destination of data to be read
   FRConsumableItemData dst;
   if (!RJSON::ToStruct (JsonData, dst)) return false;

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
   if (!RJSON::ToString (*this, dst)) return false;
   JsonData = dst;
   return true;
}

// ============================================================================
//                      FREquipmentData
// ============================================================================

FREquipmentData::FREquipmentData ()
{
   Type = FString (__func__);
   CastType.AddUnique (Type);
}

bool FREquipmentData::CanCast (const FRItemData &src)
{
   return src.CastType.Contains (FREquipmentData().Type);
}

bool FREquipmentData::Cast (const FRItemData &src, FREquipmentData &dst)
{
   if (!src.IsValid ()) return false;
   if (!FREquipmentData::CanCast (src)) return false;
   bool res = RJSON::ToStruct (src.GetJSON (), dst);
   dst.ID = src.ID;
   return res;
}

bool FREquipmentData::ReadJSON ()
{
   // Destination of data to be read
   FREquipmentData dst;
   if (!RJSON::ToStruct (JsonData, dst)) return false;

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
   if (!RJSON::ToString (*this, dst)) return false;

   JsonData = dst;
   return true;
}


// ============================================================================
//                      UREquipmentUtilLibrary
// ============================================================================


bool UREquipmentUtilLibrary::ConsumableItem_EqualEqual (
   const FRItemData& A,
   const FRConsumableItemData& B)
{
   return A.ID == B.ID;
}

bool UREquipmentUtilLibrary::ConsumableItem_NotEqual (
   const FRItemData& A,
   const FRConsumableItemData& B)
{
   return !UREquipmentUtilLibrary::ConsumableItem_EqualEqual (A, B);
}

bool UREquipmentUtilLibrary::Equipment_EqualEqual (
   const FRItemData& A,
   const FREquipmentData& B)
{
   return A.ID == B.ID;
}

bool UREquipmentUtilLibrary::Equipment_NotEqual (
   const FRItemData& A,
   const FREquipmentData& B)
{
   return !UREquipmentUtilLibrary::Equipment_EqualEqual (A, B);
}

bool UREquipmentUtilLibrary::Item_Is_ConsumableItem (const FRItemData &src)
{
   return FRConsumableItemData::CanCast (src);
}

void UREquipmentUtilLibrary::Item_To_ConsumableItem (const FRItemData &src, FRConsumableItemData &dst,
                                                     ERActionResult &Branches)
{
   bool res = FRConsumableItemData::Cast (src, dst);
   if (res) Branches = ERActionResult::Success;
   else     Branches = ERActionResult::Failure;
}

bool UREquipmentUtilLibrary::Item_Is_EquipmentItem (const FRItemData &src)
{
   return FREquipmentData::CanCast (src);
}

void UREquipmentUtilLibrary::Item_To_EquipmentItem (const FRItemData &src, FREquipmentData &dst,
                                                    ERActionResult &Branches)
{
   bool res = FREquipmentData::Cast (src, dst);
   if (res) Branches = ERActionResult::Success;
   else     Branches = ERActionResult::Failure;
}

