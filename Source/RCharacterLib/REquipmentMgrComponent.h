// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "RInventoryLib/RInventoryComponent.h"
#include "REquipmentMgrComponent.generated.h"

class UREquipmentSlotComponent;

// Status Manager Component.
UCLASS(Blueprintable, BlueprintType)
class RCHARACTERLIB_API UREquipmentMgrComponent : public URInventoryComponent
{
   GENERATED_BODY()
public:

   // Base events
   UREquipmentMgrComponent ();
   virtual void GetLifetimeReplicatedProps (TArray<FLifetimeProperty> &OutLifetimeProps) const override;
   virtual void BeginPlay () override;
   virtual void EndPlay (const EEndPlayReason::Type EndPlayReason) override;

   UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category = "Rade|Equipment")
      TArray<TSubclassOf<UREquipmentSlotComponent> > DefaultEquipmentSlots;

   // Check if item equip
   virtual bool UseItem (int32 ItemIdx) override;
};

