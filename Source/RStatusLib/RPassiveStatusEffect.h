// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "RPassiveStatusEffect.generated.h"

// ============================================================================
//                   Status Effect Scale (FLAT/PERCENT)
// ============================================================================

UENUM(Blueprintable, BlueprintType)
enum class ERStatusEffectScale : uint8
{
   None UMETA (DisplayName = "Please select"),

   FLAT    UMETA (DisplayName = "Flat (plus)"),
   PERCENT UMETA (DisplayName = "Percent (multiply)")
};

// ============================================================================
//                   Status Effect Target
// ============================================================================

UENUM(Blueprintable, BlueprintType)
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

// ============================================================================
//                   Passive Status Effect
// ============================================================================

USTRUCT(Blueprintable, BlueprintType)
struct RSTATUSLIB_API FRPassiveStatusEffect
{
   GENERATED_BODY()

   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
      ERStatusEffectScale Scale = ERStatusEffectScale::None;

   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
      ERStatusEffectTarget Target = ERStatusEffectTarget::None;

   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
      float Value = 0.;

   inline FRPassiveStatusEffect operator + (const FRPassiveStatusEffect &obj) const;
};

USTRUCT(BlueprintType)
struct RSTATUSLIB_API FRPassiveStatusEffectWithTag
{
   GENERATED_BODY()

   // Who or What is applying effect
   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
      FString Tag;

   // What value is added
   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
      FRPassiveStatusEffect Value;
};

