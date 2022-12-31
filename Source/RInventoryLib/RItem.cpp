// Copyright 2015-2023 Vagen Ayrapetyan

#include "RItem.h"
//#include "InventoryComponent.h"
//#include "Net/UnrealNetwork.h"
//#include "Online.h"
//#include "OnlineSubsystem.h"
//#include "GameFramework/Character.h"
//#include "RUtilLib/Log.h"

class AActor;
class URInventoryComponent;
class ARItemPickup;


void URItemAction::Used (AActor *InventoryOwner, URInventoryComponent *Inventory) {
   BP_Used (InventoryOwner, Inventory);
}

void URItemAction::Droped (AActor *InventoryOwner, URInventoryComponent *Inventory, ARItemPickup* Pickup) {
   BP_Droped (InventoryOwner, Inventory, Pickup);
}

/*
// Replicate Player Reference
void AItem::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
   Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}
*/

