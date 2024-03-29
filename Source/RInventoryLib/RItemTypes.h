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
class  URItemAction;
class  URInventoryComponent;
class  UStaticMesh;
struct FRItemData;

// ============================================================================
//          Handle for ItemData defined in a table
// ============================================================================

USTRUCT(Blueprintable, BlueprintType)
struct RINVENTORYLIB_API FRItemDataHandle
{
   GENERATED_BODY()

   UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(RowType="/Script/RInventoryLib.RItemData"))
      FDataTableRowHandle Arch;

   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      int32 Count = 1;

   // Converts Handle to ItemData;
   bool ToItem (FRItemData &dst) const;
};

// ============================================================================
//          Item Recipe. Mapping Item <=> Array Of Items
// ============================================================================

USTRUCT(Blueprintable, BlueprintType)
struct RINVENTORYLIB_API FRCraftRecipe : public FTableRowBase
{
   GENERATED_BODY()

   UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(RowType="/Script/RInventoryLib.RItemData"))
      FRItemDataHandle CreateItem;

   UPROPERTY(EditAnywhere, BlueprintReadOnly)
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
USTRUCT(Blueprintable, BlueprintType)
struct RINVENTORYLIB_API FRItemData : public FTableRowBase
{
   GENERATED_BODY()

   FRItemData ();

   // Data table row Unique ID
   UPROPERTY()
      FString ID;

   // Must be set in all subclasses for type checking and casting
   UPROPERTY()
      FString Type;

   // Into what types can the 'JsonData' be converted to
   UPROPERTY()
      TArray<FString> CastType;

   // UI Information
   UPROPERTY(EditAnywhere, BlueprintReadOnly)
      FRUIDescription Description;

   // Rarity
   UPROPERTY(EditAnywhere, BlueprintReadOnly)
      ERItemRarity Rarity = ERItemRarity::None;

   // Number of item instances
   UPROPERTY(EditAnywhere, BlueprintReadOnly)
      int32 Count = 1;

   // Max number of item instances per item slot
   // Stackable: MaxCount > 1
   UPROPERTY(EditAnywhere, BlueprintReadOnly)
      int32 MaxCount = 100;

   // Item Weight of each instance. In gramms.
   UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (ClampMin = 0))
      int32 Weight = 0;

   // Selling price
   // 0 -> Can't sell
   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      int32 Price = 0;

   // --- Pickup
   //    If both are empty, default inventory drop class will be used

   // Pickup mesh. Will search and update statis mesh component
   UPROPERTY(EditAnywhere, BlueprintReadOnly)
      TSoftObjectPtr<UStaticMesh> PickupMesh;

   // Custom pickup actor class. Will be used if set.
   UPROPERTY(EditAnywhere, BlueprintReadOnly)
      TSoftClassPtr<AActor> Pickup;

   // --- Internal Runtime information

   // Parse JsonData and update current class variables
   virtual bool ReadJSON ();

   // Constructs JSON string of current struct types and assigns it to JsonData
   // DANGEROUS!!! Only call it on correct subclass version.
   virtual bool WriteJSON ();

   bool IsValid () const;

private:

   // This data will be serialized. Subclasses must write data into here
   UPROPERTY()
      FString JsonData;

public:
   // Get a copy of internal value
   FString GetJSON () const              { return JsonData; };

   // Sets the internal value
   void    SetJSON (const FString &data) { JsonData = data; };

};

// ============================================================================
//          Item with use action
// ============================================================================

USTRUCT(Blueprintable, BlueprintType)
struct RINVENTORYLIB_API FRActionItemData : public FRItemData
{
   GENERATED_BODY()

   FRActionItemData ();

   // Use interface callback
   UPROPERTY(EditAnywhere, BlueprintReadOnly)
      TSoftClassPtr<URItemAction> Action;

   UPROPERTY(EditAnywhere, BlueprintReadOnly)
      bool DestroyOnAction = false;

   virtual bool ReadJSON  () override;
   virtual bool WriteJSON () override;

   static bool CanCast (const FRItemData &src);
   static bool Cast    (const FRItemData &src, FRActionItemData &dst);

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


   UFUNCTION(BlueprintPure, Category = "Rade|Inventory")
      static bool Item_IsValid (const FRItemData& ItemData);

   UFUNCTION(BlueprintPure, Category = "Rade|Inventory")
      static FString Item_GetId (const FRItemData& ItemData);


   UFUNCTION(BlueprintPure, Category = "Rade|Inventory",
             meta=(DisplayName="Equal (FRItemData, FRItemData)", CompactNodeTitle="=="))
      static bool Item_EqualEqual (const FRItemData& A,
                                   const FRItemData& B);

   UFUNCTION(BlueprintPure, Category = "Rade|Inventory",
             meta=(DisplayName="NotEqual (FRItemData, FRItemData)", CompactNodeTitle="!="))
      static bool Item_NotEqual (const FRItemData& A,
                                 const FRItemData& B);


   UFUNCTION(BlueprintPure, Category = "Rade|Inventory",
             meta=(DisplayName="Equal (FRItemData, FRActionItemData)", CompactNodeTitle="=="))
      static bool ActionItem_EqualEqual (const FRItemData& A,
                                             const FRActionItemData& B);

   UFUNCTION(BlueprintPure, Category = "Rade|Inventory",
             meta=(DisplayName="NotEqual (FRItemData, FRActionItemData)", CompactNodeTitle="!="))
      static bool ActionItem_NotEqual (const FRItemData& A,
                                           const FRActionItemData& B);

   /// --- Item casts

   UFUNCTION(BlueprintCallable, Category = "Rade|Inventory", Meta = (ExpandEnumAsExecs = "Outcome"))
      static void ItemHandle_To_Item (const FRItemDataHandle &ItemHandle,
                                      FRItemData             &ItemData,
                                      ERActionResult         &Outcome);

   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Inventory")
      static bool Item_Is_ActionItem (const FRItemData &ItemData);

   UFUNCTION(BlueprintCallable, Category = "Rade|Inventory", Meta = (ExpandEnumAsExecs = "Outcome"))
      static void Item_To_ActionItem (const FRItemData &ItemData,
                                      FRActionItemData &ActionItem,
                                      ERActionResult   &Outcome);


   /// --- Recipe / break operations

   UFUNCTION(BlueprintCallable, Category = "Rade|Inventory")
      static bool Item_GetRecipe (const UDataTable *RecipeTable,
                                  const FRItemData &ItemData,
                                  FRCraftRecipe    &Recipe);

   UFUNCTION(BlueprintCallable, Category = "Rade|Inventory")
      static bool Item_IsBreakable (const FRItemData &BreakItem,
                                    const UDataTable *BreakItemTable);

   UFUNCTION(BlueprintCallable, Category = "Rade|Inventory")
      static bool Item_GetBreakList (const FRItemData         &BreakItem,
                                     const UDataTable         *BreakItemTable,
                                     TArray<FRItemDataHandle> &ResultItems);
};

