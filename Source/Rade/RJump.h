// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "RAbilityLib/RAbilityTypes.h"
#include "RJump.generated.h"


UCLASS(BlueprintType)
class RADE_API URAbility_Jump : public URAbility
{
   GENERATED_BODY()
public:

   URAbility_Jump ();

   virtual void Use () override;
   virtual bool CanUse () const override;

   UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Rade|Jump")
	   float StaminaCost = 40;

protected:
};


UCLASS(BlueprintType)
class RADE_API URAbility_DoubleJump : public URAbility
{
   GENERATED_BODY()
public:

   URAbility_DoubleJump ();

   virtual void Use () override;
   virtual bool CanUse () const override;

   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Jetpack")
      FRuntimeFloatCurve AgiToJumpPower;

   UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Rade|Jump")
	   float ManaCost = 30;

protected:
};

