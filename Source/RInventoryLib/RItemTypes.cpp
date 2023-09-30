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
   Item.ID = Arch.RowName.ToString ();
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

bool FRItemData::IsValid () const
{
   return !ID.IsEmpty () && !Type.IsEmpty () && !CastType.IsEmpty ();
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

bool FRActionItemData::CanCast (const FRItemData &src)
{
   return src.IsValid () && src.CastType.Contains (FRActionItemData().Type);
}

bool FRActionItemData::Cast (const FRItemData &src, FRActionItemData &dst)
{
   if (!src.IsValid ()) return false;
   if (!CanCast (src)) return false;
   bool res = RJSON::ToStruct (src.GetJSON (), dst);
   dst.ID = src.ID;
   return res;
}

bool FRActionItemData::Used (AActor* Owner, URInventoryComponent *Inventory)
{
   // valid archetype
   if (Action.IsNull ()) return false;

   //URItemAction *ItemBP = Action->GetDefaultObject<URItemAction>();
   //if (!ensure (ItemBP)) return false;

   return true;
}


// ============================================================================
//                      URItemUtilLibrary
// ============================================================================

bool URItemUtilLibrary::Item_IsValid (const FRItemData& ItemData)
{
   return ItemData.IsValid ();
}

bool URItemUtilLibrary::Item_EqualEqual (const FRItemData& A,
                                         const FRItemData& B)
{
   return A.ID == B.ID;
}

bool URItemUtilLibrary::Item_NotEqual (const FRItemData& A,
                                       const FRItemData& B)
{
   return !URItemUtilLibrary::Item_EqualEqual (A, B);
}

bool URItemUtilLibrary::ActionItem_EqualEqual (const FRItemData& A,
                                               const FRActionItemData& B)
{
   return A.ID == B.ID;
}

bool URItemUtilLibrary::ActionItem_NotEqual (const FRItemData& A,
                                             const FRActionItemData& B)
{
   return !URItemUtilLibrary::ActionItem_EqualEqual (A, B);
}


void URItemUtilLibrary::ItemHandle_To_Item (const FRItemDataHandle &src,
                                            FRItemData             &dst,
                                            ERActionResult         &Outcome)
{
   bool res = src.ToItem (dst);
   if (res) Outcome = ERActionResult::Success;
   else     Outcome = ERActionResult::Failure;
}

bool URItemUtilLibrary::Item_Is_ActionItem (const FRItemData &src)
{
   return FRActionItemData::CanCast (src);
}

void URItemUtilLibrary::Item_To_ActionItem (const FRItemData &src,
                                            FRActionItemData &dst,
                                            ERActionResult   &Outcome)
{
   bool res = FRActionItemData::Cast (src, dst);
   if (res) Outcome = ERActionResult::Success;
   else     Outcome = ERActionResult::Failure;
}

bool URItemUtilLibrary::Item_GetRecipe (const UDataTable *RecipeTable,
                                        const FRItemData &Item,
                                        FRCraftRecipe    &Recipe)
{
   if (!IsValid (RecipeTable)) return false;
   if (!Item.IsValid ()) return false;
   FString ContextString;
   TArray<FName> RowNames = RecipeTable->GetRowNames ();
   for (const FName& ItRowName : RowNames) {
      FRItemData ItItem;
      FRCraftRecipe* ItRow = RecipeTable->FindRow<FRCraftRecipe>(ItRowName, ContextString);
      if (ItRow && ItRow->CreateItem.ToItem (ItItem)) {
         if (ItItem.ID == Item.ID) {
            Recipe = FRCraftRecipe (*ItRow);
            return true;
         }
      }
   }
   return false;
}

bool URItemUtilLibrary::Item_IsBreakable (const FRItemData &BreakItem, UDataTable* BreakItemTable)
{
   if (!BreakItem.IsValid ()) return false;
   FRCraftRecipe Recipe;
   return Item_GetRecipe (BreakItemTable, BreakItem, Recipe);
}

bool URItemUtilLibrary::Item_GetBreakList (const FRItemData         &BreakItem,
                                           UDataTable               *BreakItemTable,
                                           TArray<FRItemDataHandle> &ResultItems)
{
   if (!BreakItem.IsValid ()) return false;
   FRCraftRecipe Recipe;
   if (!Item_GetRecipe (BreakItemTable, BreakItem, Recipe)) return false;

   ResultItems = Recipe.RequiredItems;
   return true;
}

