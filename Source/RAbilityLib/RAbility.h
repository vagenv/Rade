// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "RAbilityTypes.h"
#include "RAbility.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE (FRAbilityEvent);

class URAbilityMgrComponent;
class URWorldAbilityMgr;

// ============================================================================
//                   Ability Component
// ============================================================================

UCLASS(Abstract, Blueprintable, BlueprintType, ClassGroup=(_Rade))
class RABILITYLIB_API URAbility : public UActorComponent,
                                  public IRGetDescriptionInterface
{
   GENERATED_BODY()
public:

   URAbility ();

   virtual void BeginPlay () override;
   virtual void EndPlay (const EEndPlayReason::Type EndPlayReason) override;

   //==========================================================================
   //                 Core Params
   //==========================================================================

   // Called after WorldAbilityMgr becomes valid and AbilityInfo updated.
   UFUNCTION()
      virtual void AbilityInfoLoaded ();

   // Get class cached version. May be invalid
   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Ability")
      FRAbilityInfo GetAbilityInfo () const;

   // Wrapper around GetAbilityInfo ().Description
   virtual FRUIDescription GetDescription_Implementation () const override;

   //==========================================================================
   //                 Core Functions
   //==========================================================================

   // Disable / Enable the effect of ability
   UFUNCTION(BlueprintCallable, Category = "Rade|Ability")
      virtual void SetIsEnabled (bool IsEnabled_);

   // Should the effect of ability be applied
   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Ability")
      virtual bool GetIsEnabled () const;

private:

   UPROPERTY()
      bool IsEnabled = true;

   // Value will be updated when WorldAbilityMgr becomes valid
   UPROPERTY()
      FRAbilityInfo AbilityInfo;

protected:

   UFUNCTION()
      void PullAbilityInfo ();

   UPROPERTY()
      TWeakObjectPtr<URAbilityMgrComponent> OwnerAbilityMgr = nullptr;

   UPROPERTY()
      TWeakObjectPtr<URWorldAbilityMgr> WorldAbilityMgr = nullptr;
};

