// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "Components/ActorComponent.h"
#include "RExperienceComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE (FRExperienceEvent);

UCLASS(Blueprintable, BlueprintType, ClassGroup=(_Rade), meta=(BlueprintSpawnableComponent))
class REXPERIENCELIB_API URExperienceComponent : public UActorComponent
{
   GENERATED_BODY()
public:

   virtual void BeginPlay () override;
   virtual void EndPlay (const EEndPlayReason::Type EndPlayReason) override;
};
