// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "Components/ActorComponent.h"
#include "RJetpackComponent.generated.h"

class UCharacterMovementComponent;



UCLASS()
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
   TObjectPtr<UCharacterMovementComponent> MovementComponent;

   //==========================================================================
   //                         Params
   //==========================================================================

   // Current Charge Percent
   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rade|Jetpack")
      float CurrentChargePercent;

   // Minimal Usable Charge
   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rade|Jetpack")
      float MinUseablePercent;

   // restore Speed
   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rade|Jetpack")
      float RestoreSpeed;

   // Restore Value
   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rade|Jetpack")
      float RestorePower;

   // Push Multiplier
   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rade|Jetpack")
      float PushPower;

   //==========================================================================
   //                         Calls
   //==========================================================================

   UFUNCTION(Reliable, Server)
      void Use ();
      void Use_Implementation ();

   protected:
      void FillUp ();
};

