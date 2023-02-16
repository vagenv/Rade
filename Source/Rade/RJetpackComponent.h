// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "Components/ActorComponent.h"
#include "RJetpackComponent.generated.h"

class URStatusMgrComponent;
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

   UFUNCTION()
      virtual bool CanUse () const;

   UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Rade|Jetpack")
      float UseCost = 30;

   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Jetpack")
      FRuntimeFloatCurve AgiToJumpPower;

   //==========================================================================
   //                         Calls
   //==========================================================================

   UFUNCTION(Reliable, Server)
      void Use ();
      virtual void Use_Implementation ();

   protected:

   UFUNCTION(BlueprintCallable, Category = "Rade|Jetpack")
      URStatusMgrComponent* GetStatusMgr () const;

   UFUNCTION(BlueprintCallable, Category = "Rade|Jetpack")
      UCharacterMovementComponent* GetMovementComponent () const;
};

