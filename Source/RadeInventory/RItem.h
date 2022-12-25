// Copyright 2015-2022 Vagen Ayrapetyan

#pragma once

#include "RItemTypes.h"
#include "RItem.generated.h"

class AActor;
class URInventoryComponent;
class ARItemPickup;

UCLASS(Abstract, DefaultToInstanced, BlueprintType, Blueprintable, EditInlineNew, NotPlaceable)
class RADEINVENTORY_API URItem : public UObject
{
public:
   GENERATED_BODY()

   // A hack to initialize default value for class from blueprint state
   virtual void PostCDOContruct () override;

   // -------------------------------------------------------------------------
   //          Core data
   // -------------------------------------------------------------------------

   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rade|Item")
   FRItemData ItemData;

   // -------------------------------------------------------------------------
   //          Blueprint events
   // -------------------------------------------------------------------------

public:
   // --- Get objects

   // C++
   virtual void Used   (AActor *InventoryOwner, URInventoryComponent *Inventory);
   virtual void Droped (AActor *InventoryOwner, URInventoryComponent *Inventory, ARItemPickup* Pickup);

   // --- Events

   UFUNCTION(BlueprintImplementableEvent, Category = "Rade|Item")
      void BP_Used (AActor *InventoryOwner, URInventoryComponent *Inventory);

   UFUNCTION(BlueprintImplementableEvent, Category = "Rade|Item")
      void BP_Droped (AActor *InventoryOwner, URInventoryComponent *Inventory, ARItemPickup* Pickup);

   //UFUNCTION(BlueprintImplementableEvent, Category = "Rade|Item")
   //   void BP_Updated ();

   // Advanced Item Pickup
   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rade|Item")
      TSubclassOf<ARItemPickup> PickupArch;
};

