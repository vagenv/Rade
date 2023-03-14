// Copyright 2015-2023 Vagen Ayrapetyan

#include "RTargetingComponent.h"
#include "RTargetableComponent.h"
#include "RTargetableMgr.h"

#include "RUtilLib/RLog.h"
#include "DrawDebugHelpers.h"

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

   TargetMgr = URTargetableMgr::GetInstance (this);

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

void URTargetingComponent::TargetAdjust (float OffsetX, float OffsetY)
{
   // If there was input stop turning
   if (!CustomTargetDir.IsNearlyZero ()) CustomTargetDir = FVector::Zero ();

   if (  TargetCurrent
      && !TargetSearchLeft
      && (  FMath::Abs (OffsetX) > TargetAdjustMinOffset
         || FMath::Abs (OffsetY) > TargetAdjustMinOffset))
   {
      // Reset value
      TargetFocusLeft = TargetRefocusDelay;

      URTargetableComponent* TargetNew = FindTarget (OffsetX, OffsetY);
      if (TargetNew) {
         TargetCurrent->SetIsTargeted (false);
         TargetNew->SetIsTargeted (true);
         TargetCurrent = TargetNew;
         TargetFocusLeft = 0;
         TargetSearchLeft = TargetSearchDelay;
         OnTargetUpdated.Broadcast ();
      }
   }
}

void URTargetingComponent::TargetingTick (float DeltaTime)
{
   // Delay active
   TargetFocusLeft  = FMath::Clamp (TargetFocusLeft  - DeltaTime, 0, TargetRefocusDelay);
   TargetSearchLeft = FMath::Clamp (TargetSearchLeft - DeltaTime, 0, TargetSearchDelay);
   if (TargetFocusLeft > 0) return;

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

      float Angle = URTargetableMgr::GetAngle (CurrentDir, TargetDir);

      // Transform Angle to Lerp power
      const FRichCurve* TargetAngleToLerpPowerData = TargetAngleToLerpPower.GetRichCurveConst ();
      if (TargetAngleToLerpPowerData) LerpPower = TargetAngleToLerpPowerData->Eval (Angle);

      // Remove targeting
      if (!CustomTargetDir.IsNearlyZero () && Angle < TargetStopAngle) CustomTargetDir = FVector::Zero ();

      // Lerp to Target Rotation
      float LerpValue = FMath::Clamp (DeltaTime * LerpPower, 0, 1);
      TargetRotation = FMath::Lerp (CurrentDir, TargetDir, LerpValue).Rotation ();
   }
}

void URTargetingComponent::TargetToggle ()
{
   TargetFocusLeft = 0;

   URTargetableComponent *TargetLast = TargetCurrent;

   if (TargetCurrent) {
      TargetCurrent->SetIsTargeted (false);
      TargetCurrent = nullptr;
   } else {

      TargetCurrent = FindTarget ();

      // Set Target
      if (TargetCurrent)  TargetCurrent->SetIsTargeted (true);
      // Turn in actor direction
      else                CustomTargetDir = GetOwner ()->GetActorRotation ().Vector ();
   }
   if (TargetCurrent != TargetLast) OnTargetUpdated.Broadcast ();
}

void URTargetingComponent::TargetCheck ()
{
   if (TargetCurrent && TargetMgr) {

      // Check if was disabled/died
      if (!TargetCurrent->GetIsTargetable ()) {
         TargetCurrent->SetIsTargeted (false);
         TargetCurrent = FindTarget ();
         if (TargetCurrent) TargetCurrent->SetIsTargeted (true);
         OnTargetUpdated.Broadcast ();
         return;
      }

      bool RemoveTarget = false;

      // --- Check distance
      float Distance = FVector::Dist (GetOwner ()->GetActorLocation (), TargetCurrent->GetComponentLocation ());
      if (Distance > TargetMgr->SearchDistance) RemoveTarget = true;

      // --- Check Angle
      FVector CameraDir = GetComponentRotation ().Vector ();
      CameraDir.Normalize ();
      FVector TargetDir = TargetCurrent->GetComponentLocation () - GetComponentLocation ();
      TargetDir.Normalize ();
      if (URTargetableMgr::GetAngle (CameraDir, TargetDir) > TargetMgr->SearchAngle) RemoveTarget = true;

      // Remove
      if (RemoveTarget) {
         TargetCurrent->SetIsTargeted (false);
         TargetCurrent = nullptr;
         OnTargetUpdated.Broadcast ();
      }
   }
}

URTargetableComponent* URTargetingComponent::FindTarget (float OffsetX, float OffsetY) const
{
   if (!TargetMgr) return nullptr;
   //if (TargetFocusLeft > 0) return nullptr;

   // --- Ignore List
   TArray<AActor*>                blacklistActors;
   TArray<URTargetableComponent*> blacklistTargets;
   blacklistActors.Add (GetOwner ());
   if (TargetCurrent) blacklistTargets.Add (TargetCurrent);

   // --- Adjust search if required
   FVector  Location = GetComponentLocation ();
   FRotator Rotation = GetComponentRotation ();
   Rotation.Yaw   += FMath::Clamp (OffsetX, -1, 1) * TargetAdjustSearchDegree;
   Rotation.Pitch += FMath::Clamp (OffsetY, -1, 1) * TargetAdjustSearchDegree;


   URTargetableComponent* NewTarget = TargetMgr->Find (Location,
                                                       Rotation,
                                                       blacklistActors,
                                                       blacklistTargets);


   if (NewTarget) {

      FVector TargetDir = NewTarget->GetComponentLocation () - GetComponentLocation ();
      TargetDir.Normalize ();

      // --- Check Offset
      if (OffsetX) {
         float dot = FVector::DotProduct (GetRightVector (), TargetDir);
         if (OffsetX > 0 && dot < 0) return nullptr;
         if (OffsetX < 0 && dot > 0) return nullptr;
      }

      if (OffsetY) {
         float dot = FVector::DotProduct (GetUpVector (), TargetDir);

         if (OffsetY > 0 && dot < 0) return nullptr;
         if (OffsetY < 0 && dot > 0) return nullptr;
      }

   }


   // Check that new target is in correct direction

   return NewTarget;
}
