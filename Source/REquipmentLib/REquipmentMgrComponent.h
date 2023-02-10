// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "RInventoryLib/RInventoryComponent.h"
#include "REquipmentMgrComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE (FREquipmentEvent);

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

   UFUNCTION(BlueprintCallable, Category = "Rade|Equipment")
      URStatusMgrComponent*      GetStatusMgr     () const;

   UFUNCTION(BlueprintCallable, Category = "Rade|Equipment")
      UREquipmentSlotComponent * GetEquipmentSlot (const TSubclassOf<UREquipmentSlotComponent> &Type) const;

   // Delegate when equipment updated
   UPROPERTY(BlueprintAssignable, Category = "Rade|Equipment")
      FREquipmentEvent OnEquipmentUpdated;
};

