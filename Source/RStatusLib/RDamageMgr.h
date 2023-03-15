// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "Components/ActorComponent.h"
#include "RDamageMgr.generated.h"

class URDamageType;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams (FRAnyDamageEvent,
                                               AActor*, DamageTarget,
                                               float, Damage,
                                               const URDamageType*, DamageType,
                                               AActor*, DamageCauser);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam (FRDeathEvent,
                                             AActor*, WhoDied);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam (FRReviveEvent,
                                             AActor*, WhoRevived);

UCLASS(Blueprintable, BlueprintType, ClassGroup=(_Rade), meta=(BlueprintSpawnableComponent))
class RSTATUSLIB_API URDamageMgr : public UActorComponent
{
   GENERATED_BODY()
public:

   URDamageMgr ();

   // --- When someone took damage
   UFUNCTION()
      void ReportDamage (AActor* DamageTarget,
                        float Damage,
                        const URDamageType* DamageType,
                        AActor* DamageCauser);

   UPROPERTY(BlueprintAssignable, Category = "Rade|Status")
      FRAnyDamageEvent OnAnyRDamage;


   // --- When someone died
   UFUNCTION()
      void ReportDeath (AActor* WhoDied);

   UPROPERTY(BlueprintAssignable, Category = "Rade|Status")
      FRDeathEvent OnDeath;


   // --- When someone revived
   UFUNCTION()
      void ReportRevive (AActor* WhoDied);

   UPROPERTY(BlueprintAssignable, Category = "Rade|Status")
      FRReviveEvent OnRevive;

   //==========================================================================
   //                  Get instamce -> GameState component
   //==========================================================================

   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Status", meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", DisplayName = "Get Damage Mgr", CompactNodeTitle = "Damage Mgr"))
      static URDamageMgr* GetInstance (UObject* WorldContextObject);
};

