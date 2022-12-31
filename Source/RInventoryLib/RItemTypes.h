// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "Engine/DataTable.h"
#include "RItemTypes.generated.h"

//=============================================================================
//                   Item description
//=============================================================================

class URItem;


// Anim State Type
UENUM(BlueprintType)
enum class FRItemType : uint8
{
   Base        UMETA(DisplayName = "Base"),
   Action      UMETA(DisplayName = "Action"),
   Equipment   UMETA(DisplayName = "Equipment"),
};

USTRUCT(BlueprintType)
struct RINVENTORYLIB_API FRItemRawData
{
   GENERATED_BODY()

   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      FRItemType Type;

   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      FString Data;
};


USTRUCT(BlueprintType)
struct RINVENTORYLIB_API FRItemRow_Base : public FTableRowBase
{
   GENERATED_BODY()
public:
   // --- Must be implemented in all subclasses
   static FString        ToJSON   (const FRItemRow_Base      &item);
   static FRItemRow_Base FromJSON (const FString             &rawData);
   static FRItemRow_Base FromRow  (const FDataTableRowHandle &rowData);

   // --- Data

   // Item Name
   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      FString Name = FString ("Undefined item name");

   // Tooltip or Press E to Use/Equip
   UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (MultiLine = true))
      FString Tooltip = FString ("Undefined item tooltip");

   // Item Icon
   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      TSoftObjectPtr<UTexture2D> Icon;

   // Number of item instances
   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      int32 Count = 1;

   // Max number of item instances per item slot
   // MaxCount > 1 => Stackable
   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      int32 MaxCount = 1;

   // Item Weight of each instance
   UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0.0))
      float Weight = 1;

   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      int32 Cost = 0;
};

USTRUCT(BlueprintType)
struct RINVENTORYLIB_API FRItemRow_Action : public FRItemRow_Base
{
   GENERATED_BODY()

   // --- Must be implemented in all subclasses
   // static FString          ToJSON   (const FRItemRow_Action    &item);
   // static FRItemRow_Action FromJSON (const FString             &rawData);
   // static FRItemRow_Action FromRow  (const FDataTableRowHandle &rowData);

   // --- Data

   UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
      TSubclassOf<URItem> Action;

};

USTRUCT(BlueprintType)
struct RINVENTORYLIB_API FRItemRow_Equipment : public FRItemRow_Action
{
   GENERATED_BODY()

   // --- Must be implemented in all subclasses
   // static FString             ToJSON   (const FRItemRow_Equipment &item);
   // static FRItemRow_Equipment FromJSON (const FString             &rawData);
   // static FRItemRow_Equipment FromRow  (const FDataTableRowHandle &rowData);

   // --- Data
   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      FString WeaponSlot = FString ("Undefined slot");
};



// USTRUCT(Blueprintable, BlueprintType)
// struct RINVENTORYLIB_API FRItemProperty
// {
//    GENERATED_BODY()

//    FRItemProperty (){};
//    FRItemProperty (const FString &Key_, const FString &Value_);

//    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rade|Inventory")
//       FString Key;

//    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rade|Inventory")
//       FString Value;
// };

// // Contains raw data
// USTRUCT(Blueprintable, BlueprintType)
// struct RINVENTORYLIB_API FRItemData
// {
//    GENERATED_BODY()

//    // Main description for UI
//    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rade|Inventory")
//       FRItemDescription Description;

//    // Object that drives the behavior
//    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rade|Inventory")
//       TSubclassOf<URItem> ItemArch;

//    //=============================================================================
//    //          RAW data Management
//    //=============================================================================

//    bool    Has (const FString &Key_) const;
//    FString Get (const FString &Key_) const;
//    void    Set (const FString &Key_, const FString &Value_);


//    // // internal use only
//    // FString Get      (const FString &key) const;
//    // int32   GetINT32 (const FString &key) const;
//    // int64   GetINT64 (const FString &key) const;
//    // //void*   GetPTR   (const FString &key) const;
//    // float   GetFloat (const FString &key) const;

//    // void Set (const FString &key, const FString &value);
//    // void Set (const FString &key, const int32 value);
//    // void Set (const FString &key, const int64 value);
//    // //void Set (const FString &key, const void* value);
//    // void Set (const FString &key, const float value);

//    // // Set all local params from RawData
//    // virtual void FromRAW ();
//    // // Update RawData from local params
//    // virtual void ToRAW ();


//    // Set all local params from JSON string
//    bool    FromJSON (const FString &Data_);

//    // Return all local params as JSON string
//    FString   ToJSON ();


//    UPROPERTY()
//       TArray<FRItemProperty> RawData;

// private:
//    // Raw container that will be used to store data
//    //UPROPERTY()
//    //   TMap<FString, FString> RawData;
// };

