// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "RAbilityLib/RAbilityTypes.h"
#include "RJump.generated.h"


UCLASS(BlueprintType)
class RADE_API URAbility_Jump : public URAbility
{
   GENERATED_BODY()
public:

   virtual void Use () override;
   virtual bool CanUse () override;

protected:
};


UCLASS(BlueprintType)
class RADE_API URAbility_DoubleJump : public URAbility
{
   GENERATED_BODY()
public:

   virtual void Use () override;
   virtual bool CanUse () override;

protected:
};

