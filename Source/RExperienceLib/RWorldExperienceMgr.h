// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "Components/ActorComponent.h"
#include "RWorldExperienceMgr.generated.h"


UCLASS(Blueprintable, BlueprintType, ClassGroup=(_Rade), meta=(BlueprintSpawnableComponent))
class REXPERIENCELIB_API URWorldExperienceMgr : public UActorComponent
{
   GENERATED_BODY()
public:

   URWorldExperienceMgr ();

   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Experience", meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", DisplayName = "Get Target Mgr", CompactNodeTitle = "Target Mgr"))
      static URWorldExperienceMgr* GetInstance (UObject* WorldContextObject);
};

