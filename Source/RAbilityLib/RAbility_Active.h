// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "RAbility.h"
#include "RAbility_Active.generated.h"

class URStatusMgrComponent;

//=============================================================================
//                 Active Ability
//=============================================================================

UCLASS(Abstract, Blueprintable, BlueprintType, ClassGroup=(_Rade))
class RABILITYLIB_API URAbility_Active : public URAbility
{
   GENERATED_BODY()
public:

   URAbility_Active ();
   virtual void BeginPlay () override;

   //==========================================================================
   //                 Override
   //==========================================================================
public:
   virtual void AbilityInfoLoaded () override;
   virtual void SetIsEnabled (bool Enabled) override;

   //==========================================================================
   //                 Cost
   //==========================================================================

   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Ability")
      float HealthCost = 0;

   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Ability")
      float StaminaCost = 0;

   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Ability")
      float ManaCost = 0;

   //==========================================================================
   //                 Cooldown
   //==========================================================================

   // In seconds
   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Ability")
      float Cooldown = 3;

   // In seconds
   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Ability")
      double GetCooldownLeft () const;
private:
   UPROPERTY()
      double UseLastTime = 0;

   UPROPERTY()
      FTimerHandle CooldownResetHandle;
protected:
   UFUNCTION()
      virtual void CooldownReset ();

   //==========================================================================
   //                 Can Use
   //==========================================================================

   // Can the ability be used
   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Ability")
      virtual bool CanUse () const;

   UFUNCTION(BlueprintImplementableEvent, Category = "Rade|Ability")
      bool BP_CanUse () const;
      virtual bool BP_CanUse_Implementation () const  { return true; };

   //==========================================================================
   //                 Use
   //==========================================================================

   // Called by player input.
   // Client: Use -> Use_Server -> Use_Global
   // Server: Use -> Use_Global
   UFUNCTION(BlueprintCallable, Category = "Rade|Ability")
      virtual void Use ();

   // When Ability was used
   UPROPERTY(BlueprintAssignable, Category = "Rade|Ability")
      FRAbilityEvent OnAbilityUsed;

protected:

   // --- Server version
   UFUNCTION(Server, Reliable, Category = "Rade|Ability")
              void Use_Server ();
      virtual void Use_Server_Implementation ();

   // --- All clients recieve this event
   UFUNCTION(NetMulticast, Reliable, Category = "Rade|Ability")
              void Use_Global ();
      virtual void Use_Global_Implementation ();

   //==========================================================================
   //                 World objects
   //==========================================================================
protected:
   UPROPERTY ()
      TWeakObjectPtr<UWorld> World = nullptr;

   UPROPERTY (VisibleAnywhere, BlueprintReadOnly, Category = "Rade|Ability")
      TWeakObjectPtr<URStatusMgrComponent> StatusMgr = nullptr;
};

