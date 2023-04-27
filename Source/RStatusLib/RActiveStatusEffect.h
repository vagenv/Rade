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

   //==========================================================================
   //                 Values
   //==========================================================================

   // Must be set by server
   UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Rade|Status")
      AActor* Causer = nullptr;

protected:

   //UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rade|Status")
   //   URStatusMgrComponent* StatusMgr = nullptr;

   //==========================================================================
   //                 Events
   //==========================================================================
public:
    // Component created
    UPROPERTY(BlueprintAssignable, Category = "Rade|Status")
       FRActiveStatusEffectEvent OnStart;

    // Canceled
    UPROPERTY(BlueprintAssignable, Category = "Rade|Status")
       FRActiveStatusEffectEvent OnStop;

   // Effect was re-applied
   UPROPERTY(BlueprintAssignable, Category = "Rade|Status")
      FRActiveStatusEffectEvent OnRefresh;

    // Destroying component
    UPROPERTY(BlueprintAssignable, Category = "Rade|Status")
       FRActiveStatusEffectEvent OnEnd;

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

