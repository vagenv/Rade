// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "RStatusTypes.h"
#include "RStatusEffect.h"
#include "RDamageType.h"
#include "RStatusMgrComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE (FRStatusMgrEvent);

class UCharacterMovementComponent;
class URInventoryComponent;

// Status Manager Component.
UCLASS(Blueprintable, BlueprintType)
class RSTATUSLIB_API URStatusMgrComponent : public UActorComponent
{
   GENERATED_BODY()
public:

   // Base events
   URStatusMgrComponent ();
   virtual void GetLifetimeReplicatedProps (TArray<FLifetimeProperty> &OutLifetimeProps) const override;
   virtual void BeginPlay () override;
   virtual void EndPlay (const EEndPlayReason::Type EndPlayReason) override;
   virtual void TickComponent (float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

   // Called when variables replicated
   UFUNCTION()
      void OnRep_Status ();

   UFUNCTION()
      void RecalcStatus ();

   UFUNCTION()
      void RecalcCoreStats ();

   UFUNCTION()
      void RecalcSubStats ();

   UFUNCTION()
      void RecalcStatusValues ();

   // Owners Movement Component. For stamina Regen.
private:
      UCharacterMovementComponent *MovementComponent = nullptr;
      bool bIsAdmin = false;

   //==========================================================================
   //                 Status
   //==========================================================================
protected:
   // Called by Tick
   virtual void StatusRegen (float DeltaTime);

   UPROPERTY(ReplicatedUsing = "OnRep_Status", Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Rade|Status")
      bool bDead = false;

   UPROPERTY(ReplicatedUsing = "OnRep_Status", Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Rade|Status")
      FRStatusValue Health;

   UPROPERTY(ReplicatedUsing = "OnRep_Status", Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Rade|Status")
      FRStatusValue Mana;

   UPROPERTY(ReplicatedUsing = "OnRep_Status", Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Rade|Status")
      FRStatusValue Stamina;

   //==========================================================================
   //                 Status Func
   //==========================================================================

public:

   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Status")
      bool IsDead () const;

   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Status")
      FRStatusValue GetHealth () const;

   UFUNCTION(BlueprintCallable, Category = "Rade|Status")
      void UseHealth (float Amount);

   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Status")
      FRStatusValue GetStamina () const;

   UFUNCTION(BlueprintCallable, Category = "Rade|Status")
      void UseStamina (float Amount);

   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Status")
      FRStatusValue GetMana () const;

   UFUNCTION(BlueprintCallable, Category = "Rade|Status")
      void UseMana (float Amount);

   //==========================================================================
   //                 Stats Data
   //==========================================================================

protected:
   // --- Core Stats
   UPROPERTY(ReplicatedUsing = "OnRep_Status", Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Rade|Status")
      FRCoreStats CoreStats_Base;

   UPROPERTY(ReplicatedUsing = "OnRep_Status", Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Rade|Status")
      FRCoreStats CoreStats_Added;

   // --- Extra Stats
   UPROPERTY(ReplicatedUsing = "OnRep_Status", Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Rade|Status")
      FRSubStats SubStats_Base;

   UPROPERTY(ReplicatedUsing = "OnRep_Status", Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Rade|Status")
      FRSubStats SubStats_Added;

   // --- Effects
   // Should be Map of FRSubStats, but for replication -> Array
   UPROPERTY(ReplicatedUsing = "OnRep_Status", Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Rade|Status")
      TArray<FRPassiveStatusEffectWithTag> PassiveEffects;

   // --- Resistance
   // Should be Map of FRResistanceStat, but for replication -> Array
   UPROPERTY(ReplicatedUsing = "OnRep_Status", Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Rade|Status")
      TArray<FRResistanceStatWithTag> Resistence;

   //==========================================================================
   //                 Status Value Curves
   //==========================================================================

protected:
   // --- Status Values
   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Status")
      FRuntimeFloatCurve StrToHealthMax;

   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Status")
      FRuntimeFloatCurve StrToHealthRegen;

   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Status")
      FRuntimeFloatCurve AgiToStaminaMax;

   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Status")
      FRuntimeFloatCurve AgiToStaminaRegen;

   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Status")
      FRuntimeFloatCurve IntToManaMax;

   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Status")
      FRuntimeFloatCurve IntToManaRegen;

   //==========================================================================
   //                 Extra Stat Curves
   //==========================================================================

   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Status")
      FRuntimeFloatCurve AgiToCritical;

   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Status")
      FRuntimeFloatCurve AgiToEvasion;

   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Status")
      FRuntimeFloatCurve AgiToAttackSpeed;

   //==========================================================================
   //                 Stats Funcs
   //==========================================================================

public:
   // --- Get Core Stats
   UFUNCTION(BlueprintCallable, Category = "Rade|Status")
      FRCoreStats GetCoreStats_Base () const;

   UFUNCTION(BlueprintCallable, Category = "Rade|Status")
      FRCoreStats GetCoreStats_Added () const;

   UFUNCTION(BlueprintCallable, Category = "Rade|Status")
      FRCoreStats GetCoreStats_Total () const;

   // --- Get Extra Stats
   UFUNCTION(BlueprintCallable, Category = "Rade|Status")
      FRSubStats GetSubStats_Base () const;

   UFUNCTION(BlueprintCallable, Category = "Rade|Status")
      FRSubStats GetSubStats_Added () const;

   UFUNCTION(BlueprintCallable, Category = "Rade|Status")
      FRSubStats GetSubStats_Total () const;

   // --- Stats functions

   UFUNCTION(BlueprintCallable, Category = "Rade|Status")
      bool HasStats (const FRCoreStats &RequiredStats) const;

   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Status")
      bool RollCritical () const;

   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Status")
      bool RollEvasion () const;

   //==========================================================================
   //                 Effect Funcs
   //==========================================================================

   UFUNCTION(BlueprintCallable, Category = "Rade|Status")
      bool SetEffects (const FString &Tag, const TArray<FRPassiveStatusEffect> &AddValues);

   UFUNCTION(BlueprintCallable, Category = "Rade|Status")
      bool RmEffects (const FString &Tag);

   UFUNCTION(BlueprintCallable, Category = "Rade|Status")
      TArray<FRPassiveStatusEffect> GetEffects () const;

   UFUNCTION(BlueprintCallable, Category = "Rade|Status")
      TArray<FRPassiveStatusEffectWithTag> GetEffectsWithTag () const;

   //==========================================================================
   //                 Resistance Funcs
   //==========================================================================

   UFUNCTION(BlueprintCallable, Category = "Rade|Status")
      void AddResistance (const FString &Tag, const TArray<FRResistanceStat> &AddValues);

   UFUNCTION(BlueprintCallable, Category = "Rade|Status")
      void RmResistance (const FString &Tag);

   UFUNCTION(BlueprintCallable, Category = "Rade|Status")
      TArray<FRResistanceStat> GetResistance () const;

   UFUNCTION(BlueprintCallable, Category = "Rade|Status")
      TArray<FRResistanceStatWithTag> GetResistanceWithTag () const;

   UFUNCTION(BlueprintCallable, Category = "Rade|Status")
      float GetResistanceFor (TSubclassOf<UDamageType> const DamageType) const;

   //==========================================================================
   //                 Events
   //==========================================================================

public:
   // Delegate when status updated
   UPROPERTY(BlueprintAssignable, Category = "Rade|Status")
      FRStatusMgrEvent OnStatusUpdated;

   // Calls from RCharacter
   float TakeDamage (float DamageAmount,
                     FDamageEvent const& DamageEvent,
                     AController* EventInstigator,
                     AActor* DamageCauser);

   //==========================================================================
   //                 Save/Load
   //==========================================================================

   // Status Saved / Loaded between sessions.
   // Careful with collision of 'SaveUniqueId'
   UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Rade|Status")
      bool bSaveLoad = false;

protected:
   // Rade Save events
   UFUNCTION()
      void OnSave ();
   UFUNCTION()
      void OnLoad ();


   //==========================================================================
   //                    UTIL
   //==========================================================================

public:
   static URStatusMgrComponent* Get (AActor* Target);
};

