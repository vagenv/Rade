// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "RPassiveStatusEffect.h"
#include "RUtilLib/RUIDescription.h"
#include "Engine/DataTable.h"
#include "RActiveStatusEffect.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE (FRActiveStatusEffectEvent);

class AActor;
class URStatusMgrComponent;
class URActiveStatusEffect;

// ============================================================================
//                   Active Status Effect Info
// ============================================================================

USTRUCT(BlueprintType)
struct RSTATUSLIB_API FRActiveStatusEffectInfo : public FTableRowBase
{
   GENERATED_BODY()
public:

   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
      TSubclassOf<URActiveStatusEffect> EffectClass;

   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
      FRUIDescription Description;

   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
      float Duration = 5;

   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
      TArray<FRPassiveStatusEffect> PassiveEffects;

   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
      TArray<float> StackScaling;

   bool IsValid () const {
      return EffectClass != nullptr && !Description.Label.IsEmpty ();
   };
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
   virtual void EndPlay (const EEndPlayReason::Type EndPlayReason) override;

   //==========================================================================
   //                 Functions
   //==========================================================================
public:
   // Effect has been re-applied
   UFUNCTION(NetMulticast, Unreliable, BlueprintCallable, Category = "Rade|Status")
              void Refresh ();
      virtual void Refresh_Implementation ();

   // Cancel effect and destroy component.
   UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Rade|Status")
      void Stop ();

   // Get lifetime left
   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Status")
      double GetDurationLeft () const;

   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Status")
      bool GetIsRunning () const;


   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Status")
      int GetStackCurrent () const;

   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Status")
      int GetStackMax () const;

   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Status")
      float GetStackScale (int Stack = -1) const;


   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Status")
      FRActiveStatusEffectInfo GetEffectInfo () const;

   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Status")
      FRUIDescription GetDescription () const;

   //==========================================================================
   //                 Values
   //==========================================================================

   // Must be set by server
   UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Rade|Status")
      AActor* Causer = nullptr;

protected:

   UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rade|Status")
      URStatusMgrComponent* StatusMgr = nullptr;

   //==========================================================================
   //                 Events
   //==========================================================================
public:
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

   UFUNCTION()
      virtual void Started ();

   // Called by Started and Refresh
   UFUNCTION()
      virtual void Apply ();

   // On server when timer ended
   UFUNCTION()
      virtual void Timeout ();

   UFUNCTION()
      virtual void Ended ();

   // Value must be defined in table
   UPROPERTY()
      FRActiveStatusEffectInfo EffectInfo;

   UPROPERTY()
      int StackCurrent = 1;

   UPROPERTY()
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

