// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "Engine/DataTable.h"
#include "RUtilLib/RUtilTypes.h"
#include "RUILib/RUIDescription.h"
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

   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(RowType="/Script/RInventoryLib.RItemData"))
      FRItemDataHandle CreateItem;

   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
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

   FRItemData();

   // Must be defined in all subclasses for type checking and casting
   UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
      FString Type;

   UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
      TArray<FString> CastType;

   // --- Base data every item should have

   // Rarity
   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
      ERItemRarity Rarity = ERItemRarity::None;

   // Data table row Unique ID
   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
      FString Name;

   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
      FRUIDescription Description;

   // Number of item instances
   UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
      int32 Count = 1;

   // Max number of item instances per item slot
   // Stackable: MaxCount > 1
   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
      int32 MaxCount = 100;

   // Item Weight of each instance. In gramms.
   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (ClampMin = 0))
      int32 Weight = 0;

   // Selling price
   // 0 -> Can't sell
   UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
      int32 Price = 0;

   // --- Pickup
   //    If both are empty, item can not be dropped.

   // Pickup mesh
   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
      TSoftObjectPtr<UStaticMesh> PickupMesh;

   // Custom pickup class. Will be used if set.
   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
      TSubclassOf<ARItemPickup> Pickup;

   // --- Internal Runtime information

   // Refreshes member variables from internal JsonData

   // Parse JsonData and update current class variables
   virtual bool ReadJSON ();

   // Constructs JSON string of current struct types and assigns it to JsonData
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

   FRActionItemData ();

   // Use interface callback
   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
      TSubclassOf<URItemAction> Action;

   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
      bool DestroyOnAction = false;

   virtual bool ReadJSON  () override;
   virtual bool WriteJSON () override;

   static bool Cast (const FRItemData &src, FRActionItemData &dst);

   virtual bool Used (AActor* Owner, URInventoryComponent *Inventory);
};



// ============================================================================
//          Blueprint Library
// ============================================================================

UCLASS(ClassGroup=(_Rade))
class RINVENTORYLIB_API URItemUtilLibrary : public UBlueprintFunctionLibrary
{
   GENERATED_BODY()
public:

   UFUNCTION(BlueprintCallable, Category = "Rade|Inventory", Meta = (ExpandEnumAsExecs = "Outcome"))
      static void ItemHandle_To_Item (const FRItemDataHandle &src, FRItemData &ItemData,
                                      ERActionResult &Outcome);

   UFUNCTION(BlueprintCallable, Category = "Rade|Inventory", Meta = (ExpandEnumAsExecs = "Outcome"))
      static void Item_To_ActionItem (const FRItemData &src, FRActionItemData &ItemData,
                                      ERActionResult &Outcome);

   UFUNCTION(BlueprintCallable, Category = "Rade|Inventory")
      static bool Item_IsBreakable (const FRItemData &BreakItem, UDataTable* BreakItemTable);

   UFUNCTION(BlueprintCallable, Category = "Rade|Inventory")
      static bool Item_GetBreakList (const FRItemData &BreakItem, UDataTable* BreakItemTable,
                                     TArray<FRItemDataHandle> &ResultItems);


};

