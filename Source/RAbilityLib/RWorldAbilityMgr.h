// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "Components/ActorComponent.h"
#include "RWorldAbilityMgr.generated.h"

UCLASS(Blueprintable, BlueprintType, ClassGroup=(_Rade), meta=(BlueprintSpawnableComponent))
class RABILITYLIB_API URWorldAbilityMgr : public UActorComponent
{
   GENERATED_BODY()
public:

   URWorldAbilityMgr ();


   //==========================================================================
   //                  Get instance -> GameState component
   //==========================================================================

   UFUNCTION(BlueprintCallable, BlueprintPure,
             Category = "Rade|Damage",
             meta = (HidePin          = "WorldContextObject",
                     DefaultToSelf    = "WorldContextObject",
                     DisplayName      = "Get Ability Mgr",
                     CompactNodeTitle = "Ability Mgr"))
      static URWorldAbilityMgr* GetInstance (UObject* WorldContextObject);
};

