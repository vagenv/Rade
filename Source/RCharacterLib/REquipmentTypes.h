// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "RInventoryLib/RItemTypes.h"
#include "RStatusTypes.h"
#include "RDamageType.h"
#include "REquipmentTypes.generated.h"

class AActor;
class UREquipmentSlotComponent;
class UREquipmentMgrComponent;
class UWorld;

// ============================================================================
//          Move to RCharacter ?
// ============================================================================

USTRUCT(BlueprintType)
struct RCHARACTERLIB_API FRConsumableItemData : public FRActionItemData
{
   GENERATED_BODY()

   // 0 = Single instance effect
   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      float DurationSec = 0;

   // ----
   // List of applied effects.
   // ----

   static bool Cast (const FRItemData &src, FRConsumableItemData &dst);
};


USTRUCT(BlueprintType)
struct RCHARACTERLIB_API FREquipmentData : public FRActionItemData
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
      FRCharacterStats Stats;

   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      FRCharacterStats RequiredStats;

   static bool Cast (const FRItemData &src, FREquipmentData &dst);
};

UCLASS()
class RCHARACTERLIB_API UREquipmentUtilLibrary : public UBlueprintFunctionLibrary
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

