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

   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rade|Jetpack")
      FRuntimeFloatCurve AgiToJumpPower;

   //==========================================================================
   //                         Calls
   //==========================================================================

   UFUNCTION(Reliable, Server)
      virtual void Use ();
      void Use_Implementation ();

   protected:

   UFUNCTION(BlueprintCallable, Category = "Rade|Jetpack")
      URStatusMgrComponent* GetStatusMgr () const;

   UFUNCTION(BlueprintCallable, Category = "Rade|Jetpack")
      UCharacterMovementComponent* GetMovementComponent () const;
};

