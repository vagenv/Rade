// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "RTargetingComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE (FRTargetingEvent);

class URTargetableComponent;
class URTargetableMgr;

UCLASS(Blueprintable, BlueprintType, ClassGroup=(_Rade), meta=(BlueprintSpawnableComponent))
class RTARGETABLELIB_API URTargetingComponent : public USceneComponent
{
	GENERATED_BODY()
public:

   //==========================================================================
   //                         Core
   //==========================================================================

	URTargetingComponent ();

	virtual void BeginPlay () override;
	virtual void TickComponent (float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

   //==========================================================================
   //                         Params
   //==========================================================================

   // Offset when turning to look at target
   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Targetable")
      float TargetVerticalOffset = -0.1;

	// Angle at which targeting will stop
   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Targetable")
      float TargetStopAngle = 1;

   // Speed at which camera turns to required direction
   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Targetable")
      FRuntimeFloatCurve TargetAngleToLerpPower;

   // Delay in seconds after look input before camera can be turned to target
   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Targetable")
      float TargetRefocusDelay = 1.5;

   //==========================================================================
   //                         Functions
   //==========================================================================

   // Searches for new focus target
   UFUNCTION(BlueprintCallable, Category = "Rade|Targetable")
      virtual void TargetToggle ();

	UFUNCTION(BlueprintCallable, Category = "Rade|Targetable")
      virtual void TargetAdjust ();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Targetable")
      virtual bool IsTargeting () const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Targetable")
      virtual FRotator GetTargetRotation () const;

   // Broadcasted when TargetCurrent has changed
   UPROPERTY(BlueprintAssignable, Category = "Rade|Targetable")
      FRTargetingEvent OnTargetUpdated;

protected:

	// What should be used
	UPROPERTY()
		FRotator TargetRotation;

   // Manager containing all available targets
   UPROPERTY()
      URTargetableMgr* TargetMgr = nullptr;

	// Current focus target
   UPROPERTY()
      URTargetableComponent* TargetCurrent = nullptr;

   // Direction where to look. Capsule direction
   UPROPERTY()
      FVector CustomTargetDir;

	// How long is left before camera can be turned to target
   float TargetFocusLeft = 0;


   // Handle to TargetCheck
   UPROPERTY()
      FTimerHandle TargetCheckHandle;

	// Called by Tick. Adjusts camera angle
   UFUNCTION()
      virtual void TargetingTick (float DeltaTime);

   // Checks if Targeting actor is valid and within range
   UFUNCTION()
      virtual void TargetCheck ();
};

