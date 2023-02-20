// Copyright 2015-2023 Vagen Ayrapetyan

#include "RItemAction.h"

void URItemAction::Used (AActor* Owner, URInventoryComponent *Inventory, const FRActionItemData &ItemData)
{
   BP_Used (Owner, Inventory, ItemData);
}

