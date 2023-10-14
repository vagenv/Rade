// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "RStatusTypes.h"
#include "RPassiveStatusEffect.h"
#include "RStatusMgrComponent.generated.h"

class URDamageType;
class URWorldDamageMgr;
class URActiveStatusEffect;
class URInventoryComponent;
class URWorldStatusMgr;
class UCharacterMovementComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE (FRStatusMgrEvent);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams (FRTakeDamageEvent,
                                                float,               Amount,
                                                const URDamageType*, Type,
                                                AActor*,             Causer);

// Status Manager Component.
UCLASS(Blueprintable, BlueprintType, ClassGroup=(_Rade), meta=(BlueprintSpawnableComponent))
class RSTATUSLIB_API URStatusMgrComponent : public UActorComponent
{
   GENERATED_BODY()
public:

   //==========================================================================
   //                 Core
   //==========================================================================
   URStatusMgrComponent ();
   virtual void GetLifetimeReplicatedProps (TArray<FLifetimeProperty> &OutLifetimeProps) const override;
   virtual void BeginPlay () override;
   virtual void EndPlay (const EEndPlayReason::Type EndPlayReason) override;
   virtual void TickComponent (float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

   //==========================================================================
   //                 Stored pointers
   //==========================================================================
protected:
   // Owners Movement Component. For stamina Regen.
   UPROPERTY()
      TWeakObjectPtr<UCharacterMovementComponent> MovementComponent = nullptr;

   // For reporting applied status effects
   UPROPERTY()
      TWeakObjectPtr<URWorldStatusMgr> WorldStatusMgr = nullptr;

   UPROPERTY()
      TWeakObjectPtr<URWorldDamageMgr> WorldDamageMgr = nullptr;

private:

   UFUNCTION()
      void FindWorldMgrs ();

   //==========================================================================
   //                 Dead
   //==========================================================================
private:
   UPROPERTY(ReplicatedUsing = "OnRep_Dead", Replicated)
      bool bDead = false;

   UFUNCTION()
      void OnRep_Dead ();
public:
   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Status")
      bool IsDead () const;

   UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Rade|Status")
      void SetDead (bool Dead);

   UFUNCTION()
      void ReportDeath ();

   UPROPERTY(BlueprintAssignable, Category = "Rade|Status")
      FRStatusMgrEvent OnDeath;

   UFUNCTION()
      void ReportRevive ();

   UPROPERTY(BlueprintAssignable, Category = "Rade|Status")
      FRStatusMgrEvent OnRevive;

   //==========================================================================
   //                 Recalc status
   //==========================================================================
private:
   UFUNCTION()
      virtual void RecalcStatus ();

   UFUNCTION()
      virtual void RecalcStatusValues ();

   //==========================================================================
   //                 Status
   //==========================================================================
protected:
   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Status")
      FRStatusValue Health;

   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Status")
      FRStatusValue Mana;

   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Status")
      FRStatusValue Stamina;

private:
   // Backup of initial value.
   FRStatusValue Start_Health;
   FRStatusValue Start_Mana;
   FRStatusValue Start_Stamina;

   //==========================================================================
   //                 Status Func
   //==========================================================================
protected:
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

public:
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
   //                 Evasion / Critical
   //==========================================================================
protected:
   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Status")
      float EvasionChance = 0.;

   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Status")
      float CriticalChance = 0.;

   UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Rade|Status")
      bool RollCritical () const;

   UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Rade|Status")
      bool RollEvasion () const;

   //==========================================================================
   //                 Passive Effect
   //==========================================================================
protected:
   // Should be a Map of FRSubStats, but for replication -> Array
   UPROPERTY(ReplicatedUsing = "OnRep_PassiveEffects", Replicated)
      TArray<FRPassiveStatusEffectWithTag> PassiveEffects;

   UFUNCTION()
      void OnRep_PassiveEffects ();
public:
   UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Rade|Status")
      bool SetPassiveEffects (const FString& Tag, const TArray<FRPassiveStatusEffect>& AddValues);

   UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Rade|Status")
      bool RmPassiveEffects (const FString& Tag);

   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Status")
      TArray<FRPassiveStatusEffect> GetPassiveEffects () const;

   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Status")
      TArray<FRPassiveStatusEffectWithTag> GetPassiveEffectsWithTag () const;

   //==========================================================================
   //                 Active Effect
   //==========================================================================
public:
   UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Rade|Status")
      bool AddActiveStatusEffect (AActor* Causer_,
                                  const TSoftClassPtr<URActiveStatusEffect> Effect_);

   UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Rade|Status")
      static bool ApplyActiveStatusEffect (AActor* Causer_,
                                           AActor* Target_,
                                           const TSoftClassPtr<URActiveStatusEffect> Effect_);

   //==========================================================================
   //                 Resistance
   //==========================================================================
protected:
   // Should be Map of FRResistanceStat, but for replication -> Array
   UPROPERTY(ReplicatedUsing = "OnRep_Resistance", Replicated)
      TArray<FRDamageResistanceWithTag> Resistance;

   UFUNCTION()
      void OnRep_Resistance ();

public:
   UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Rade|Status")
      void AddResistance (const FString &Tag, const TArray<FRDamageResistance> &AddValues);

   UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Rade|Status")
      void RmResistance (const FString &Tag);

   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Status")
      TArray<FRDamageResistance> GetResistance () const;

   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Status")
      TArray<FRDamageResistanceWithTag> GetResistanceWithTag () const;

   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Status")
      FRDamageResistance GetResistanceFor (TSubclassOf<UDamageType> const DamageType) const;

   //==========================================================================
   //                 Events
   //==========================================================================
public:

   UFUNCTION()
      void ReporPassiveEffectsUpdated ();

   UPROPERTY(BlueprintAssignable, Category = "Rade|Status")
      FRStatusMgrEvent OnPassiveEffectsUpdated;

   UFUNCTION()
      void ReportActiveEffectsUpdated ();

   UPROPERTY(BlueprintAssignable, Category = "Rade|Status")
      FRStatusMgrEvent OnActiveEffectsUpdated;

   UFUNCTION()
      void ReportResistanceUpdated ();

   UPROPERTY(BlueprintAssignable, Category = "Rade|Status")
      FRStatusMgrEvent OnResistanceUpdated;


   //==========================================================================
   //                 Damage / Evade (Hooking to AnyDamage)
   //==========================================================================

private:
   // Connected to AActor::OnTakeAnyDamage event
   // Called only on server
   UFUNCTION()
      void AnyDamage (AActor*            Target,
                      float              Amount,
                      const UDamageType* Type,
                      AController*       InstigatedBy,
                      AActor*            Causer);

   // Unreliable -> to save bandwidth
   UFUNCTION (NetMulticast, Unreliable)
      void ReportRDamage                (float               Amount,
                                         const URDamageType* Type,
                                         AActor*             Causer);
      void ReportRDamage_Implementation (float               Amount,
                                         const URDamageType* Type,
                                         AActor*             Causer);
public:
   UPROPERTY(BlueprintAssignable, Category = "Rade|Status")
      FRTakeDamageEvent OnAnyRDamage;


private:
   UFUNCTION (NetMulticast, Unreliable)
      void ReportREvade                (float               Amount,
                                        const URDamageType* Type,
                                        AActor*             Causer);
      void ReportREvade_Implementation (float               Amount,
                                        const URDamageType* Type,
                                        AActor*             Causer);

public:
   UPROPERTY(BlueprintAssignable, Category = "Rade|Status")
      FRTakeDamageEvent OnEvadeRDamage;
};

