// Copyright 2015-2023 Vagen Ayrapetyan

#include "RItemTypes.h"
#include "RItemAction.h"
#include "RUtilLib/RLog.h"
#include "Json.h"
#include "JsonObjectConverter.h"
#include "RUtilLib/RJson.h"

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
   Item.SetJSON (JsonData);
   dst = Item;
   return true;
}

// ============================================================================
//                      FRItemData
// ============================================================================

FRItemData::FRItemData ()
{
   Type = FString (__func__);
   CastType.AddUnique (Type);
}

bool FRItemData::ReadJSON ()
{
   // Destination of data to be read
   FRItemData dst;
   if (!RJSON::ToStruct (JsonData, dst)) return false;

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
   if (!RJSON::ToString (*this, dst)) return false;
   JsonData = dst;
   return true;
}


// ============================================================================
//                      FRActionItemData
// ============================================================================

FRActionItemData::FRActionItemData ()
{
   Type = FString (__func__);
   CastType.AddUnique (Type);
}

bool FRActionItemData::ReadJSON ()
{
   // Destination of data to be read
   FRActionItemData dst;
   if (!RJSON::ToStruct (JsonData, dst)) return false;

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
   if (!RJSON::ToString (*this, dst)) return false;
   JsonData = dst;
   return true;
}

bool FRActionItemData::Cast (const FRItemData &src, FRActionItemData &dst)
{
   if (!src.CastType.Contains (dst.Type)) return false;
   return RJSON::ToStruct (src.GetJSON (), dst);
}

bool FRActionItemData::Used (AActor* Owner, URInventoryComponent *Inventory)
{
   // valid archetype
   if (!Action) return false;

   URItemAction *ItemBP = Action->GetDefaultObject<URItemAction>();
   if (!ensure (ItemBP)) return false;
   ItemBP->Used (Owner, Inventory, *this);

   return true;
}


// ============================================================================
//                      URItemUtilLibrary
// ============================================================================

void URItemUtilLibrary::ItemHandle_To_Item (const FRItemDataHandle &src, FRItemData &dst,
                                            ERActionResult &Outcome)
{
   bool res = src.ToItem (dst);
   if (res) Outcome = ERActionResult::Success;
   else     Outcome = ERActionResult::Failure;
}

void URItemUtilLibrary::Item_To_ActionItem (const FRItemData &src, FRActionItemData &dst,
                                            ERActionResult &Outcome)
{
   bool res = FRActionItemData::Cast (src, dst);
   if (res) Outcome = ERActionResult::Success;
   else     Outcome = ERActionResult::Failure;
}

