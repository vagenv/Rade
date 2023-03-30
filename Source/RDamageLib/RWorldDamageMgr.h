// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "Components/ActorComponent.h"
#include "RWorldDamageMgr.generated.h"

class URDamageType;
class UActorComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams (FRAnyDamageEvent,
                                               AActor*,             Victim,
                                               float,               Amount,
                                               const URDamageType*, Type,
                                               AActor*,             Causer);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams (FRDeathEvent,
                                                AActor*,             Victim,
                                                AActor*,             Causer,
                                                const URDamageType*, DamageType);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam (FRReviveEvent,
                                             AActor*, WhoRevived);


UCLASS(Blueprintable, BlueprintType, ClassGroup=(_Rade), meta=(BlueprintSpawnableComponent))
class RDAMAGELIB_API URWorldDamageMgr : public UActorComponent
{
   GENERATED_BODY()
public:

   URWorldDamageMgr ();

   //==========================================================================
   //                  Events
   //==========================================================================

   // --- Take Damage
   UFUNCTION()
      void ReportDamage (AActor*             Victim,
                         float               Amount,
                         const URDamageType* Type,
                         AActor*             Causer);

   UPROPERTY(BlueprintAssignable, Category = "Rade|Damage")
      FRAnyDamageEvent OnAnyRDamage;


   // --- Died
   UFUNCTION()
      void ReportDeath (AActor*             Victim,
                        AActor*             Causer,
                        const URDamageType* Type);

   UPROPERTY(BlueprintAssignable, Category = "Rade|Damage")
      FRDeathEvent OnDeath;


   // --- Revived
   UFUNCTION()
      void ReportRevive (AActor* Victim);

   UPROPERTY(BlueprintAssignable, Category = "Rade|Damage")
      FRReviveEvent OnRevive;

   //==========================================================================
   //                  Get instance -> GameState component
   //==========================================================================

   UFUNCTION(BlueprintCallable, BlueprintPure,
             Category = "Rade|Damage",
             meta = (HidePin          = "WorldContextObject",
                     DefaultToSelf    = "WorldContextObject",
                     DisplayName      = "World Damage Mgr",
                     CompactNodeTitle = "World Damage Mgr"))
      static URWorldDamageMgr* GetInstance (UObject* WorldContextObject);
};

