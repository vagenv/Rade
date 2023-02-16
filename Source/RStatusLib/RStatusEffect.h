// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "RStatusEffect.generated.h"

UENUM(BlueprintType)
enum class ERStatusEffectScale : uint8
{
   None UMETA (DisplayName = "Please select"),

   FLAT    UMETA (DisplayName = "Flat (plus)"),
   PERCENT UMETA (DisplayName = "Percent (multiply)")
};

UENUM(BlueprintType)
enum class ERStatusEffectTarget : uint8
{
   None UMETA (DisplayName = "Please select"),

   // --- Attribute
   STR UMETA (DisplayName = "STR Attribute"),
   AGI UMETA (DisplayName = "AGI Attribute"),
   INT UMETA (DisplayName = "INT Attribute"),

   // --- Status
   HealthMax    UMETA (DisplayName = "Maximum Health"),
   HealthRegen  UMETA (DisplayName = "Health Regeneration"),
   StaminaMax   UMETA (DisplayName = "Maximum Stamina"),
   StaminaRegen UMETA (DisplayName = "Stamina Regeneration"),
   ManaMax      UMETA (DisplayName = "Maximum Mana"),
   ManaRegen    UMETA (DisplayName = "Mana Regeneration"),

   // --- World
   Evasion     UMETA (DisplayName = "Evasion Chance"),
   Critical    UMETA (DisplayName = "Critical Chance"),
   MoveSpeed   UMETA (DisplayName = "Move Speed"),
   AttackSpeed UMETA (DisplayName = "Attack Speed"),
   AttackPower UMETA (DisplayName = "Attack Power")
};

USTRUCT(Blueprintable, BlueprintType)
struct RSTATUSLIB_API FRStatusEffect
{
   GENERATED_BODY()

   UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
      ERStatusEffectScale Scale = ERStatusEffectScale::None;

   UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
      ERStatusEffectTarget Target = ERStatusEffectTarget::None;

   UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
      float Value = 0.;

   inline FRStatusEffect operator + (const FRStatusEffect &obj) const;
};

USTRUCT(BlueprintType)
struct RSTATUSLIB_API FRStatusEffectWithTag
{
   GENERATED_BODY()

   // Who or What is applying effect
   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      FString Tag;

   // What value is added
   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      FRStatusEffect Value;
};

