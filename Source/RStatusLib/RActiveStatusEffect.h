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
class RSTATUSLIB_API URActiveStatusEffect : public UActorComponent
{
   GENERATED_BODY()
public:

   URActiveStatusEffect ();

   virtual void GetLifetimeReplicatedProps (TArray<FLifetimeProperty> &OutLifetimeProps) const override;
   virtual void BeginPlay () override;

   virtual void OnComponentCreated () override;
   virtual void OnComponentDestroyed (bool bDestroyingHierarchy) override;

   //==========================================================================
   //                 Functions
   //==========================================================================
public:
   // Effect has been re-applied
   UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Rade|Status")
      void Refresh ();

   // Cancel effect and destroy component.
   UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Rade|Status")
      void Stop ();

   // Get lifetime left
   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Status")
      double GetDurationLeft () const;

   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Status")
      bool GetIsRunning () const;

   //==========================================================================
   //                 Values
   //==========================================================================

   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Status")
      FString UIName;

   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Status")
      float Duration = 5;

   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Status")
      int StackMax = 1;

   UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rade|Status")
      int StackCurrent = 1;

   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Status")
      TArray<FRPassiveStatusEffect> PassiveEffects;

   // Must be set by server
   UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Rade|Status")
      AActor* Causer = nullptr;

   UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rade|Status")
      URStatusMgrComponent* StatusMgr = nullptr;

   //==========================================================================
   //                 Events
   //==========================================================================

   // Effect started
   UPROPERTY(BlueprintAssignable)
      FRActiveStatusEffectEvent OnStart;

   // Effect was re-applied
   UPROPERTY(BlueprintAssignable)
      FRActiveStatusEffectEvent OnRefresh;

   // Effect ended
   UPROPERTY(BlueprintAssignable)
      FRActiveStatusEffectEvent OnEnd;

   // Effect canceled
   UPROPERTY(BlueprintAssignable)
      FRActiveStatusEffectEvent OnCancel;

protected:

   virtual void Apply ();
   virtual void Started ();
   virtual void Ended ();

   FTimerHandle TimerToEnd;

   UPROPERTY()
      bool IsRunning = false;

   UPROPERTY()
      double StartTime = 0;
};


// ============================================================================
//                   Status Effect Library
// ============================================================================

UCLASS(ClassGroup=(_Rade))
class RSTATUSLIB_API URStatusEffectUtilLibrary : public UBlueprintFunctionLibrary
{
   GENERATED_BODY()
public:

   UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Rade|Status")
      static bool SetStatusEffect_Passive (AActor *Target, const FString &Tag, const TArray<FRPassiveStatusEffect> &Effects);

   UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Rade|Status")
      static bool RmStatusEffect_Passive (AActor *Target, const FString &Tag);

   UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Rade|Status")
      static bool ApplyStatusEffect_Active (AActor* Causer, AActor *Target, const TSubclassOf<URActiveStatusEffect> Effect);
};

