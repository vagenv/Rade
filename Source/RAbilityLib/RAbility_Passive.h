// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "RAbility.h"
#include "RAbility_Passive.generated.h"

//=============================================================================
//                 Passive Ability
//=============================================================================

UCLASS(Abstract, Blueprintable, BlueprintType, ClassGroup=(_Rade))
class RABILITYLIB_API URAbility_Passive : public URAbility
{
   GENERATED_BODY()
public:

   URAbility_Passive ();
};

//=============================================================================
//                 Aura Ability
//=============================================================================

UCLASS(Abstract, Blueprintable, BlueprintType, ClassGroup=(_Rade))
class RABILITYLIB_API URAbility_Aura : public URAbility_Passive
{
   GENERATED_BODY()
public:

   URAbility_Aura ();

   virtual void BeginPlay () override;
   virtual void EndPlay (const EEndPlayReason::Type EndPlayReason) override;

   //==========================================================================
   //                 Range checking
   //==========================================================================

   // Start/Stop timer
   UFUNCTION(BlueprintCallable, Category = "Rade|Ability")
      void SetCheckRangeActive (bool Enable);

   // Called in interval to create AffectedActirs list
   UFUNCTION()
      virtual void CheckRange ();

   // How often to check if target is within range
   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Ability")
      float CheckRangeInterval = 1;

   // Range for actor search
   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Ability",
             meta=(UIMin = "100.0", UIMax = "2000.0"))
      float Range = 1000;

private:
   UPROPERTY()
      FTimerHandle CheckRangeHandle;

   //==========================================================================
   //                 Target
   //==========================================================================
public:
   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Ability")
      TSoftClassPtr<AActor> TargetClass;

private:
   UPROPERTY()
      TObjectPtr<UClass> TargetClassLoaded;

   //==========================================================================
   //                 Get info
   //==========================================================================
public:
   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Ability")
      const TArray<AActor*> GetAffectedActors () const;

private:
   // Actors currently within range.
   UPROPERTY()
      TArray<TWeakObjectPtr<AActor> > AffectedActors;


   //==========================================================================
   //                 Events
   //==========================================================================
public:
   // Called after interal check range and target list updated
   UPROPERTY(BlueprintAssignable, Category = "Rade|Ability")
      FRAbilityEvent OnCheckRange;
};

