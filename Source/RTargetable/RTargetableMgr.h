// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "Components/ActorComponent.h"
#include "RTargetableMgr.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE (FRTargetableEvent);

UCLASS(Blueprintable, BlueprintType, meta=(BlueprintSpawnableComponent) )
class RTARGETABLELIB_API URTargetableMgr : public UActorComponent
{
   GENERATED_BODY()
public:

   URTargetableMgr();

   UPROPERTY(BlueprintAssignable, Category = "Rade|Targetable")
      FRTargetableEvent OnListUpdated;


   //==========================================================================
   //                  Get instamce -> GameMode component
   //==========================================================================

   UFUNCTION(BlueprintCallable, Category = "Rade|Save")
      static URTargetableMgr* GetInstance (UObject* WorldContextObject);
};

