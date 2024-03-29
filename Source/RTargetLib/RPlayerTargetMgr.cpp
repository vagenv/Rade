// Copyright 2015-2023 Vagen Ayrapetyan

#include "RPlayerTargetMgr.h"
#include "RTargetComponent.h"
#include "RWorldTargetMgr.h"
#include "RUtilLib/RUtil.h"
#include "RUtilLib/RCheck.h"
#include "RUtilLib/RLog.h"
#include "RUtilLib/RTimer.h"

#include "Net/UnrealNetwork.h"

//=============================================================================
//                         Core
//=============================================================================

URPlayerTargetMgr::URPlayerTargetMgr ()
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
void URPlayerTargetMgr::GetLifetimeReplicatedProps (TArray<FLifetimeProperty> &OutLifetimeProps) const
{
   Super::GetLifetimeReplicatedProps (OutLifetimeProps);
}

void URPlayerTargetMgr::BeginPlay ()
{
   Super::BeginPlay ();
   FindWorldTargetMgr ();
   World = URUtil::GetWorld (this);
}

void URPlayerTargetMgr::EndPlay (const EEndPlayReason::Type EndPlayReason)
{
   SetTargetCheckEnabled (false);
   Super::EndPlay (EndPlayReason);
}

//=============================================================================
//                         Init
//=============================================================================

void URPlayerTargetMgr::FindWorldTargetMgr ()
{
   WorldTargetMgr = URWorldTargetMgr::GetInstance (this);
   if (!WorldTargetMgr.IsValid ()) {
      FTimerHandle RetryHandle;
      RTIMER_START (RetryHandle,
                    this, &URPlayerTargetMgr::FindWorldTargetMgr,
                    1, false);
      return;
   }

   SetTargetCheckEnabled (true);
}

void URPlayerTargetMgr::SetTargetCheckEnabled (bool Enabled)
{
   if (Enabled) {
      RTIMER_START (TargetCheckHandle,
                    this, &URPlayerTargetMgr::TargetCheck,
                    1,
                    true);
   } else {
      RTIMER_STOP (TargetCheckHandle, this);
   }
}

//=============================================================================
//                         Get functions
//=============================================================================

bool URPlayerTargetMgr::IsTargeting () const
{
   return (TargetCurrent.IsValid () || !CustomTargetDir.IsNearlyZero ());
}

URTargetComponent* URPlayerTargetMgr::GetCurrentTarget () const
{
   return TargetCurrent.IsValid () ? TargetCurrent.Get () : nullptr;
}

FVector URPlayerTargetMgr::GetTargetDir () const
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

FRotator URPlayerTargetMgr::GetControlRotation ()
{
   // Current rotation
   FRotator Result = GetComponentRotation ();

   FVector CurrentDir = Result.Vector ();
   FVector TargetDir  = GetTargetDir ();

   // --- Is there a target
   if (!TargetDir.IsNearlyZero () && World.IsValid () && !World->bIsTearingDown) {

      // Normalize Direction and add offset;
      TargetDir.Normalize ();
      TargetDir += FVector (0, 0, TargetVerticalOffset);

      // Camera lerp speed
      float LerpPower = 4;

      float Angle = URUtil::GetAngle (CurrentDir, TargetDir);

      // Transform Angle to Lerp power
      LerpPower = URUtil::GetRuntimeFloatCurveValue (TargetAngleToLerpPower, Angle);

      // Remove targeting
      if (!CustomTargetDir.IsNearlyZero () && Angle < TargetStopAngle) CustomTargetDir = FVector::Zero ();

      // Tick DeltaTime
      float DeltaTime = World->GetDeltaSeconds ();

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
void URPlayerTargetMgr::TargetAdjust (float OffsetX, float OffsetY)
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
void URPlayerTargetMgr::TargetToggle ()
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
void URPlayerTargetMgr::TargetCheck ()
{
   if (TargetCurrent.IsValid () && WorldTargetMgr.IsValid ()) {

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
void URPlayerTargetMgr::SearchNewTarget (FVector2D InputVector)
{
   if (!WorldTargetMgr.IsValid () || !World.IsValid ()) return;

   // --- Limit the amount of camera adjustments
   double CurrentTime = World->GetTimeSeconds ();
   if (CurrentTime < LastTargetSearch + TargetSearchDelay) return;
   LastTargetSearch = CurrentTime;

   URTargetComponent* TargetNew = nullptr;

   if (TargetCurrent.IsValid ()) {

      // --- Adjust target
      TargetNew = WorldTargetMgr->Find_Screen (this,
                                               InputVector,
                                               TArray<AActor*>(),
                                               { TargetCurrent.Get () });
   } else {
      // --- Search new target
      TargetNew = WorldTargetMgr->Find_Target (this,
                                               TArray<AActor*>(),
                                               TArray<URTargetComponent*>());
   }

   if (IsValid (TargetNew)) SetTargetCurrent (TargetNew);
}

// Change the current target and notify the old and new target
void URPlayerTargetMgr::SetTargetCurrent (URTargetComponent* NewTarget)
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

   if (R_IS_NET_CLIENT) SetTargetCurrent_Server (NewTarget);
}

void URPlayerTargetMgr::SetTargetCurrent_Server_Implementation (URTargetComponent* NewTarget)
{
   TargetCurrent = NewTarget;
   ReportTargetUpdate ();
}

void URPlayerTargetMgr::ReportTargetUpdate () const
{
   if (R_IS_VALID_WORLD && OnTargetUpdated.IsBound ()) OnTargetUpdated.Broadcast ();
}

