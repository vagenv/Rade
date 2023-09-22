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


   UFUNCTION(BlueprintCallable, Category = "Rade|Equipment")
      UREquipmentSlotComponent* GetEquipmentSlot (const TSubclassOf<UREquipmentSlotComponent> Type) const;

   //==========================================================================
   //                 Use / Drop override. Check if Item equiped.
   //==========================================================================
   virtual bool UseItem  (int32 ItemIdx) override;
   virtual bool DropItem (int32 ItemIdx, int32 Count = 0) override;

   //==========================================================================
   //                 Equip/unequip
   //==========================================================================
public:
   UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Rade|Equipment")
      virtual bool EquipItem (const FREquipmentData &EquipmentData);

   UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Rade|Equipment")
      virtual bool Equip (UREquipmentSlotComponent *EquipmentSlot, const FREquipmentData &EquipmentData);

   UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Rade|Equipment")
      virtual bool UnEquip (UREquipmentSlotComponent *EquipmentSlot);

   //==========================================================================
   //                 Server versions of the functions
   //==========================================================================

   UFUNCTION(Server, Reliable, BlueprintCallable, BlueprintAuthorityOnly, Category = "Rade|Equipment")
      virtual void EquipItem_Server                (const FREquipmentData &EquipmentData);
      virtual void EquipItem_Server_Implementation (const FREquipmentData &EquipmentData);

   UFUNCTION(Server, Reliable, BlueprintCallable, BlueprintAuthorityOnly, Category = "Rade|Equipment")
      virtual void Equip_Server                (UREquipmentSlotComponent *EquipmentSlot,
                                                const FREquipmentData    &EquipmentData);
      virtual void Equip_Server_Implementation (UREquipmentSlotComponent *EquipmentSlot,
                                                const FREquipmentData    &EquipmentData);

   UFUNCTION(Server, Reliable, BlueprintCallable, BlueprintAuthorityOnly, Category = "Rade|Equipment")
      virtual void UnEquip_Server                (UREquipmentSlotComponent *EquipmentSlot);
      virtual void UnEquip_Server_Implementation (UREquipmentSlotComponent *EquipmentSlot);

   //==========================================================================
   //                 Weight calculation
   //==========================================================================
protected:
   virtual void CalcWeight () override;

   // When status mgr stat values updated
   UFUNCTION()
      void OnStatsUpdated ();
private:
   // To evade endless when:
   // OnStatusUpdated -> CalcWeight -> SetEffect -> OnStatusUpdated
   int32 LastWeightMax = 0;

   //==========================================================================
   //                 Balanacing
   //==========================================================================
protected:
   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Equipment")
      FRuntimeFloatCurve StrToWeightMax;

   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Equipment")
      FRuntimeFloatCurve WeightToEvasion;

   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Equipment")
      FRuntimeFloatCurve WeightToMoveSpeed;

   //==========================================================================
   //                 Events
   //==========================================================================
public:

   UFUNCTION()
      void ReportEquipmentUpdated ();

   UPROPERTY(BlueprintAssignable, Category = "Rade|Equipment")
      FREquipmentMgrEvent OnEquipmentUpdated;

   //==========================================================================
   //                 Save / Load
   //==========================================================================
protected:
   virtual void OnSave (FBufferArchive &SaveData) override;
   virtual void OnLoad (FMemoryReader &LoadData) override;
};

