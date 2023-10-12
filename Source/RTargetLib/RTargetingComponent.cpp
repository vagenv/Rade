// Copyright 2015-2023 Vagen Ayrapetyan

#include "RTargetingComponent.h"
#include "RTargetComponent.h"
#include "RWorldTargetMgr.h"
#include "RUtilLib/RUtil.h"
#include "RUtilLib/RCheck.h"
#include "RUtilLib/RLog.h"

#include "Net/UnrealNetwork.h"

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

// Replication
void URTargetingComponent::GetLifetimeReplicatedProps (TArray<FLifetimeProperty> &OutLifetimeProps) const
{
   Super::GetLifetimeReplicatedProps (OutLifetimeProps);
   DOREPLIFETIME (URTargetingComponent, TargetCurrent);
}

void URTargetingComponent::BeginPlay ()
{
	Super::BeginPlay ();
   FindWorldTargetMgr ();
}

void URTargetingComponent::EndPlay (const EEndPlayReason::Type EndPlayReason)
{
   SetTargetCheckEnabled (false);
   Super::EndPlay (EndPlayReason);
}

//=============================================================================
//                         Init
//=============================================================================

void URTargetingComponent::FindWorldTargetMgr ()
{
   WorldTargetMgr = URWorldTargetMgr::GetInstance (this);
   if (!WorldTargetMgr) {
      FTimerHandle RetryHandle;
      GetWorld ()->GetTimerManager ().SetTimer (RetryHandle,
                                                this, &URTargetingComponent::FindWorldTargetMgr,
                                                1);
      return;
   }

   SetTargetCheckEnabled (true);
}

void URTargetingComponent::SetTargetCheckEnabled (bool Enabled)
{
   if (Enabled) {
      if (!TargetCheckHandle.IsValid ())
	      GetOwner ()->GetWorldTimerManager ().SetTimer (TargetCheckHandle,
                                                         this, &URTargetingComponent::TargetCheck,
                                                         1,
                                                         true);
   } else {
      if (TargetCheckHandle.IsValid ()) GetWorld ()->GetTimerManager ().ClearTimer (TargetCheckHandle);
   }
}

//=============================================================================
//                         Get functions
//=============================================================================

bool URTargetingComponent::IsTargeting () const
{
	return (TargetCurrent.IsValid () || !CustomTargetDir.IsNearlyZero ());
}

URTargetComponent* URTargetingComponent::GetCurrentTarget () const
{
   return TargetCurrent.IsValid () ? TargetCurrent.Get () : nullptr;
}

FVector URTargetingComponent::GetTargetDir () const
{
   FVector Result = FVector::Zero ();

   // --- Focus Target
   if (TargetCurrent.IsValid ()) {
      FVector TargetLocation = TargetCurrent->GetComponentLocation ();
      Result = TargetLocation - GetComponentLocation ();
      Result.Normalize ();

   // --- Focus Angle
   } else if (!CustomTargetDir.IsNearlyZero ()) {
      Result = CustomTargetDir;
   }
   return Result;
}

FRotator URTargetingComponent::GetControlRotation ()
{
   // Current rotation
   FRotator Result = GetComponentRotation ();

   FVector CurrentDir = Result.Vector ();
   FVector TargetDir  = GetTargetDir ();

   // --- Is there a target
   if (!TargetDir.IsNearlyZero ()) {

      // Normalize Direction and add offset;
      TargetDir.Normalize ();
      TargetDir += FVector (0, 0, TargetVerticalOffset);

      // Camera lerp speed
      float LerpPower = 4;

      float Angle = URUtilLibrary::GetAngle (CurrentDir, TargetDir);

      // Transform Angle to Lerp power
      LerpPower = URUtilLibrary::GetRuntimeFloatCurveValue (TargetAngleToLerpPower, Angle);

      // Remove targeting
      if (!CustomTargetDir.IsNearlyZero () && Angle < TargetStopAngle) CustomTargetDir = FVector::Zero ();

      // Tick DeltaTime
      float DeltaTime = GetWorld ()->GetDeltaSeconds ();

      // Lerp to Target Rotation
      float LerpValue = FMath::Clamp (DeltaTime * LerpPower, 0, 1);
      Result = FMath::Lerp (CurrentDir, TargetDir, LerpValue).Rotation ();
   }
   return Result;
}

//=============================================================================
//                         Functions
//=============================================================================

// Camera input to change target
void URTargetingComponent::TargetAdjust (float OffsetX, float OffsetY)
{
   // If there was input stop turning
   if (!CustomTargetDir.IsNearlyZero ()) CustomTargetDir = FVector::Zero ();

   if (   TargetCurrent.IsValid ()
      && (  FMath::Abs (OffsetX) > TargetAdjustMinOffset
         || FMath::Abs (OffsetY) > TargetAdjustMinOffset))
   {
      SearchNewTarget (FVector2D (OffsetX, OffsetY));
   }
}

// Targeting enabled/disabled
void URTargetingComponent::TargetToggle ()
{
   if (TargetCurrent.IsValid ()) {
      SetTargetCurrent (nullptr);
   } else {
      SearchNewTarget ();

      // No Target. Focus forward
      if (!TargetCurrent.IsValid ()) CustomTargetDir = GetOwner ()->GetActorRotation ().Vector ();
   }
}

// Check if target is valid
void URTargetingComponent::TargetCheck ()
{
   if (TargetCurrent.IsValid () && WorldTargetMgr) {

      bool RemoveTarget = false;

      // --- Check distance
      float Distance = FVector::Dist (GetOwner ()->GetActorLocation (), TargetCurrent->GetComponentLocation ());
      if (Distance > WorldTargetMgr->SearchDistanceMax) RemoveTarget = true;

      // Check if target was disabled
      if (!TargetCurrent->GetIsTargetable ()) RemoveTarget = true;

      // Remove
      if (RemoveTarget) {
         SetTargetCurrent (nullptr);
         SearchNewTarget ();
      }
   }
}

// Perform search for new target
void URTargetingComponent::SearchNewTarget (FVector2D InputVector)
{
   if (!WorldTargetMgr) return;

   // --- Limit the amount of camera adjustments
   double CurrentTime = GetWorld ()->GetTimeSeconds ();
   if (CurrentTime < LastTargetSearch + TargetSearchDelay) return;
   LastTargetSearch = CurrentTime;

   URTargetComponent* TargetNew = nullptr;

   if (TargetCurrent.IsValid ()) {
      TArray<URTargetComponent*> ExcludeTargets;
      ExcludeTargets.Add (TargetCurrent.Get ());

      // --- Adjust target
      TargetNew = WorldTargetMgr->Find_Screen (this,
                                               InputVector,
                                               TArray<AActor*>(),
                                               ExcludeTargets);
   } else {
      // --- Search new target
      TargetNew = WorldTargetMgr->Find_Target (this,
                                               TArray<AActor*>(),
                                               TArray<URTargetComponent*>());
   }

   if (IsValid (TargetNew)) SetTargetCurrent (TargetNew);
}

// Change the current target and notify the old and new target
void URTargetingComponent::SetTargetCurrent (URTargetComponent* NewTarget)
{
   // Notify the old target
   if (TargetCurrent.IsValid ()) {
      TargetCurrent->SetIsTargeted (false);
   }

   // Reset the state
   TargetCurrent = nullptr;

   // Set new target and notify it
   if (IsValid (NewTarget)) {
      TargetCurrent = NewTarget;
      TargetCurrent->SetIsTargeted (true);
   }

   // Report
   ReportTargetUpdate ();
}

void URTargetingComponent::OnRep_TargetCurrent ()
{
   ReportTargetUpdate ();
}

void URTargetingComponent::ReportTargetUpdate () const
{
   if (R_IS_VALID_WORLD && OnTargetUpdated.IsBound ()) OnTargetUpdated.Broadcast ();
}

