// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "UObject/Object.h"
#include "RItemTypes.h"
#include "RItemAction.generated.h"

class AActor;
class URInventoryComponent;
class ARItemPickup;

UCLASS(Abstract, DefaultToInstanced, BlueprintType, EditInlineNew, NotPlaceable, ClassGroup=(_Rade))
class RINVENTORYLIB_API URItemAction : public UBlueprintFunctionLibrary
{
   GENERATED_BODY()
public:

   // C++
   virtual void Used (AActor* Owner, URInventoryComponent *Inventory, const FRActionItemData &ItemData);

   // Blueprint event, will be called by C++ version
   UFUNCTION(BlueprintImplementableEvent, Category = "Rade|Inventory")
      void BP_Used (AActor* Owner, URInventoryComponent *Inventory, const FRActionItemData &ItemData);
};

