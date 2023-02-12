// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "Components/ActorComponent.h"
#include "RJetpackComponent.generated.h"

class UCharacterMovementComponent;

UCLASS(BlueprintType)
class RADE_API URJetpackComponent : public UActorComponent
{
   GENERATED_BODY()
public:

   //==========================================================================
   //                         Core
   //==========================================================================

   // Base events
   URJetpackComponent ();
   virtual void BeginPlay () override;

   // Owners Movement Component
protected:
   UCharacterMovementComponent *MovementComponent = nullptr;
public:

   //==========================================================================
   //                         Params
   //==========================================================================

   // Minimal Usable Charge
   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rade|Jetpack")
      float MinUseablePercent = 40;

   // Push Multiplier
   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rade|Jetpack")
      float PushPower = 7;

   //==========================================================================
   //                         Calls
   //==========================================================================

   UFUNCTION(Reliable, Server)
      void Use ();
      void Use_Implementation ();

   protected:
      void FillUp (float DeltaTime);
};

