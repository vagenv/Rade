// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "Components/ActorComponent.h"
#include "RWorldAbilityMgr.generated.h"

class URAbility;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam (FRAddAbilityEvent,
                                             URAbility*, Ability);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam (FRRmAbilityEvent,
                                             URAbility*, Ability);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam (FRUseAbilityEvent,
                                             URAbility*, Ability);

UCLASS(Blueprintable, BlueprintType, ClassGroup=(_Rade), meta=(BlueprintSpawnableComponent))
class RABILITYLIB_API URWorldAbilityMgr : public UActorComponent
{
   GENERATED_BODY()
public:

   URWorldAbilityMgr ();

   //==========================================================================
   //                  Events
   //==========================================================================

   // --- Add
   UFUNCTION()
      void ReportAddAbility (URAbility* Ability);

   UPROPERTY(BlueprintAssignable, Category = "Rade|Ability")
      FRAddAbilityEvent OnAddAbility;

   // --- Remove
   UFUNCTION()
      void ReportRmAbility (URAbility* Ability);

   UPROPERTY(BlueprintAssignable, Category = "Rade|Ability")
      FRRmAbilityEvent OnRmAbility;

   // --- Use
   UFUNCTION()
      void ReportUseAbility (URAbility* Ability);

   UPROPERTY(BlueprintAssignable, Category = "Rade|Ability")
      FRUseAbilityEvent OnUseAbility;

   //==========================================================================
   //                  Get instance -> GameState component
   //==========================================================================

   UFUNCTION(BlueprintCallable, BlueprintPure,
             Category = "Rade|Ability",
             meta = (HidePin          = "WorldContextObject",
                     DefaultToSelf    = "WorldContextObject",
                     DisplayName      = "Get Ability Mgr",
                     CompactNodeTitle = "Ability Mgr"))
      static URWorldAbilityMgr* GetInstance (UObject* WorldContextObject);
};

