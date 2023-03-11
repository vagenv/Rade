// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "RStatusEffect.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE (FRActiveStatusEffectEvent);

class AActor;
class URStatusMgrComponent;

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
//                   Passive Status Effect
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

// ============================================================================
//                   Active Status Effect
// ============================================================================

UCLASS(Abstract, Blueprintable, BlueprintType, ClassGroup=(_Rade))
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

   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
      float StackMax = 1;

   UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
      float StackCurrent = 1;

   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
      TArray<FRPassiveStatusEffect> PassiveEffects;

   UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
      bool isRunning = false;

   UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
      AActor* Causer = nullptr;

   UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
      AActor* Target = nullptr;

   UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
      URStatusMgrComponent* StatusMgr = nullptr;

   // Must be called right after construction
   bool Apply (AActor* Causer_, AActor* Target_);

   UFUNCTION(BlueprintCallable, Category = "Rade|Status")
      void Cancel ();

   // --- Events

   UFUNCTION(BlueprintImplementableEvent, Category = "Rade|Status")
      void BP_Started ();

   UPROPERTY(BlueprintAssignable)
      FRActiveStatusEffectEvent OnStart;

   UFUNCTION(BlueprintImplementableEvent, Category =  "Rade|Status")
      void BP_Ended ();

   UPROPERTY(BlueprintAssignable)
      FRActiveStatusEffectEvent OnEnd;

   UFUNCTION(BlueprintImplementableEvent, Category =  "Rade|Status")
      void BP_Canceled ();

   UPROPERTY(BlueprintAssignable)
      FRActiveStatusEffectEvent OnCancel;

   // Functions
   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Status")
      double GetDurationLeft () const;

protected:

   virtual void Started ();
   virtual void Ended ();

   FTimerHandle TimerToEnd;

   double Elapse = 0;
};


// ============================================================================
//                   Status Effect Library
// ============================================================================

UCLASS(ClassGroup=(_Rade))
class RSTATUSLIB_API URStatusEffectUtilLibrary : public UBlueprintFunctionLibrary
{
   GENERATED_BODY()
public:

   UFUNCTION(BlueprintCallable, Category = "Rade|Status")
      static bool SetStatusEffect_Passive (AActor *Target, const FString &Tag, const TArray<FRPassiveStatusEffect> &Effects);

   UFUNCTION(BlueprintCallable, Category = "Rade|Status")
      static bool RmStatusEffect_Passive (AActor *Target, const FString &Tag);

   UFUNCTION(BlueprintCallable, Category = "Rade|Status")
      static bool ApplyStatusEffect_Active (AActor* Causer, AActor *Target, const TSubclassOf<ARActiveStatusEffect> Effect);
};

