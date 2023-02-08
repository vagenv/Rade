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

   // Are any values set
   bool Empty () const {
      return (
            Strength     == 0.
         && Dexterity    == 0.
         && Consitution  == 0.
         && Intelligence == 0.
         && Wisdom       == 0.
         && Charisma     == 0.
      );
   }

   inline FRCharacterStats operator + (const FRCharacterStats &obj) const {
      FRCharacterStats res;
      res.Strength     = Strength     + obj.Strength    ;
      res.Dexterity    = Dexterity    + obj.Dexterity   ;
      res.Consitution  = Consitution  + obj.Consitution ;
      res.Intelligence = Intelligence + obj.Intelligence;
      res.Wisdom       = Wisdom       + obj.Wisdom      ;
      res.Charisma     = Charisma     + obj.Charisma    ;
      return res;
   }

   inline FRCharacterStats operator - (const FRCharacterStats &obj) const {
      FRCharacterStats res;
      res.Strength     = Strength     - obj.Strength    ;
      res.Dexterity    = Dexterity    - obj.Dexterity   ;
      res.Consitution  = Consitution  - obj.Consitution ;
      res.Intelligence = Intelligence - obj.Intelligence;
      res.Wisdom       = Wisdom       - obj.Wisdom      ;
      res.Charisma     = Charisma     - obj.Charisma    ;
      return res;
   }

   inline bool MoreThan (const FRCharacterStats &obj) const {
      return (
            Strength     >= obj.Strength
         && Dexterity    >= obj.Dexterity
         && Consitution  >= obj.Consitution
         && Intelligence >= obj.Intelligence
         && Wisdom       >= obj.Wisdom
         && Charisma     >= obj.Charisma
      );
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

