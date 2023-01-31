// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "RStatusTypes.generated.h"

USTRUCT(BlueprintType)
struct RCHARACTERLIB_API FRCharacterStats
{
   GENERATED_BODY()

   FRCharacterStats ();
   FRCharacterStats (float Value);

   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      float Strength = 0;

   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      float Dexterity = 0;

   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      float Consitution = 0;

   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      float Intelligence = 0;

   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      float Wisdom = 0;

   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      float Charisma = 0;
};

USTRUCT(BlueprintType)
struct RCHARACTERLIB_API FRStatusValue
{
   GENERATED_BODY()

   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      float Current = 50;

   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      float Max = 100;

   // Per second
   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      float Regen = 0;

   friend FArchive& operator<< (FArchive& Ar, FRStatusValue &Value);
};

