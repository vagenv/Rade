// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "RItemTypes.generated.h"

// ----------------------------------------------------------------------------
//                   Item description
// ----------------------------------------------------------------------------

class URItem;

// Used for UI
USTRUCT(BlueprintType)
struct RINVENTORYLIB_API FRItemDescription
{
public:
   GENERATED_BODY()

   // Item Name
   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rade|Inventory")
      FString Name = FString ("Undefined item name");;

   // Item Icon
   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rade|Inventory")
      UTexture2D* Icon = nullptr;
      //TSoftObjectPtr<UTexture2D> Icon;

   // Number of item instances
   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rade|Inventory")
      int32 Count = 1;

   // Max number of item instances per item slot
   // MaxCount > 1 => Stackable
   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rade|Inventory")
      int32 MaxCount = 1;

   // Item Weight of each instance
   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rade|Inventory", Meta = (ClampMin = 0.0))
      float Weight = 1;

   // Tooltip or Press E to Use/Equip
   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rade|Inventory", Meta = (MultiLine = true))
      FString Tooltip = FString ("Undefined item tooltip");

   bool operator == (const FRItemDescription& Other) const {
      return Name == Other.Name;
   }
};

USTRUCT(BlueprintType)
struct RINVENTORYLIB_API FRItemProperty
{
public:
   GENERATED_BODY()

   FRItemProperty (){};
   FRItemProperty (const FString &Key_, const FString &Value_);

   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rade|Inventory")
      FString Key;

   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rade|Inventory")
      FString Value;
};

// Contains raw data
USTRUCT(BlueprintType)
struct RINVENTORYLIB_API FRItemData
{
public:
   GENERATED_BODY()

   FRItemData (){};
   virtual ~FRItemData (){};

   // Main description for UI
   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rade|Inventory")
      FRItemDescription Description;

   // Object that drives the behavior
   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rade|Inventory")
      TSubclassOf<URItem> ItemArch;

   // -------------------------------------------------------------------------
   //          RAW data Management
   // -------------------------------------------------------------------------

   bool    Has (const FString &Key_) const;
   FString Get (const FString &Key_) const;
   void    Set (const FString &Key_, const FString &Value_);

   /*
   // internal use only
   FString Get      (const FString &key) const;
   int32   GetINT32 (const FString &key) const;
   int64   GetINT64 (const FString &key) const;
   //void*   GetPTR   (const FString &key) const;
   float   GetFloat (const FString &key) const;

   void Set (const FString &key, const FString &value);
   void Set (const FString &key, const int32 value);
   void Set (const FString &key, const int64 value);
   //void Set (const FString &key, const void* value);
   void Set (const FString &key, const float value);

   // Set all local params from RawData
   virtual void FromRAW ();
   // Update RawData from local params
   virtual void ToRAW ();
   */

   // Set all local params from JSON string
   bool    FromJSON (const FString &Data_);

   // Return all local params as JSON string
   FString   ToJSON ();


   UPROPERTY()
      TArray<FRItemProperty> RawData;

private:
   // Raw container that will be used to store data
   //UPROPERTY()
   //   TMap<FString, FString> RawData;
};