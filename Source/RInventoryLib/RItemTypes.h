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

// The class item will be cast to
UENUM(BlueprintType)
enum class FRItemType : uint8
{
   Base        UMETA(DisplayName = "Base"),
   Action      UMETA(DisplayName = "Action"),
   Equipment   UMETA(DisplayName = "Equipment"),
};

// Rarity of item
UENUM(BlueprintType)
enum class FRItemRarity : uint8
{
   // --- Items
   POOR       UMETA(DisplayName = "POOR"),
   COMMON     UMETA(DisplayName = "COMMON"),
   UNCOMMON   UMETA(DisplayName = "UNCOMMON"),
   RARE       UMETA(DisplayName = "RARE"),
   EPIC       UMETA(DisplayName = "EPIC"),
   LEGENDARY  UMETA(DisplayName = "LEGENDARY"),
   DIVINE     UMETA(DisplayName = "DIVINE"),

   // --- Custom
   SPECIAL    UMETA(DisplayName = "SPECIAL"),

   // --- Quest
   MAINQUEST  UMETA(DisplayName = "MAIN QUEST"),
   QUEST      UMETA(DisplayName = "QUEST")
};

// Minimal data contained in row
USTRUCT(BlueprintType)
struct RINVENTORYLIB_API FRItemData : public FTableRowBase
{
   GENERATED_BODY()
public:

   static FRItemData FromJSON (const FString             &jsonString);
   static FRItemData FromRow  (const FDataTableRowHandle &rowHandle);
   FString ToJSON () const;


   // --- Base data every item should have

   // Rarity
   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rade|Inventory")
      FRItemRarity Rarity;

   // Item Name
   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rade|Inventory")
      FString Name = FString ("Undefined item name");

   // Tooltip or Press E to Use/Equip
   UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (MultiLine = true), Category = "Rade|Inventory")
      FString Tooltip = FString ("Undefined item tooltip");

   // Item Icon
   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rade|Inventory")
      TSoftObjectPtr<UTexture2D> Icon;

   // Number of item instances
   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rade|Inventory")
      int32 Count = 1;

   // Max number of item instances per item slot
   // MaxCount > 1 => Stackable
   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rade|Inventory")
      int32 MaxCount = 1;

   // Item Weight of each instance
   UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0.0), Category = "Rade|Inventory")
      float Weight = 1;

   // Selling price
   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rade|Inventory")
      int32 Cost = 0;

   // --- Interactable items

   // Use interface callback
   UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Rade|Inventory")
      TSubclassOf<URItemAction> Action;

   // --- Pickup

   // Pickup mesh
   UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Rade|Inventory")
      TObjectPtr<UStaticMesh> PickupMesh;

   // Custom pickup class. Will be used if set.
   UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Rade|Inventory")
      TSubclassOf<ARItemPickup> Pickup;


   // --- Equipment

   // Socket to which item will be attached on spawn
   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rade|Inventory")
      FString AttachSocket;

private:

   // For subclass runtime data serialization
   UPROPERTY()
      FString RuntimeData;
};

