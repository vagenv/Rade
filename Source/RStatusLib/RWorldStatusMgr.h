// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "RWorldStatusMgr.generated.h"

class URActiveStatusEffect;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams (FRStatusEffectEvent,
                                                URActiveStatusEffect*, Effect,
                                                AActor*,               Causer,
                                                AActor*,               Victim);

UCLASS(Blueprintable, BlueprintType, ClassGroup=(_Rade), meta=(BlueprintSpawnableComponent))
class RSTATUSLIB_API URWorldStatusMgr : public UActorComponent
{
   GENERATED_BODY()
public:

   URWorldStatusMgr ();
   virtual void BeginPlay () override;

   //==========================================================================
   //          Subscribe to DamageMgr
   //==========================================================================
public:

   // --- When status Effect was applied
   UFUNCTION()
      void ReportStatusEffect (URActiveStatusEffect* Effect,
                               AActor*               Causer,
                               AActor*               Victim);

   UPROPERTY(BlueprintAssignable, Category = "Rade|Damage")
      FRStatusEffectEvent OnStatusEffect;

   //==========================================================================
   //           Get instance -> GameState component
   //==========================================================================
public:
   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Status", meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", DisplayName = "Get Target Mgr", CompactNodeTitle = "Target Mgr"))
      static URWorldStatusMgr* GetInstance (UObject* WorldContextObject);
};

