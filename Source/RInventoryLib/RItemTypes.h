// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "Engine/DataTable.h"
#include "RItemTypes.generated.h"

//=============================================================================
//                   Item description
//=============================================================================

class  AActor;
class  ARItemPickup;
class  URItemAction;
class  URInventoryComponent;
class  UStaticMesh;
struct FRItemDataHandle;
struct FRItemData;

// ============================================================================
//          Handle for ItemData defined in a table
// ============================================================================

USTRUCT(BlueprintType)
struct RINVENTORYLIB_API FRItemDataHandle
{
   GENERATED_BODY()

   UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(RowType="/Script/RInventoryLib.RItemData"))
      FDataTableRowHandle Arch;

   UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
      int32 Count = 1;

   // Converts Handle to ItemData;
   bool ToItem (FRItemData &dst) const;
};

// ============================================================================
//          Item Recipe. Mapping Item <=> Array Of Items
// ============================================================================

USTRUCT(BlueprintType)
struct RINVENTORYLIB_API FRCraftRecipe : public FTableRowBase
{
   GENERATED_BODY()

   UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(RowType="/Script/RInventoryLib.RItemData"))
      FRItemDataHandle CreateItem;

   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      TArray<FRItemDataHandle> RequiredItems;
};

// ============================================================================
//          Rarity of Item
// ============================================================================

UENUM(BlueprintType)
enum class ERItemRarity : uint8
{
   F         UMETA (DisplayName =    "F Grade"),
   E         UMETA (DisplayName =    "E Grade"),
   D         UMETA (DisplayName =    "D Grade"),
   C         UMETA (DisplayName =    "C Grade"),
   B         UMETA (DisplayName =    "B Grade"),
   BB        UMETA (DisplayName =   "BB Grade"),
   A         UMETA (DisplayName =    "A Grade"),
   AA        UMETA (DisplayName =   "AA Grade"),
   AAA       UMETA (DisplayName =  "AAA Grade"),
   S         UMETA (DisplayName =    "S Grade"),
   SS        UMETA (DisplayName =   "SS Grade"),
   SSS       UMETA (DisplayName =  "SSS Grade"),
   SSSS      UMETA (DisplayName = "SSSS Grade"),

   MAINQUEST UMETA (DisplayName = "Main Quest Item"),
   QUEST     UMETA (DisplayName =      "Quest Item"),

   None      UMETA (DisplayName = "Please set it")
};

// ============================================================================
//          Minimal data for Item Descrption
// ============================================================================
USTRUCT(BlueprintType)
struct RINVENTORYLIB_API FRItemData : public FTableRowBase
{
   GENERATED_BODY()

   // --- Base data every item should have

   // Rarity
   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      ERItemRarity Rarity = ERItemRarity::None;

   // Item Name
   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      FString Name = FString ("Please set item name");

   // Tooltip or Press E to Use/Equip
   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      FString Tooltip = FString ("Please set item tooltip");

   // Item Icon
   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      TSoftObjectPtr<UTexture2D> Icon;

   // Number of item instances
   UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
      int32 Count = 1;

   // Max number of item instances per item slot
   // Stackable: MaxCount > 1
   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      int32 MaxCount = 100;

   // Item Weight of each instance
   UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0.0))
      float Weight = 1;

   // Selling price
   // 0 -> Can't sell
   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      int32 Price = 0;

   // --- Pickup
   //    If both are empty, item can not be dropped.

   // Pickup mesh
   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      TObjectPtr<UStaticMesh> PickupMesh;

   // Custom pickup class. Will be used if set.
   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      TSubclassOf<ARItemPickup> Pickup;


   // --- Internal Runtime information

   // Refreshes member variables from internal JsonData

   // Parse JsonData and update current class variables
   virtual bool ReadJSON ();

   // Constructs JSON string of current structt and assigns it to JsonData
   // DANGEROUS!!! Only call it on correct subclass version.
   virtual bool WriteJSON ();

   // Get a copy of internal value
   FString GetJSON () const              { return JsonData; };

   // Sets the internal value
   void    SetJSON (const FString &data) { JsonData = data; };

protected:

   // This data will be serialized. Subclasses must write data into here
   UPROPERTY()
      FString JsonData;
};

// ============================================================================
//          Item with use action
// ============================================================================

USTRUCT(BlueprintType)
struct RINVENTORYLIB_API FRActionItemData : public FRItemData
{
   GENERATED_BODY()

   // Use interface callback
   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      TSubclassOf<URItemAction> Action;

   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      bool DestroyOnAction = false;

   virtual bool ReadJSON  () override;
   virtual bool WriteJSON () override;

   static bool Cast (const FRItemData &src, FRActionItemData &dst);

   virtual bool Used (AActor* Owner, URInventoryComponent *Inventory);
};



// ============================================================================
//          Blueprint Library
// ============================================================================

UENUM(BlueprintType)
enum class ERActionResult : uint8
{
   Success UMETA (DisplayName = "Success"),
   Failure UMETA (DisplayName = "Failure")
};

UCLASS()
class RINVENTORYLIB_API URItemUtilLibrary : public UBlueprintFunctionLibrary
{
   GENERATED_BODY()
public:

   UFUNCTION(BlueprintCallable, Category = "Rade|Inventory", Meta = (ExpandEnumAsExecs = "Branches"))
      static void ItemHandle_To_Item (const FRItemDataHandle &src, FRItemData &ItemData,
                                      ERActionResult &Branches);

   UFUNCTION(BlueprintCallable, Category = "Rade|Inventory", Meta = (ExpandEnumAsExecs = "Branches"))
      static void Item_To_ActionItem (const FRItemData &src, FRActionItemData &ItemData,
                                      ERActionResult &Branches);
};

