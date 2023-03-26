// Copyright 2015-2023 Vagen Ayrapetyan

#include "RTargetingComponent.h"
#include "RTargetableComponent.h"
#include "RWorldTargetMgr.h"
#include "RUtilLib/RUtil.h"
#include "RUtilLib/RCheck.h"
#include "RUtilLib/RLog.h"

//=============================================================================
//                         Core
//=============================================================================

URTargetingComponent::URTargetingComponent ()
{
	PrimaryComponentTick.bCanEverTick = true;

   // --- Targetable
   FRichCurve* TargetAngleToLerpPowerData = TargetAngleToLerpPower.GetRichCurve ();
   TargetAngleToLerpPowerData->AddKey (0,  10);
   TargetAngleToLerpPowerData->AddKey (1,   9);
   TargetAngleToLerpPowerData->AddKey (5,   7);
   TargetAngleToLerpPowerData->AddKey (20,  4);
   TargetAngleToLerpPowerData->AddKey (40,  5);
}

void URTargetingComponent::BeginPlay ()
{
	Super::BeginPlay ();

   TargetMgr = URWorldTargetMgr::GetInstance (this);

	GetOwner ()->GetWorldTimerManager ().SetTimer (TargetCheckHandle, this, &URTargetingComponent::TargetCheck, 1, true);
}

void URTargetingComponent::TickComponent (float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent (DeltaTime, TickType, ThisTickFunction);
	TargetingTick (DeltaTime);
}

//=============================================================================
//                         Get functions
//=============================================================================

bool URTargetingComponent::IsTargeting () const
{
	return (TargetCurrent || !CustomTargetDir.IsNearlyZero ());
}

FRotator URTargetingComponent::GetTargetRotation () const
{
	return TargetRotation;
}

URTargetableComponent* URTargetingComponent::GetCurrentTarget () const
{
   return TargetCurrent;
}

//=============================================================================
//                         Functions
//=============================================================================

// Creating smooth rotation to target
void URTargetingComponent::TargetingTick (float DeltaTime)
{
   FRotator CurrentRot = GetComponentRotation ();
   FVector  CurrentDir = CurrentRot.Vector ();
   FVector  TargetDir  = FVector::Zero ();

   // --- Focus Target
   if (TargetCurrent) {
      FVector TargetLocation = TargetCurrent->GetComponentLocation ();
      TargetDir = TargetLocation - GetComponentLocation ();
      TargetDir.Normalize ();

   // --- Focus Angle
   } else if (!CustomTargetDir.IsNearlyZero ()) {
      TargetDir = CustomTargetDir;
   }

   // --- Is there a target
   if (!TargetDir.IsNearlyZero ()) {

      // Normalize Direction and add offset;
      TargetDir.Normalize ();
      TargetDir += FVector (0, 0, TargetVerticalOffset);

      // Camera lerp speed
      float LerpPower = 4;

      float Angle = URWorldTargetMgr::GetAngle (CurrentDir, TargetDir);

      // Transform Angle to Lerp power
      LerpPower = URUtilLibrary::GetRuntimeFloatCurveValue (TargetAngleToLerpPower, Angle);

      // Remove targeting
      if (!CustomTargetDir.IsNearlyZero () && Angle < TargetStopAngle) CustomTargetDir = FVector::Zero ();

      // Lerp to Target Rotation
      float LerpValue = FMath::Clamp (DeltaTime * LerpPower, 0, 1);
      TargetRotation = FMath::Lerp (CurrentDir, TargetDir, LerpValue).Rotation ();
   }
}

// Camera input to change target
void URTargetingComponent::TargetAdjust (float OffsetX, float OffsetY)
{
   // If there was input stop turning
   if (!CustomTargetDir.IsNearlyZero ()) CustomTargetDir = FVector::Zero ();

   if (   TargetCurrent
      && (  FMath::Abs (OffsetX) > TargetAdjustMinOffset
         || FMath::Abs (OffsetY) > TargetAdjustMinOffset))
   {
      SearchNewTarget (OffsetX, OffsetY);
   }
}

// Targeting enabled/disabled
void URTargetingComponent::TargetToggle ()
{
   URTargetableComponent *TargetLast = TargetCurrent;

   if (TargetCurrent) {
      TargetCurrent->SetIsTargeted (false);
      TargetCurrent = nullptr;
      if (!R_IS_NET_ADMIN) SetTarget_Server (nullptr);
   } else {

      SearchNewTarget ();

      // No Target. Focus forward
      if (!TargetCurrent) CustomTargetDir = GetOwner ()->GetActorRotation ().Vector ();
   }
}

// Check if target is valid
void URTargetingComponent::TargetCheck ()
{
   if (TargetCurrent && TargetMgr) {

      bool RemoveTarget = false;

      // --- Check distance
      float Distance = FVector::Dist (GetOwner ()->GetActorLocation (), TargetCurrent->GetComponentLocation ());
      if (Distance > TargetMgr->SearchDistance) RemoveTarget = true;

      // Check if target was disabled
      if (!TargetCurrent->GetIsTargetable ()) RemoveTarget = true;

      // Remove
      if (RemoveTarget) {
         TargetCurrent->SetIsTargeted (false);
         TargetCurrent = nullptr;
         if (!R_IS_NET_ADMIN) SetTarget_Server (nullptr);
         if (R_IS_VALID_WORLD) OnTargetUpdated.Broadcast ();
         SearchNewTarget ();
      }
   }
}

void URTargetingComponent::SearchNewTarget (float InputOffsetX, float InputOffsetY)
{
   if (!TargetMgr) return;

   // --- Limit the amount of camera adjustments
   double CurrentTime = GetWorld ()->GetTimeSeconds ();
   if (CurrentTime < LastTargetSearch + TargetSearchDelay) return;
   LastTargetSearch = CurrentTime;

   TArray<AActor*>                blacklistActors;
   TArray<URTargetableComponent*> blacklistTargets;
   blacklistActors.Add (GetOwner ());

   URTargetableComponent* TargetNew = nullptr;

   if (TargetCurrent) {
      // --- Adjust target
      blacklistTargets.Add (TargetCurrent);

      TargetNew = TargetMgr->FindNear (this,
                                       TargetCurrent,
                                       InputOffsetX,
                                       InputOffsetY,
                                       blacklistActors,
                                       blacklistTargets);

   } else {
      // --- Search new target
      TargetNew = TargetMgr->Find (this,
                                   blacklistActors,
                                   blacklistTargets);
   }

   if (TargetNew) {
      // Unset old one
      if (TargetCurrent) TargetCurrent->SetIsTargeted (false);

      // Set new one
      TargetCurrent = TargetNew;
      TargetCurrent->SetIsTargeted (true);
      if (!R_IS_NET_ADMIN) SetTarget_Server (TargetCurrent);
      if (R_IS_VALID_WORLD) OnTargetUpdated.Broadcast ();
   }
}

void URTargetingComponent::SetTarget_Server_Implementation (URTargetableComponent* TargetCurrent_)
{
   TargetCurrent = TargetCurrent_;
}