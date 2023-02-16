// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "RStatusEffect.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE (FRActiveStatusEffectEvent);

class AActor;

// ============================================================================
//                   Status Effect Scale (FLAT/PERCENT)
// ============================================================================

UENUM(BlueprintType)
enum class ERStatusEffectScale : uint8
{
   None UMETA (DisplayName = "Please select"),

   FLAT    UMETA (DisplayName = "Flat (plus)"),
   PERCENT UMETA (DisplayName = "Percent (multiply)")
};

// ============================================================================
//                   Status Effect Target
// ============================================================================

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

// ============================================================================
//                   Status Effect
// ============================================================================

USTRUCT(Blueprintable, BlueprintType)
struct RSTATUSLIB_API FRPassiveStatusEffect
{
   GENERATED_BODY()

   UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
      ERStatusEffectScale Scale = ERStatusEffectScale::None;

   UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
      ERStatusEffectTarget Target = ERStatusEffectTarget::None;

   UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
      float Value = 0.;

   inline FRPassiveStatusEffect operator + (const FRPassiveStatusEffect &obj) const;
};

USTRUCT(BlueprintType)
struct RSTATUSLIB_API FRPassiveStatusEffectWithTag
{
   GENERATED_BODY()

   // Who or What is applying effect
   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      FString Tag;

   // What value is added
   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      FRPassiveStatusEffect Value;
};


UCLASS(Blueprintable, BlueprintType)
class RSTATUSLIB_API ARActiveStatusEffect : public AActor
{
   GENERATED_BODY()
public:

   ARActiveStatusEffect ();

   // --- Values
   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
      FString UIName;

   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
      float Duration = 5;

   UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
      bool isRunning = false;

   UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
      AActor* Causer = nullptr;

   UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
      AActor* Target = nullptr;


   // --- Events

   UFUNCTION(BlueprintCallable, Category = "Rade|Status")
      virtual void Start (AActor* Causer_, AActor* Target_);

   UFUNCTION(BlueprintImplementableEvent, Category = "Rade")
      void BP_Started ();

   UPROPERTY(BlueprintAssignable)
      FRActiveStatusEffectEvent OnStart;


   UFUNCTION(BlueprintImplementableEvent, Category = "Rade")
      void BP_Ended ();

   UPROPERTY(BlueprintAssignable)
      FRActiveStatusEffectEvent OnEnd;

protected:

   virtual void Started ();
   virtual void Ended ();

   FTimerHandle TimerToEnd;

};

