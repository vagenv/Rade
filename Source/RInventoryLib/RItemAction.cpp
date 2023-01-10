// Copyright 2015-2023 Vagen Ayrapetyan

#include "RItemAction.h"
//#include "InventoryComponent.h"
//#include "Net/UnrealNetwork.h"
//#include "Online.h"
//#include "OnlineSubsystem.h"
//#include "GameFramework/Character.h"
//#include "RUtilLib/Log.h"

class AActor;
class URInventoryComponent;
class ARItemPickup;


void URItemAction::Used (URInventoryComponent *Inventory, FRItemData ItemData, int32 ItemIdx)
{
   BP_Used (Inventory, ItemData, ItemIdx);
}

// void URItemAction::Droped (URInventoryComponent *Inventory, FRItemData ItemData, ARItemPickup* Pickup)
// {
//    BP_Droped (Inventory, ItemData, Pickup);
// }
