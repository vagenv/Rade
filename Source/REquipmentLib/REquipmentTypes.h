// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "RInventoryLib/RItemTypes.h"
#include "RDamageLib/RDamageTypes.h"
#include "RStatusLib/RStatusTypes.h"
#include "RStatusLib/RPassiveStatusEffect.h"
#include "REquipmentTypes.generated.h"

class UStaticMesh;
class USkeletalMesh;
class AActor;
class URInventoryComponent;
class UREquipmentSlotComponent;
class UREquipmentMgrComponent;
class URActiveStatusEffect;
class UWorld;

// ============================================================================
//                Consumable Items
// ============================================================================

USTRUCT(Blueprintable, BlueprintType)
struct REQUIPMENTLIB_API FRConsumableItemData : public FRActionItemData
{
   GENERATED_BODY()

   FRConsumableItemData ();

   virtual bool Used (AActor* Owner, URInventoryComponent *Inventory) override;

   UPROPERTY(EditAnywhere, BlueprintReadOnly)
      TArray<TSoftClassPtr<URActiveStatusEffect> > ActiveEffects;

   static bool CanCast (const FRItemData &src);
   static bool Cast    (const FRItemData &src, FRConsumableItemData &dst);

   virtual bool ReadJSON () override;
   virtual bool WriteJSON () override;
};

// ============================================================================
//          Equipment
// ============================================================================

USTRUCT(Blueprintable, BlueprintType)
struct REQUIPMENTLIB_API FREquipmentData : public FRActionItemData
{
   GENERATED_BODY()

   FREquipmentData ();

   // Slot to which item will be attached on spawn
   UPROPERTY(EditAnywhere, BlueprintReadOnly)
      TSoftClassPtr<UREquipmentSlotComponent> EquipmentSlot;

   UPROPERTY(EditAnywhere, BlueprintReadOnly)
      TSoftObjectPtr<UStaticMesh> StaticMesh;

   UPROPERTY(EditAnywhere, BlueprintReadOnly)
      TSoftObjectPtr<USkeletalMesh> SkeletalMesh;

   UPROPERTY(EditAnywhere, BlueprintReadOnly)
      float CurrentDurability = 100;

   UPROPERTY(EditAnywhere, BlueprintReadOnly)
      float MaxDurability = 100;

   UPROPERTY(EditAnywhere, BlueprintReadOnly)
      TArray<FRDamageResistance> Resistence;

   UPROPERTY(EditAnywhere, BlueprintReadOnly)
      TArray<FRPassiveStatusEffect> PassiveEffects;

   UPROPERTY(EditAnywhere, BlueprintReadOnly)
      FRCoreStats RequiredStats;

   static bool CanCast (const FRItemData &src);
   static bool Cast    (const FRItemData &src, FREquipmentData &dst);

   virtual bool ReadJSON () override;
   virtual bool WriteJSON () override;
};

UCLASS()
class REQUIPMENTLIB_API UREquipmentUtilLibrary : public UBlueprintFunctionLibrary
{
   GENERATED_BODY()
public:

   UFUNCTION(BlueprintPure, Category = "Rade|Equipment",
             meta=(DisplayName="Equal (FRItemData, FRConsumableItemData)", CompactNodeTitle="=="))
      static bool ConsumableItem_EqualEqual (const FRItemData& A,
                                             const FRConsumableItemData& B);

   UFUNCTION(BlueprintPure, Category = "Rade|Equipment",
             meta=(DisplayName="NotEqual (FRItemData, FRConsumableItemData)", CompactNodeTitle="!="))
      static bool ConsumableItem_NotEqual (const FRItemData& A,
                                           const FRConsumableItemData& B);

   UFUNCTION(BlueprintPure, Category = "Rade|Equipment",
             meta=(DisplayName="Equal (FRItemData, FREquipmentData)", CompactNodeTitle="=="))
      static bool Equipment_EqualEqual (const FRItemData& A,
                                        const FREquipmentData& B);

   UFUNCTION(BlueprintPure, Category = "Rade|Equipment",
             meta=(DisplayName="NotEqual (FRItemData, FREquipmentData)", CompactNodeTitle="!="))
      static bool Equipment_NotEqual (const FRItemData& A,
                                      const FREquipmentData& B);




   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Equipment")
      static bool Item_Is_ConsumableItem (const FRItemData &ItemData);

   UFUNCTION(BlueprintCallable, Category = "Rade|Equipment", Meta = (ExpandEnumAsExecs = "Branches"))
      static void Item_To_ConsumableItem (const FRItemData     &ItemData,
                                          FRConsumableItemData &ConsumableItem,
                                          ERActionResult       &Branches);

   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Equipment")
      static bool Item_Is_EquipmentItem (const FRItemData &ItemData);

   UFUNCTION(BlueprintCallable, Category = "Rade|Equipment", Meta = (ExpandEnumAsExecs = "Branches"))
      static void Item_To_EquipmentItem (const FRItemData &ItemData,
                                         FREquipmentData  &EquipmentData,
                                         ERActionResult   &Branches);
};

