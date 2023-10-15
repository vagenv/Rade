// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "RAbility.h"
#include "RAbility_Active.generated.h"

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

   //==========================================================================
   //                 Use
   //==========================================================================

   // Called by user
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

   // Effect has been re-applied
   UFUNCTION(NetMulticast, Unreliable, Category = "Rade|Ability")
              void Use_Global ();
      virtual void Use_Global_Implementation ();


   //==========================================================================
   //                 World objects
   //==========================================================================
protected:
   UPROPERTY ()
      TWeakObjectPtr<UWorld> World = nullptr;
};

