// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "RStatusTypes.generated.h"

USTRUCT(BlueprintType)
struct RSTATUSLIB_API FRCharacterStats
{
   GENERATED_BODY()

   FRCharacterStats ();
   FRCharacterStats (float Value);

   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      float Strength = 0;

   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      float Agility = 0;

   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      float Intelligence = 0;

   // Are any values set
   bool Empty () const;

   // All values are higher than
   inline bool MoreThan (const FRCharacterStats &obj) const;

   inline FRCharacterStats operator + (const FRCharacterStats &obj) const;
   inline FRCharacterStats operator - (const FRCharacterStats &obj) const;
};

USTRUCT(BlueprintType)
struct RSTATUSLIB_API FRExtraStat
{
   GENERATED_BODY()

   // Who or What is applying stat
   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      FString Tag;

   // What value is added
   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      FRCharacterStats Stat;
};

USTRUCT(BlueprintType)
struct RSTATUSLIB_API FRStatusValue
{
   GENERATED_BODY()

   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      float Current = 50;

   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      float Max = 100;

   // Per second
   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      float Regen = 0;

   inline void Tick (float DeltaTime);

   friend FArchive& operator<< (FArchive& Ar, FRStatusValue &Value);
};

UCLASS()
class RSTATUSLIB_API URStatusUtilLibrary : public UBlueprintFunctionLibrary
{
   GENERATED_BODY()
public:

   // UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Status",
   //    meta = (DisplayName = "<", CompactNodeTitle = "<"))
   //    static bool FRCharacterStats_Less (const FRCharacterStats &a, const FRCharacterStats &b) {
   //       return (a < b);
   //    }
};

