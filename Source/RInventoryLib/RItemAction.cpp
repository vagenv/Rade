// Copyright 2015-2023 Vagen Ayrapetyan

#include "RItemAction.h"

void URItemAction::Used (URInventoryComponent *Inventory, FRItemData ItemData, int32 ItemIdx)
{
   BP_Used (Inventory, ItemData, ItemIdx);
}

