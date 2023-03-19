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
                                                float,               Amount,
                                                const URDamageType*, Type,
                                                AActor*,             Causer);

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

   // Owners Movement Component. For stamina Regen.
private:
      UCharacterMovementComponent *MovementComponent = nullptr;
      bool bIsAdmin = false;

   //==========================================================================
   //                 Dead
   //==========================================================================
protected:
   UPROPERTY(ReplicatedUsing = "OnRep_Dead", Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Rade|Status")
      bool bDead = false;

   UFUNCTION()
      void OnRep_Dead ();

   //==========================================================================
   //                 Dead Func
   //==========================================================================

public:
   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Status")
      bool IsDead () const;

   UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Rade|Status")
      void SetDead (bool Dead);

   //==========================================================================
   //                 Status
   //==========================================================================

protected:
   UPROPERTY()
      FRStatusValue Health;

   UPROPERTY()
      FRStatusValue Mana;

   UPROPERTY()
      FRStatusValue Stamina;

   // Called by Tick
   virtual void StatusRegen (float DeltaTime);

   // --- Called by server to adjust client health
   UFUNCTION (NetMulticast, Unreliable)
      void SetHealth                (FRStatusValue Health_);
      void SetHealth_Implementation (FRStatusValue Health_);

   UFUNCTION (NetMulticast, Unreliable)
      void SetMana                (FRStatusValue Mana_);
      void SetMana_Implementation (FRStatusValue Mana_);

   UFUNCTION (NetMulticast, Unreliable)
      void SetStamina                (FRStatusValue Stamina_);
      void SetStamina_Implementation (FRStatusValue Stamina_);

   //==========================================================================
   //                 Status Func
   //==========================================================================

public:

   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Status")
      FRStatusValue GetHealth () const;

   UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Rade|Status")
      void UseHealth (float Amount);

   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Status")
      FRStatusValue GetStamina () const;

   UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Rade|Status")
      void UseStamina (float Amount);

   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Status")
      FRStatusValue GetMana () const;

   UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Rade|Status")
      void UseMana (float Amount);

   //==========================================================================
   //                 Stats Data
   //==========================================================================

protected:
   // --- Core Stats
   UPROPERTY(ReplicatedUsing = "OnRep_Stats", Replicated)
      FRCoreStats CoreStats_Base;

   UPROPERTY(ReplicatedUsing = "OnRep_Stats", Replicated)
      FRCoreStats CoreStats_Added;

   // --- Extra Stats
   UPROPERTY(ReplicatedUsing = "OnRep_Stats", Replicated)
      FRSubStats SubStats_Base;

   UPROPERTY(ReplicatedUsing = "OnRep_Stats", Replicated)
      FRSubStats SubStats_Added;

   // --- Effects
   // Should be Map of FRSubStats, but for replication -> Array
   UPROPERTY(ReplicatedUsing = "OnRep_Stats", Replicated)
      TArray<FRPassiveStatusEffectWithTag> PassiveEffects;

   // --- Resistance
   // Should be Map of FRResistanceStat, but for replication -> Array
   UPROPERTY(ReplicatedUsing = "OnRep_Stats", Replicated)
      TArray<FRResistanceStatWithTag> Resistence;

   UFUNCTION()
      void OnRep_Stats ();

   UFUNCTION()
      void RecalcStatus ();

   UFUNCTION()
      void RecalcCoreStats ();

   UFUNCTION()
      void RecalcSubStats ();

   UFUNCTION()
      void RecalcStatusValues ();

   //==========================================================================
   //                 Stats Funcs
   //==========================================================================

public:
   // --- Get Core Stats
   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Status")
      FRCoreStats GetCoreStats_Base () const;

   UFUNCTION(BlueprintCallable, Category = "Rade|Status")
      FRCoreStats GetCoreStats_Added () const;

   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Status")
      FRCoreStats GetCoreStats_Total () const;

   // --- Get Extra Stats
   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Status")
      FRSubStats GetSubStats_Base () const;

   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Status")
      FRSubStats GetSubStats_Added () const;

   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Status")
      FRSubStats GetSubStats_Total () const;

   // --- Stats functions
   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Status")
      bool HasStats (const FRCoreStats &RequiredStats) const;

   UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Rade|Status")
      bool RollCritical () const;

   UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Rade|Status")
      bool RollEvasion () const;

   //==========================================================================
   //                 Status Value Curves
   //==========================================================================
protected:
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
protected:
   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Status")
      FRuntimeFloatCurve AgiToCritical;

   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Status")
      FRuntimeFloatCurve AgiToEvasion;

   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Status")
      FRuntimeFloatCurve AgiToAttackSpeed;

   //==========================================================================
   //                 Passive Effect Funcs
   //==========================================================================
public:
   UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Rade|Status")
      bool SetPassiveEffects (const FString &Tag, const TArray<FRPassiveStatusEffect> &AddValues);

   UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Rade|Status")
      bool RmPassiveEffects (const FString &Tag);

   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Status")
      TArray<FRPassiveStatusEffect> GetPassiveEffects () const;

   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Status")
      TArray<FRPassiveStatusEffectWithTag> GetPassiveEffectsWithTag () const;

   //==========================================================================
   //                 Active Effect Funcs
   //==========================================================================
public:

   UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Rade|Status")
      bool AddActiveStatusEffect (AActor* Causer, const TSubclassOf<URActiveStatusEffect> Effect);

   //==========================================================================
   //                 Resistance Funcs
   //==========================================================================
public:
   UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Rade|Status")
      void AddResistance (const FString &Tag, const TArray<FRResistanceStat> &AddValues);

   UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Rade|Status")
      void RmResistance (const FString &Tag);

   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Status")
      TArray<FRResistanceStat> GetResistance () const;

   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Status")
      TArray<FRResistanceStatWithTag> GetResistanceWithTag () const;

   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Status")
      float GetResistanceFor (TSubclassOf<UDamageType> const DamageType) const;

   //==========================================================================
   //                 Events
   //==========================================================================
public:
   // Delegate when status updated
   UPROPERTY(BlueprintAssignable, Category = "Rade|Status")
      FRStatusMgrEvent OnStatsUpdated;

   UPROPERTY(BlueprintAssignable, Category = "Rade|Status")
      FRStatusMgrEvent OnActiveEffectsUpdated;

   UPROPERTY(BlueprintAssignable, Category = "Rade|Status")
      FRStatusMgrEvent OnDeath;

   UPROPERTY(BlueprintAssignable, Category = "Rade|Status")
      FRStatusMgrEvent OnRevive;

   UPROPERTY(BlueprintAssignable, Category = "Rade|Status")
      FRTakeDamageEvent OnAnyRDamage;

   UPROPERTY(BlueprintAssignable, Category = "Rade|Status")
      FRTakeDamageEvent OnEvadeRDamage;

private:
   // Connected to AActor::OnTakeAnyDamage event
   UFUNCTION()
      void AnyDamage (AActor*            Target,
                      float              Amount,
                      const UDamageType* Type,
                      AController*       InstigatedBy,
                      AActor*            Causer);

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

