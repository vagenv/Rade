// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "Engine/DataTable.h"
#include "RItemTypes.generated.h"

//=============================================================================
//                   Item description
//=============================================================================

class ARItemPickup;
class URItemAction;
class UStaticMesh;

// Rarity of item
UENUM(BlueprintType)
enum class FRItemRarity : uint8
{
   F    UMETA(DisplayName =    "F Grade"),
   E    UMETA(DisplayName =    "E Grade"),
   D    UMETA(DisplayName =    "D Grade"),
   C    UMETA(DisplayName =    "C Grade"),
   B    UMETA(DisplayName =    "B Grade"),
   BB   UMETA(DisplayName =   "BB Grade"),
   A    UMETA(DisplayName =    "A Grade"),
   AA   UMETA(DisplayName =   "AA Grade"),
   AAA  UMETA(DisplayName =  "AAA Grade"),
   S    UMETA(DisplayName =    "S Grade"),
   SS   UMETA(DisplayName =   "SS Grade"),
   SSS  UMETA(DisplayName =  "SSS Grade"),
   SSSS UMETA(DisplayName = "SSSS Grade"),

   MAINQUEST UMETA(DisplayName = "Main Quest Item"),
   QUEST     UMETA(DisplayName = "Quest Item"),

   None      UMETA(DisplayName = "Please set it")
};

// Minimal data contained in row
USTRUCT(BlueprintType)
struct RINVENTORYLIB_API FRItemData : public FTableRowBase
{
   GENERATED_BODY()

   static bool FromJSON (const FString             &src, FRItemData &dst);
   static bool FromRow  (const FDataTableRowHandle &src, FRItemData &dst);
   static bool ToJSON   (const FRItemData          &src, FString    &dst);

   // --- Base data every item should have

   // Rarity
   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      FRItemRarity Rarity = FRItemRarity::None;

   // Item Name
   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      FString Name = FString ("Undefined item name");

   // Tooltip or Press E to Use/Equip
   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      FString Tooltip = FString ("Undefined item tooltip");

   // Item Icon
   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      TSoftObjectPtr<UTexture2D> Icon;

   // Number of item instances
   UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
      int32 Count = 1;

   // Max number of item instances per item slot
   // MaxCount > 1 => Stackable
   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      int32 MaxCount = 1;

   // Item Weight of each instance
   UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0.0))
      float Weight = 1;

   // Selling price
   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      int32 Cost = 0;

   // --- Interactable items

   // Use interface callback
   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      TSubclassOf<URItemAction> Action;

   // --- Pickup

   // Pickup mesh
   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      TObjectPtr<UStaticMesh> PickupMesh;

   // Custom pickup class. Will be used if set.
   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      TSubclassOf<ARItemPickup> Pickup;

protected:



   // For subclass runtime data serialization
   UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
      FString RuntimeData;
};

USTRUCT(BlueprintType)
struct RINVENTORYLIB_API FREquipmentData : public FRItemData
{
   GENERATED_BODY()

   // Socket to which item will be attached on spawn
   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      FString AttachSocket;

   // --- Defence and attack stats
};

USTRUCT(BlueprintType)
struct RINVENTORYLIB_API FRDefaultItem
{
   GENERATED_BODY()
   UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(RowType=RItemData))
      FDataTableRowHandle Arch;

   UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
      int32 Count = 1;
};

USTRUCT(BlueprintType)
struct RINVENTORYLIB_API FRCraftRecipe : public FTableRowBase
{
   GENERATED_BODY()

   UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(RowType=RItemData))
      FDataTableRowHandle CreateItem;

   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      TArray<FRDefaultItem> RequiredItems;
};


