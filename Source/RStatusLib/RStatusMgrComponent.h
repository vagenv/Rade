// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "RStatusTypes.h"
#include "RStatusEffect.h"
#include "RSaveLib/RSaveInterface.h"
#include "RStatusMgrComponent.generated.h"

class URDamageType;
class UCharacterMovementComponent;
class URInventoryComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE (FRStatusMgrEvent);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams (FRTakeDamageEvent,
                                                float, Damage,
                                                const URDamageType*, DamageType,
                                                AActor*, DamageCauser);

// Status Manager Component.
UCLASS(Blueprintable, BlueprintType, ClassGroup=(_Rade), meta=(BlueprintSpawnableComponent))
class RSTATUSLIB_API URStatusMgrComponent : public UActorComponent, public IRSaveInterface
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

   UFUNCTION(BlueprintCallable, Category = "Rade|Status")
      void SetDead (bool Dead);

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

   // Active instances running
   UPROPERTY(ReplicatedUsing = "OnRep_Status", Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Rade|Status")
      TArray<ARActiveStatusEffect*> ActiveEffects;

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
   //                 Passive Effect Funcs
   //==========================================================================

   UFUNCTION(BlueprintCallable, Category = "Rade|Status")
      bool SetPassiveEffects (const FString &Tag, const TArray<FRPassiveStatusEffect> &AddValues);

   UFUNCTION(BlueprintCallable, Category = "Rade|Status")
      bool RmPassiveEffects (const FString &Tag);

   UFUNCTION(BlueprintCallable, Category = "Rade|Status")
      TArray<FRPassiveStatusEffect> GetPassiveEffects () const;

   UFUNCTION(BlueprintCallable, Category = "Rade|Status")
      TArray<FRPassiveStatusEffectWithTag> GetPassiveEffectsWithTag () const;

   //==========================================================================
   //                 Active Effect Funcs
   //==========================================================================

   UFUNCTION(BlueprintCallable, Category = "Rade|Status")
      TArray<ARActiveStatusEffect*> GetActiveEffects () const;

   // For internal use only
   bool AddActiveEffect (ARActiveStatusEffect* Effect);
   bool RmActiveEffect  (ARActiveStatusEffect* Effect);

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

   UPROPERTY(BlueprintAssignable, Category = "Rade|Status")
      FRStatusMgrEvent OnDeath;

   UPROPERTY(BlueprintAssignable, Category = "Rade|Status")
      FRStatusMgrEvent OnRevive;

   UPROPERTY(BlueprintAssignable, Category = "Rade|Status")
      FRTakeDamageEvent OnAnyRDamage;

protected:
   // Connected to AActor::OnTakeAnyDamage event
   UFUNCTION()
      void AnyDamage (AActor* DamagedActor,
                      float Damage,
                      const UDamageType* DamageType,
                      AController* InstigatedBy,
                      AActor* DamageCauser);

   //==========================================================================
   //                 Save/Load
   //==========================================================================
public:
   // Status Saved / Loaded between sessions.
   UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Rade|Status")
      bool bSaveLoad = false;

protected:
   virtual void OnSave (FBufferArchive &SaveData) override;
   virtual void OnLoad (FMemoryReader &LoadData) override;
};

