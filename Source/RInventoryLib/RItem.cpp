// Copyright 2015-2023 Vagen Ayrapetyan

#include "RItem.h"
//#include "InventoryComponent.h"
//#include "Net/UnrealNetwork.h"
//#include "Online.h"
//#include "OnlineSubsystem.h"
//#include "GameFramework/Character.h"
//#include "RadeUtil/Log.h"


class AActor;
class URInventoryComponent;
class ARItemPickup;

void URItem::PostCDOContruct ()
{
   Super::PostCDOContruct ();

   // --- A new blueprint class initialized
   if (!ItemData.ItemArch && GetClass () != StaticClass()) {
      ItemData.ItemArch = GetClass ();

      // Assign Description
      if (ItemData.Description == FRItemDescription ()) {
         ItemData.Description.Name    = GetClass()->GetName ();
         ItemData.Description.Tooltip = "Use " + GetClass()->GetName ();
      }
   }
}

void URItem::Used (AActor *InventoryOwner, URInventoryComponent *Inventory) {
   BP_Used (InventoryOwner, Inventory);
}

void URItem::Droped (AActor *InventoryOwner, URInventoryComponent *Inventory, ARItemPickup* Pickup) {
   BP_Droped (InventoryOwner, Inventory, Pickup);
}

/*
// Replicate Player Reference
void AItem::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
   Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}
*/