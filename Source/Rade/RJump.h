// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "RAbilityLib/RAbilityTypes.h"
#include "RJump.generated.h"

class ACharacter            ;
class UPawnMovementComponent;
class URStatusMgrComponent  ;

// ============================================================================
//                   Jump
// ============================================================================

UCLASS(BlueprintType)
class RADE_API URAbility_Jump : public URAbility
{
   GENERATED_BODY()
public:

   URAbility_Jump ();

   virtual void BeginPlay () override;
   virtual void TickComponent (float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

   virtual void Use () override;
   virtual bool CanUse () const override;

   UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Rade|Jump")
	   float StaminaCost = 40;

protected:

   bool OnGround = false;

   ACharacter             *Character     = nullptr;
   UPawnMovementComponent *MoveComponent = nullptr;
   URStatusMgrComponent   *StatusMgr     = nullptr;
};

// ============================================================================
//                   Double Jump
// ============================================================================

UCLASS(BlueprintType)
class RADE_API URAbility_DoubleJump : public URAbility
{
   GENERATED_BODY()
public:

   URAbility_DoubleJump ();

   virtual void BeginPlay     () override;
   virtual void TickComponent (float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

   virtual void Use    ()       override;
   virtual bool CanUse () const override;

   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Jetpack")
      FRuntimeFloatCurve AgiToJumpPower;

   UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Rade|Jump")
	   float ManaCost = 30;

protected:

   bool OnGround = false;
   bool Landed   = false;

   ACharacter             *Character     = nullptr;
   UPawnMovementComponent *MoveComponent = nullptr;
   URStatusMgrComponent   *StatusMgr     = nullptr;
};

