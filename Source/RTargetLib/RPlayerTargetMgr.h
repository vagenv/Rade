// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "RPlayerTargetMgr.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE (FRTargetingEvent);

class URTargetComponent;
class URWorldTargetMgr;

UCLASS(Blueprintable, BlueprintType, ClassGroup=(_Rade), meta=(BlueprintSpawnableComponent))
class RTARGETLIB_API URPlayerTargetMgr : public USceneComponent
{
   GENERATED_BODY()
public:

   //==========================================================================
   //                         Core
   //==========================================================================

   URPlayerTargetMgr ();

   virtual void GetLifetimeReplicatedProps (TArray<FLifetimeProperty> &OutLifetimeProps) const override;
   virtual void BeginPlay () override;
   virtual void EndPlay (const EEndPlayReason::Type EndPlayReason) override;

   //==========================================================================
   //                         Target Mgr
   //==========================================================================
protected:
   UFUNCTION ()
      void FindWorldTargetMgr ();

   // Manager containing all available targets
   UPROPERTY()
      TWeakObjectPtr<URWorldTargetMgr> WorldTargetMgr = nullptr;

   UPROPERTY()
      TWeakObjectPtr<UWorld> World = nullptr;

   //==========================================================================
   //                         Params
   //==========================================================================
public:
   // Offset when turning to look at target
   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Target")
      float TargetVerticalOffset = -0.1f;

   // Angle at which targeting will stop
   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Target")
      float TargetStopAngle = 1.;

   // Speed at which camera turns to required direction
   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Target")
      FRuntimeFloatCurve TargetAngleToLerpPower;

   // How often can a new target can be searched
   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Target")
      float TargetSearchDelay = 0.2f;

   // Minimum input to start new target search
   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Target")
      float TargetAdjustMinOffset = 0.3f;

   //==========================================================================
   //                         Functions
   //==========================================================================

   // Searches for new focus target
   UFUNCTION(BlueprintCallable, Category = "Rade|Target")
      virtual void TargetToggle ();

   UFUNCTION(BlueprintCallable, Category = "Rade|Target")
      virtual void TargetAdjust (float OffsetX, float OffsetY);

   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Target")
      virtual bool IsTargeting () const;

   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Target")
      virtual URTargetComponent* GetCurrentTarget () const;

   // Direction of current target
   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Target")
      virtual FVector GetTargetDir () const;

   // Value should be fed to SetControlRotation
   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Target")
      virtual FRotator GetControlRotation ();

   // Broadcasted when TargetCurrent has changed
   UPROPERTY(BlueprintAssignable, Category = "Rade|Target")
      FRTargetingEvent OnTargetUpdated;

protected:

   // Current focus target
   UPROPERTY()
      TWeakObjectPtr<URTargetComponent> TargetCurrent = nullptr;

   // Direction where to look. Owner root forward direction
   UPROPERTY()
      FVector CustomTargetDir;

   // Time in seconds when last search was performed
   double LastTargetSearch = 0;

   // Handle to TargetCheck
   UPROPERTY()
      FTimerHandle TargetCheckHandle;

   UFUNCTION()
      virtual void SetTargetCheckEnabled (bool Enabled);

   // Checks if Targeting actor is valid and within range
   UFUNCTION()
      virtual void TargetCheck ();

   // Perform search for new target
   UFUNCTION()
      virtual void SearchNewTarget (FVector2D InputVector = FVector2D::ZeroVector);

   // Change the current target and notify the old and new target
   UFUNCTION()
      void SetTargetCurrent (URTargetComponent* NewTarget);

   // Called by client to notify server
   UFUNCTION(Server, Reliable)
      void SetTargetCurrent_Server                (URTargetComponent* NewTarget);
      void SetTargetCurrent_Server_Implementation (URTargetComponent* NewTarget);

   // Called when target is updated to broadcast event
   UFUNCTION()
      void ReportTargetUpdate () const;
};

