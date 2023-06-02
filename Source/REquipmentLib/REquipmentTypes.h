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

USTRUCT(BlueprintType)
struct REQUIPMENTLIB_API FRConsumableItemData : public FRActionItemData
{
   GENERATED_BODY()

   FRConsumableItemData ();

   virtual bool Used (AActor* Owner, URInventoryComponent *Inventory) override;

   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TArray<TSubclassOf<URActiveStatusEffect> > ActiveEffects;

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

   FREquipmentData ();

   // Slot to which item will be attached on spawn
   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
      TSubclassOf<UREquipmentSlotComponent> EquipmentSlot;

   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	   TSoftObjectPtr<UStaticMesh> StaticMesh;

   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	   TSoftObjectPtr<USkeletalMesh> SkeletalMesh;

   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
      float CurrentDurability = 100;

   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
      float MaxDurability = 100;

   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
      TArray<FRDamageResistance> Resistence;

   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
      TArray<FRPassiveStatusEffect> PassiveEffects;

   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
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

   UFUNCTION(BlueprintCallable, Category = "Rade|Equipment", Meta = (ExpandEnumAsExecs = "Branches"))
      static void Item_To_ConsumableItem (const FRItemData &src, FRConsumableItemData &ItemData,
                                          ERActionResult &Branches);

   UFUNCTION(BlueprintCallable, Category = "Rade|Equipment", Meta = (ExpandEnumAsExecs = "Branches"))
      static void Item_To_EquipmentItem (const FRItemData &src, FREquipmentData &ItemData,
                                         ERActionResult &Branches);
};

