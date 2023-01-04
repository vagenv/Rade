// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "UObject/Object.h"
#include "RItem.generated.h"

class AActor;
class URInventoryComponent;
class ARItemPickup;

UCLASS(Abstract, DefaultToInstanced, BlueprintType, Blueprintable, EditInlineNew, NotPlaceable)
class RINVENTORYLIB_API URItemAction : public UObject
{
public:
   GENERATED_BODY()

   //=============================================================================
   //          Blueprint events
   //=============================================================================

public:
   // --- Get objects

   // C++
   virtual void Used   (AActor *InventoryOwner, URInventoryComponent *Inventory);
   virtual void Droped (AActor *InventoryOwner, URInventoryComponent *Inventory, ARItemPickup* Pickup);

   // --- Events

   UFUNCTION(BlueprintImplementableEvent, Category = "Rade|Inventory")
      void BP_Used (AActor *InventoryOwner, URInventoryComponent *Inventory);

   UFUNCTION(BlueprintImplementableEvent, Category = "Rade|Inventory")
      void BP_Droped (AActor *InventoryOwner, URInventoryComponent *Inventory, ARItemPickup* Pickup);

};

