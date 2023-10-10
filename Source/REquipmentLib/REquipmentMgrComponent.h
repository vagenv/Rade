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
      UREquipmentSlotComponent* GetEquipmentSlot (const TSoftClassPtr<UREquipmentSlotComponent> Type) const;

   //==========================================================================
   //                 Use / Drop override. Check if Item equiped.
   //==========================================================================
   virtual bool UseItem_Index   (int32 ItemIdx) override;
   virtual bool DropItem_Index  (int32 ItemIdx, int32 Count = 0) override;
   virtual bool BreakItem_Index (int32 ItemIdx, const UDataTable* BreakItemTable) override;

   //==========================================================================
   //                 Is Equiped
   //==========================================================================
public:

   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Equipment")
      virtual bool IsEquiped_Index (int32 ItemIdx);

   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Equipment")
      virtual bool IsEquiped_Item (const FRItemData &ItemData);

   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Equipment")
      virtual bool IsEquiped_Equipment (const FREquipmentData &ItemData);

   //==========================================================================
   //                 Equip
   //==========================================================================

   UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Rade|Equipment")
      virtual bool Equip_Index (int32 ItemIdx);

   UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Rade|Equipment")
      virtual bool Equip_Item (const FRItemData &ItemData);

   UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Rade|Equipment")
      virtual bool Equip_Equipment (const FREquipmentData &EquipmentData);
      
   UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Rade|Equipment")
      virtual bool Equip_Slot (UREquipmentSlotComponent *EquipmentSlot,
                               const FREquipmentData &EquipmentData);

   //==========================================================================
   //                 Unquiped
   //==========================================================================

   UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Rade|Equipment")
      virtual bool UnEquip_Index (int32 ItemIdx);

   UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Rade|Equipment")
      virtual bool UnEquip_Item (const FRItemData &ItemData);

   UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Rade|Equipment")
      virtual bool UnEquip_Equipment (const FREquipmentData &ItemData);

   UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Rade|Equipment")
      virtual bool UnEquip_Slot (UREquipmentSlotComponent *EquipmentSlot);

   //==========================================================================
   //                 Server versions of the functions
   //==========================================================================

   // --- Equip
   UFUNCTION(Server, Reliable, BlueprintCallable, BlueprintAuthorityOnly, Category = "Rade|Equipment")
      virtual void Equip_Index_Server (
         UREquipmentMgrComponent *DstEquipment, int32 ItemIdx);
      virtual void Equip_Index_Server_Implementation (
         UREquipmentMgrComponent *DstEquipment, int32 ItemIdx);

   UFUNCTION(Server, Reliable, BlueprintCallable, BlueprintAuthorityOnly, Category = "Rade|Equipment")
      virtual void Equip_Item_Server (
         UREquipmentMgrComponent *DstEquipment, const FRItemData &ItemData);
      virtual void Equip_Item_Server_Implementation (
         UREquipmentMgrComponent *DstEquipment, const FRItemData &ItemData);

   UFUNCTION(Server, Reliable, BlueprintCallable, BlueprintAuthorityOnly, Category = "Rade|Equipment")
      virtual void Equip_Equipment_Server (
         UREquipmentMgrComponent *DstEquipment, const FREquipmentData &EquipmentData);
      virtual void Equip_Equipment_Server_Implementation (
         UREquipmentMgrComponent *DstEquipment, const FREquipmentData &EquipmentData);

   UFUNCTION(Server, Reliable, BlueprintCallable, BlueprintAuthorityOnly, Category = "Rade|Equipment")
      virtual void Equip_Slot_Server (
         UREquipmentMgrComponent  *DstEquipment,
         UREquipmentSlotComponent *EquipmentSlot,
         const FREquipmentData    &EquipmentData);
      virtual void Equip_Slot_Server_Implementation (
         UREquipmentMgrComponent  *DstEquipment, 
         UREquipmentSlotComponent *EquipmentSlot,
         const FREquipmentData    &EquipmentData);

   // --- Unequip
   UFUNCTION(Server, Reliable, BlueprintCallable, BlueprintAuthorityOnly, Category = "Rade|Equipment")
      virtual void UnEquip_Index_Server (
         UREquipmentMgrComponent *DstEquipment, int32 ItemIdx);
      virtual void UnEquip_Index_Server_Implementation (
         UREquipmentMgrComponent *DstEquipment, int32 ItemIdx);

   UFUNCTION(Server, Reliable, BlueprintCallable, BlueprintAuthorityOnly, Category = "Rade|Equipment")
      virtual void UnEquip_Item_Server (
         UREquipmentMgrComponent *DstEquipment, const FRItemData &ItemData);
      virtual void UnEquip_Item_Server_Implementation (
         UREquipmentMgrComponent *DstEquipment, const FRItemData &ItemData);

   UFUNCTION(Server, Reliable, BlueprintCallable, BlueprintAuthorityOnly, Category = "Rade|Equipment")
      virtual void UnEquip_Equipment_Server (
         UREquipmentMgrComponent *DstEquipment, const FREquipmentData &EquipmentData);
      virtual void UnEquip_Equipment_Server_Implementation (
         UREquipmentMgrComponent *DstEquipment, const FREquipmentData &EquipmentData);
   UFUNCTION(Server, Reliable, BlueprintCallable, BlueprintAuthorityOnly, Category = "Rade|Equipment")
      virtual void UnEquip_Slot_Server (
         UREquipmentMgrComponent  *DstEquipment,
         UREquipmentSlotComponent *EquipmentSlot);
      virtual void UnEquip_Slot_Server_Implementation (
         UREquipmentMgrComponent  *DstEquipment, 
         UREquipmentSlotComponent *EquipmentSlot);

   //==========================================================================
   //                 Weight calculation
   //==========================================================================
protected:
   virtual void CalcWeight () override;

   // When status mgr stat values updated
   UFUNCTION()
      void OnStatsUpdated ();
private:
   // To evade endless loop when:
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

