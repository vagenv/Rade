// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "Components/ActorComponent.h"
#include "RWorldDamageMgr.generated.h"

class URDamageType;
class UActorComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams (FRAnyDamageEvent,
                                               AActor*,             Target,
                                               float,               Amount,
                                               const URDamageType*, Type,
                                               AActor*,             Causer);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams (FRDeathEvent,
                                                AActor*, Victim,
                                                AActor*, Causer,
                                                const URDamageType*, DamageType);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam (FRReviveEvent,
                                             AActor*, WhoRevived);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams (FRStatusEffectEvent,
                                                UActorComponent*,    Effect, // Cast to URActiveStatusEffect
                                                AActor*,             Causer,
                                                AActor*,             Target);

UCLASS(Blueprintable, BlueprintType, ClassGroup=(_Rade), meta=(BlueprintSpawnableComponent))
class RDAMAGELIB_API URWorldDamageMgr : public UActorComponent
{
   GENERATED_BODY()
public:

   URWorldDamageMgr ();

   // --- When someone took damage
   UFUNCTION()
      void ReportDamage (AActor*             Target,
                         float               Amount,
                         const URDamageType* Type,
                         AActor*             Causer);

   UPROPERTY(BlueprintAssignable, Category = "Rade|Damage")
      FRAnyDamageEvent OnAnyRDamage;


   // --- When someone died
   UFUNCTION()
      void ReportDeath (AActor* Victim,
                        AActor* Causer,
                        const URDamageType* DamageType);

   UPROPERTY(BlueprintAssignable, Category = "Rade|Damage")
      FRDeathEvent OnDeath;


   // --- When someone revived
   UFUNCTION()
      void ReportRevive (AActor* WhoRevived);

   UPROPERTY(BlueprintAssignable, Category = "Rade|Damage")
      FRReviveEvent OnRevive;


   // --- When status Effect was applied
   UFUNCTION()
      void ReportStatusEffect (UActorComponent* Effect,
                               AActor*          Causer,
                               AActor*          Target);

   UPROPERTY(BlueprintAssignable, Category = "Rade|Damage")
      FRStatusEffectEvent OnStatusEffect;

   //==========================================================================
   //                  Get instamce -> GameState component
   //==========================================================================

   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Damage", meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", DisplayName = "Get Damage Mgr", CompactNodeTitle = "Damage Mgr"))
      static URWorldDamageMgr* GetInstance (UObject* WorldContextObject);
};

