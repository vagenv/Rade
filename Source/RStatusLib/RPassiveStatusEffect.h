// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "RPassiveStatusEffect.generated.h"


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
      ERStatusEffectTarget EffectTarget = ERStatusEffectTarget::None;

   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
      float Flat = 0;

   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
      float Percent = 0;
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


// ============================================================================
//                   Status Effect Library
// ============================================================================

UCLASS(ClassGroup=(_Rade))
class RSTATUSLIB_API URPassiveStatusEffectUtilLibrary : public UBlueprintFunctionLibrary
{
   GENERATED_BODY()
public:

   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Status")
      static TArray<FRPassiveStatusEffect> MergeEffects (const TArray<FRPassiveStatusEffectWithTag>& Effects);

   // UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Rade|Status")
   //    static bool SetStatusEffect_Passive (AActor *Target, const FString &Tag, const TArray<FRPassiveStatusEffect> &Effects);

   // UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Rade|Status")
   //    static bool RmStatusEffect_Passive (AActor *Target, const FString &Tag);
};

