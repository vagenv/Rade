// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "RInventoryLib/RInventoryComponent.h"
#include "REquipmentMgrComponent.generated.h"

class URStatusMgrComponent;
class UREquipmentSlotComponent;
struct FREquipmentData;

// Status Manager Component.
UCLASS(Blueprintable, BlueprintType)
class REQUIPMENTLIB_API UREquipmentMgrComponent : public URInventoryComponent
{
   GENERATED_BODY()
public:

   // Base events
   UREquipmentMgrComponent ();
   virtual void GetLifetimeReplicatedProps (TArray<FLifetimeProperty> &OutLifetimeProps) const override;
   virtual void BeginPlay () override;
   virtual void EndPlay (const EEndPlayReason::Type EndPlayReason) override;

   // Check if item equip
   virtual bool UseItem (int32 ItemIdx) override;

   virtual bool Equip   (const FREquipmentData    &EquipmentData);
   virtual bool UnEquip (UREquipmentSlotComponent *EquipmentSlot);

   inline URStatusMgrComponent*      GetStatusMgr     () const;
   inline UREquipmentSlotComponent * GetEquipmentSlot (const TSubclassOf<UREquipmentSlotComponent> &Type) const;
};

