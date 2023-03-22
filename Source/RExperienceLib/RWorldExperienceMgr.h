// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "Components/ActorComponent.h"
#include "RWorldExperienceMgr.generated.h"

class URDamageType;

UCLASS(Blueprintable, BlueprintType, ClassGroup=(_Rade), meta=(BlueprintSpawnableComponent))
class REXPERIENCELIB_API URWorldExperienceMgr : public UActorComponent
{
   GENERATED_BODY()
public:

   URWorldExperienceMgr ();

   virtual void BeginPlay () override;


   //==========================================================================
   //          Subscribe to DamageMgr
   //==========================================================================
protected:
   UFUNCTION()
      void OnDamage (AActor*             Victim,
                     float               Amount,
                     const URDamageType* Type,
                     AActor*             Causer);
   UFUNCTION()
      void OnDeath (AActor*             Victim,
                    AActor*             Causer,
                    const URDamageType* Type);

   //==========================================================================
   //                  Get instance -> GameState component
   //==========================================================================
public:

   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Experience", meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", DisplayName = "Get Target Mgr", CompactNodeTitle = "Target Mgr"))
      static URWorldExperienceMgr* GetInstance (UObject* WorldContextObject);
};

