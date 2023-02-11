// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "UObject/Object.h"
#include "RItemTypes.h"
#include "RItemAction.generated.h"

class AActor;
class URInventoryComponent;
class ARItemPickup;

UCLASS(Abstract, DefaultToInstanced, BlueprintType, EditInlineNew, NotPlaceable)
class RINVENTORYLIB_API URItemAction : public UBlueprintFunctionLibrary
{
   GENERATED_BODY()
public:

   // C++
   virtual void Used (URInventoryComponent *Inventory, FRItemData ItemData, int32 ItemIdx);

   // Blueprint event, will be called by C++ version
   UFUNCTION(BlueprintImplementableEvent, Category = "Rade|Inventory")
      void BP_Used (URInventoryComponent *Inventory, FRItemData ItemData, int32 ItemIdx);
};

