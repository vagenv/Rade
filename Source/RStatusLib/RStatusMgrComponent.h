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
      void RecalcExtraStats ();

   UFUNCTION()
      void RecalcStatusValues ();

   // Owners Movement Component. For stamina Regen.
protected:
      UCharacterMovementComponent *MovementComponent = nullptr;

   //==========================================================================
   //                 Status
   //==========================================================================
protected:
   // Called by Tick
   virtual void StatusRegen (float DeltaTime);

   UPROPERTY(ReplicatedUsing = "OnRep_Status", Replicated, EditAnywhere, BlueprintReadOnly, Category = "Rade|Status")
      bool bDead = false;

   UPROPERTY(ReplicatedUsing = "OnRep_Status", Replicated, EditAnywhere, BlueprintReadOnly, Category = "Rade|Status")
      FRStatusValue Health;

   UPROPERTY(ReplicatedUsing = "OnRep_Status", Replicated, EditAnywhere, BlueprintReadOnly, Category = "Rade|Status")
      FRStatusValue Mana;

   UPROPERTY(ReplicatedUsing = "OnRep_Status", Replicated, EditAnywhere, BlueprintReadOnly, Category = "Rade|Status")
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
   UPROPERTY(ReplicatedUsing = "OnRep_Status", Replicated, EditAnywhere, BlueprintReadOnly, Category = "Rade|Status")
      FRCoreStats CoreStats_Base;

   UPROPERTY(ReplicatedUsing = "OnRep_Status", Replicated, EditAnywhere, BlueprintReadOnly, Category = "Rade|Status")
      FRCoreStats CoreStats_Added;

   // --- Extra Stats
   UPROPERTY(ReplicatedUsing = "OnRep_Status", Replicated, EditAnywhere, BlueprintReadOnly, Category = "Rade|Status")
      FRExtraStats ExtraStats_Base;

   UPROPERTY(ReplicatedUsing = "OnRep_Status", Replicated, EditAnywhere, BlueprintReadOnly, Category = "Rade|Status")
      FRExtraStats ExtraStats_Added;

   // --- Resistance
   // Should be Map of FRResistanceStat, but for replication -> Array
   UPROPERTY(ReplicatedUsing = "OnRep_Status", Replicated, EditAnywhere, BlueprintReadOnly, Category = "Rade|Status")
      TArray<FRResistanceStatWithTag> ExtraResistence;

   // --- Effects
   // Should be Map of FRExtraStats, but for replication -> Array
   UPROPERTY(ReplicatedUsing = "OnRep_Status", Replicated, EditAnywhere, BlueprintReadOnly, Category = "Rade|Status")
      TArray<FRStatusEffectWithTag> ExtraEffects;

   //==========================================================================
   //                 Stat curves
   //==========================================================================

protected:
   UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Rade|Status")
      FRuntimeFloatCurve StrToHealthMax;

   UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Rade|Status")
      FRuntimeFloatCurve StrToHealthRegen;

   UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Rade|Status")
      FRuntimeFloatCurve AgiToStaminaMax;

   UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Rade|Status")
      FRuntimeFloatCurve AgiToStaminaRegen;

   UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Rade|Status")
      FRuntimeFloatCurve AgiToCritical;

   UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Rade|Status")
      FRuntimeFloatCurve AgiToEvasion;

   UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Rade|Status")
      FRuntimeFloatCurve IntToManaMax;

   UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Rade|Status")
      FRuntimeFloatCurve IntToManaRegen;

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
      FRExtraStats GetExtraStats_Base () const;

   UFUNCTION(BlueprintCallable, Category = "Rade|Status")
      FRExtraStats GetExtraStats_Added () const;

   UFUNCTION(BlueprintCallable, Category = "Rade|Status")
      FRExtraStats GetExtraStats_Total () const;

   // --- Stats functions

   UFUNCTION(BlueprintCallable, Category = "Rade|Status")
      inline bool HasStats (const FRCoreStats &RequiredStats) const;

   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Status")
      inline bool RollCritical () const;

   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Status")
      inline bool RollEvasion () const;

   //==========================================================================
   //                 Effect Funcs
   //==========================================================================

   UFUNCTION(BlueprintCallable, Category = "Rade|Status")
      void SetEffects (const FString &Tag, const TArray<FRStatusEffect> &AddValues);

   UFUNCTION(BlueprintCallable, Category = "Rade|Status")
      void RmEffects (const FString &Tag);

   UFUNCTION(BlueprintCallable, Category = "Rade|Status")
      TArray<FRStatusEffect> GetEffects () const;

   //==========================================================================
   //                 Resistance Funcs
   //==========================================================================

   UFUNCTION(BlueprintCallable, Category = "Rade|Status")
      void AddResistance (const FString &Tag, const TArray<FRResistanceStat> &AddValues);

   UFUNCTION(BlueprintCallable, Category = "Rade|Status")
      void RmResistance (const FString &Tag);

   UFUNCTION(BlueprintCallable, Category = "Rade|Status")
      TArray<FRResistanceStat> GetResistance () const;

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
};

