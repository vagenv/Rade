// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "RStatusTypes.h"
#include "RDamageType.h"
#include "RStatusMgrComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE (FRStatusMgrEvent);

class UCharacterMovementComponent;

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
      void Calc_Status ();

   // --- Status

   // Get Funcs

   void StatusRegen (float DeltaTime);

   UPROPERTY(ReplicatedUsing = "OnRep_Status", Replicated, EditAnywhere, BlueprintReadOnly, Category = "Rade|Status")
      bool bDead = false;

   UPROPERTY(ReplicatedUsing = "OnRep_Status", Replicated, EditAnywhere, BlueprintReadOnly, Category = "Rade|Status")
      FRStatusValue Health;

   UPROPERTY(ReplicatedUsing = "OnRep_Status", Replicated, EditAnywhere, BlueprintReadOnly, Category = "Rade|Status")
      FRStatusValue Mana;

   UPROPERTY(ReplicatedUsing = "OnRep_Status", Replicated, EditAnywhere, BlueprintReadOnly, Category = "Rade|Status")
      FRStatusValue Stamina;

   // Owners Movement Component. For stamina Regen
protected:
      UCharacterMovementComponent *MovementComponent = nullptr;
public:

   // --- Stats
protected:
   UPROPERTY(ReplicatedUsing = "OnRep_Status", Replicated, EditAnywhere, BlueprintReadOnly, Category = "Rade|Status")
      FRCharacterStats BaseStats;
public:
   UFUNCTION(BlueprintCallable, Category = "Rade|Status")
      void AddStat (const FRCharacterStats &AddValue);

   UFUNCTION(BlueprintCallable, Category = "Rade|Status")
      void RmStat (const FRCharacterStats &RmValue);

   UFUNCTION(BlueprintCallable, Category = "Rade|Status")
      FRCharacterStats GetStats () const;

   UFUNCTION(BlueprintCallable, Category = "Rade|Status")
      bool HasStats (const FRCharacterStats &RequiredStats) const;

   // --- Resistance
protected:
   UPROPERTY(ReplicatedUsing = "OnRep_Status", Replicated, EditAnywhere, BlueprintReadOnly, Category = "Rade|Status")
      TArray<FRResistanceStat> BaseResistence;
public:
   UFUNCTION(BlueprintCallable, Category = "Rade|Status")
      void AddResistance (const TArray<FRResistanceStat> &AddValue);

   UFUNCTION(BlueprintCallable, Category = "Rade|Status")
      void RmResistance (const TArray<FRResistanceStat> &RmValue);

   UFUNCTION(BlueprintCallable, Category = "Rade|Status")
      TArray<FRResistanceStat> GetResistance () const;

   // --- Events
public:
   // Delegate when status updated
   UPROPERTY(BlueprintAssignable, Category = "Rade|Status")
      FRStatusMgrEvent OnStatusUpdated;

   // Calls from RCharacter
   float TakeDamage (float DamageAmount,
                     FDamageEvent const& DamageEvent,
                     AController* EventInstigator,
                     AActor* DamageCauser);

   // --- Save/Load

   // Status Saved / Loaded between sessions.
   // Should be used only for Player.
   // Careful with collision of 'PlayerUniqueId'
   UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Rade|Status")
      bool bSaveLoad = false;

protected:
   // Rade Save events
   UFUNCTION()
      void OnSave ();
   UFUNCTION()
      void OnLoad ();
};

