// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "REquipmentTypes.h"
#include "RInventoryLib/RInventoryComponent.h"
#include "REquipmentMgrComponent.generated.h"

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
      TArray<TSubclassOf<UREquipmentSlot> > EquipmentSlots;

   UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category = "Rade|Equipment")
      TArray<bool> EquipmentSlotsTaken;




   // Check if item equip
   virtual bool UseItem (int32 ItemIdx) override;
};

