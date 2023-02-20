// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "RInventoryLib/RItemTypes.h"
#include "RStatusLib/RDamageType.h"
#include "RStatusLib/RStatusTypes.h"
#include "RStatusLib/RStatusEffect.h"
#include "REquipmentTypes.generated.h"

class AActor;
class URInventoryComponent;
class UREquipmentSlotComponent;
class UREquipmentMgrComponent;
class UWorld;

// ============================================================================
//                Consumable Items
// ============================================================================

USTRUCT(BlueprintType)
struct REQUIPMENTLIB_API FRConsumableItemData : public FRActionItemData
{
   GENERATED_BODY()

   virtual bool Used (AActor* Owner, URInventoryComponent *Inventory) override;

   UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<TSubclassOf<ARActiveStatusEffect> > ActiveEffects;

   static bool Cast (const FRItemData &src, FRConsumableItemData &dst);


   virtual bool ReadJSON () override;
   virtual bool WriteJSON () override;
};

// ============================================================================
//          Equipment
// ============================================================================

USTRUCT(BlueprintType)
struct REQUIPMENTLIB_API FREquipmentData : public FRActionItemData
{
   GENERATED_BODY()

   // Slot to which item will be attached on spawn
   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      TSubclassOf<UREquipmentSlotComponent> EquipmentSlot;

   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      float CurrentDurability = 100;

   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      float MaxDurability = 100;

   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      TArray<FRResistanceStat> Resistence;

   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      TArray<FRPassiveStatusEffect> PassiveEffects;

   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      FRCoreStats RequiredStats;

   static bool Cast (const FRItemData &src, FREquipmentData &dst);

   virtual bool ReadJSON () override;
   virtual bool WriteJSON () override;
};

UCLASS()
class REQUIPMENTLIB_API UREquipmentUtilLibrary : public UBlueprintFunctionLibrary
{
   GENERATED_BODY()
public:

   UFUNCTION(BlueprintCallable, Category = "Rade|Character", Meta = (ExpandEnumAsExecs = "Branches"))
      static void Item_To_ConsumableItem (const FRItemData &src, FRConsumableItemData &ItemData,
                                          ERActionResult &Branches);

   UFUNCTION(BlueprintCallable, Category = "Rade|Character", Meta = (ExpandEnumAsExecs = "Branches"))
      static void Item_To_EquipmentItem (const FRItemData &src, FREquipmentData &ItemData,
                                         ERActionResult &Branches);
};

