// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "RInventoryLib/RInventoryComponent.h"
#include "REquipmentMgrComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE (FREquipmentMgrEvent);

class URStatusMgrComponent;
class UREquipmentSlotComponent;
struct FREquipmentData;

// Status Manager Component.
UCLASS(Blueprintable, BlueprintType, ClassGroup=(_Rade), meta=(BlueprintSpawnableComponent))
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
   virtual bool          UseItem  (int32 ItemIdx) override;
   virtual ARItemPickup* DropItem (int32 ItemIdx, int32 Count = 0) override;

   virtual bool Equip   (const FREquipmentData    &EquipmentData);
   virtual bool Equip   (UREquipmentSlotComponent *EquipmentSlot, const FREquipmentData &EquipmentData);
   virtual bool UnEquip (UREquipmentSlotComponent *EquipmentSlot);

   virtual void CalcWeight ();
private:
   // To evade endless when:
   // OnStatusUpdated -> CalcWeight -> SetEffect -> OnStatusUpdated
   int32 LastWeightMax = 0;

protected:
   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Equipment")
      FRuntimeFloatCurve StrToWeightMax;

   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Status")
      FRuntimeFloatCurve WeightToEvasion;

   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Status")
      FRuntimeFloatCurve WeightToMoveSpeed;

   UFUNCTION(BlueprintCallable, Category = "Rade|Equipment")
      UREquipmentSlotComponent* GetEquipmentSlot (const TSubclassOf<UREquipmentSlotComponent> Type) const;
public:

   // Delegate when equipment updated
   UPROPERTY(BlueprintAssignable, Category = "Rade|Equipment")
      FREquipmentMgrEvent OnEquipmentUpdated;

protected:
   // When status mgr stat values updated
   UFUNCTION()
      void OnStatsUpdated ();

protected:
   virtual void OnSave (FBufferArchive &SaveData) override;
   virtual void OnLoad (FMemoryReader &LoadData) override;
};

