// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "RExperienceTypes.generated.h"

class ACharacter;

USTRUCT(Blueprintable, BlueprintType)
struct REXPERIENCELIB_API FREnemyExp : public FTableRowBase
{
   GENERATED_BODY()

   UPROPERTY(EditAnywhere, BlueprintReadOnly)
      TSoftClassPtr<ACharacter> TargetClass;

   UPROPERTY(EditAnywhere, BlueprintReadOnly)
      float PerDamage = 1;

   UPROPERTY(EditAnywhere, BlueprintReadOnly)
      float PerDeath = 500;
};

