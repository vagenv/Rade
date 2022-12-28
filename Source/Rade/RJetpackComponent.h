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
   UCharacterMovementComponent *MovementComponent;

   //==========================================================================
   //                         Params
   //==========================================================================

   // Current Charge Percent
   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rade|Jetpack")
      float CurrentChargePercent = 50;

   // Minimal Usable Charge
   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rade|Jetpack")
      float MinUseablePercent = 40;

   // restore Speed
   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rade|Jetpack")
      float RestoreSpeed = 0.05f;

   // Restore Value
   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rade|Jetpack")
      float RestorePower = 0.5f;

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
      void FillUp ();
};

