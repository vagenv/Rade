// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "RPassiveStatusEffect.h"
#include "RActiveStatusEffect.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE (FRActiveStatusEffectEvent);

class AActor;
class URStatusMgrComponent;

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

   virtual void OnComponentCreated () override;
   virtual void OnComponentDestroyed (bool bDestroyingHierarchy) override;

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

   UFUNCTION(BlueprintCallable, Category = "Rade|Status")
      float GetStackScale (int Stack = -1) const;

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

protected:

   // How stacks are scaling passive effect
   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Status")
      FRuntimeFloatCurve StackToScale;

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

