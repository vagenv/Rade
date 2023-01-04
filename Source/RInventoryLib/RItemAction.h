// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "UObject/Object.h"
#include "RItemTypes.h"
#include "RItem.generated.h"

class AActor;
class URInventoryComponent;
class ARItemPickup;

// UCLASS(Abstract, DefaultToInstanced, BlueprintType, Blueprintable, EditInlineNew, NotPlaceable)
UCLASS(Abstract, DefaultToInstanced, BlueprintType, EditInlineNew, NotPlaceable)
class RINVENTORYLIB_API URItemAction : public UBlueprintFunctionLibrary
{
   GENERATED_BODY()
public:

   //=============================================================================
   //          Blueprint events
   //=============================================================================

   // C++
   virtual void Used (URInventoryComponent *Inventory, FRItemData ItemData, int32 ItemIdx);
   // virtual void Droped (URInventoryComponent *Inventory, FRItemData ItemData, ARItemPickup* Pickup);

   // --- BP Events

   UFUNCTION(BlueprintImplementableEvent, Category = "Rade|Inventory")
      void BP_Used (URInventoryComponent *Inventory, FRItemData ItemData, int32 ItemIdx);

   // UFUNCTION(BlueprintImplementableEvent, Category = "Rade|Inventory")
   //    void BP_Droped (URInventoryComponent *Inventory, FRItemData ItemData, ARItemPickup* Pickup);
};

