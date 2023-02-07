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
      float Dexterity = 0;

   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      float Consitution = 0;

   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      float Intelligence = 0;

   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      float Wisdom = 0;

   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      float Charisma = 0;

   FRCharacterStats operator + (FRCharacterStats const &obj) {
      FRCharacterStats res;
      res.Strength     = Strength     + obj.Strength    ;
      res.Dexterity    = Dexterity    + obj.Dexterity   ;
      res.Consitution  = Consitution  + obj.Consitution ;
      res.Intelligence = Intelligence + obj.Intelligence;
      res.Wisdom       = Wisdom       + obj.Wisdom      ;
      res.Charisma     = Charisma     + obj.Charisma    ;
      return res;
   }
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

   friend FArchive& operator<< (FArchive& Ar, FRStatusValue &Value);
};

