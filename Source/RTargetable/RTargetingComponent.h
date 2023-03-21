// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "RTargetingComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE (FRTargetingEvent);

class URTargetableComponent;
class URWorldTargetMgr;

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
      float TargetVerticalOffset = -0.1f;

	// Angle at which targeting will stop
   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Targetable")
      float TargetStopAngle = 1.;

   // Speed at which camera turns to required direction
   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Targetable")
      FRuntimeFloatCurve TargetAngleToLerpPower;

	// How soon after a new target can be searched
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Targetable")
		float TargetSearchDelay = 0.2f;

   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Targetable")
      float TargetAdjustMinOffset = 0.3f;

   //==========================================================================
   //                         Functions
   //==========================================================================

   // Searches for new focus target
   UFUNCTION(BlueprintCallable, Category = "Rade|Targetable")
      virtual void TargetToggle ();

	UFUNCTION(BlueprintCallable, Category = "Rade|Targetable")
      virtual void TargetAdjust (float OffsetX, float OffsetY);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Targetable")
      virtual bool IsTargeting () const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Targetable")
      virtual FRotator GetTargetRotation () const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Targetable")
      virtual URTargetableComponent* GetCurrentTarget () const;

   // Broadcasted when TargetCurrent has changed
   UPROPERTY(BlueprintAssignable, Category = "Rade|Targetable")
      FRTargetingEvent OnTargetUpdated;

protected:

	// What should be used
	UPROPERTY()
		FRotator TargetRotation;

   // Manager containing all available targets
   UPROPERTY()
      URWorldTargetMgr* TargetMgr = nullptr;

	// Current focus target
   UPROPERTY()
      URTargetableComponent* TargetCurrent = nullptr;

   // Direction where to look. Capsule direction
   UPROPERTY()
      FVector CustomTargetDir;

	// Time in seconds when last search was performed
   double LastTargetSearch = 0;

   // Handle to TargetCheck
   UPROPERTY()
      FTimerHandle TargetCheckHandle;

	// Called by Tick. Adjusts camera angle
   UFUNCTION()
      virtual void TargetingTick (float DeltaTime);

   // Checks if Targeting actor is valid and within range
   UFUNCTION()
      virtual void TargetCheck ();

   // Perform search for new target
   UFUNCTION()
      virtual void SearchNewTarget (float InputOffsetX = 0, float InputOffsetY = 0);


   // In case user is targeting someone, notify server
   UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Rade|Targetable")
              void SetTarget_Server                (URTargetableComponent* TargetCurrent_);
      virtual void SetTarget_Server_Implementation (URTargetableComponent* TargetCurrent_);
};

