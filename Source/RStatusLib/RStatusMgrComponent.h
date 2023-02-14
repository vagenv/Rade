// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "RStatusTypes.h"
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

   // Owners Movement Component. For stamina Regen.
protected:
      UCharacterMovementComponent *MovementComponent = nullptr;
public:

   //==========================================================================
   //                 Status
   //==========================================================================

public:
   // Called by Tick
   void StatusRegen (float DeltaTime);

   UPROPERTY(ReplicatedUsing = "OnRep_Status", Replicated, EditAnywhere, BlueprintReadOnly, Category = "Rade|Status")
      bool bDead = false;

   UPROPERTY(ReplicatedUsing = "OnRep_Status", Replicated, EditAnywhere, BlueprintReadOnly, Category = "Rade|Status")
      FRStatusValue Health;

   UPROPERTY(ReplicatedUsing = "OnRep_Status", Replicated, EditAnywhere, BlueprintReadOnly, Category = "Rade|Status")
      FRStatusValue Mana;

   UPROPERTY(ReplicatedUsing = "OnRep_Status", Replicated, EditAnywhere, BlueprintReadOnly, Category = "Rade|Status")
      FRStatusValue Stamina;

   //==========================================================================
   //                 Stats Data
   //==========================================================================

protected:
   UPROPERTY(ReplicatedUsing = "OnRep_Status", Replicated, EditAnywhere, BlueprintReadOnly, Category = "Rade|Status")
      FRCharacterStats Stats_Base;

   // Should be Map, Bit for replication -> Array
   UPROPERTY(ReplicatedUsing = "OnRep_Status", Replicated, EditAnywhere, BlueprintReadOnly, Category = "Rade|Status")
      TArray<FRExtraStat> Stats_Extra;

   UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category = "Rade|Status")
      float CriticalChance = 0;

   UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category = "Rade|Status")
      float EvasionChance = 0;

public:
   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Status")
      inline bool RollCritical () const;

   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Status")
      inline bool RollEvasion () const;

public:
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
   // --- Get Stats
   UFUNCTION(BlueprintCallable, Category = "Rade|Status")
      FRCharacterStats GetStatsBase () const;

   UFUNCTION(BlueprintCallable, Category = "Rade|Status")
      FRCharacterStats GetStatsAdd () const;

   UFUNCTION(BlueprintCallable, Category = "Rade|Status")
      FRCharacterStats GetStatsTotal () const;

   UFUNCTION(BlueprintCallable, Category = "Rade|Status")
      bool SetExtraStat (const FString &Tag, const FRCharacterStats &ExtraValue);

   UFUNCTION(BlueprintCallable, Category = "Rade|Status")
      bool RmExtraStat (const FString &Tag);

   UFUNCTION(BlueprintCallable, Category = "Rade|Status")
      bool HasStats (const FRCharacterStats &RequiredStats) const;

   //==========================================================================
   //                 Resistance
   //==========================================================================

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

