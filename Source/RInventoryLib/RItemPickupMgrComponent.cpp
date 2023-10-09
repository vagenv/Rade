// Copyright 2015-2023 Vagen Ayrapetyan

#include "RItemPickupMgrComponent.h"
#include "RItemPickup.h"

#include "RUtilLib/RLog.h"
#include "RUtilLib/RUtil.h"
#include "RUtilLib/RCheck.h"

//=============================================================================
//                 Core
//=============================================================================

URItemPickupMgrComponent::URItemPickupMgrComponent ()
{
}

void URItemPickupMgrComponent::BeginPlay()
{
   Super::BeginPlay();
   const UWorld *world = GetWorld ();
   if (!ensure (world)) return;

   world->GetTimerManager ().SetTimer (TimerClosestPickup,
      this, &URItemPickupMgrComponent::CheckClosestPickup, CheckClosestDelay, true);
}

void URItemPickupMgrComponent::EndPlay (const EEndPlayReason::Type EndPlayReason)
{
	GetWorld ()->GetTimerManager ().ClearTimer (TimerClosestPickup);
   Super::EndPlay (EndPlayReason);
}

//=============================================================================
//                 Pickup
//=============================================================================

bool URItemPickupMgrComponent::Pickup_Register (const ARItemPickup* Pickup)
{
   if (!ensure (IsValid (Pickup))) return false;
   CurrentPickups.Add (Pickup);
   ReportPickupListUpdated ();
   return true;
}

bool URItemPickupMgrComponent::Pickup_Unregister (const ARItemPickup* Pickup)
{
   if (!ensure (IsValid (Pickup))) return false;
   CurrentPickups.RemoveSingle (Pickup);
   ReportPickupListUpdated ();
   return true;
}

void URItemPickupMgrComponent::ReportPickupListUpdated ()
{
   if (R_IS_VALID_WORLD && OnPickupListUpdated.IsBound ()) OnPickupListUpdated.Broadcast ();
}

void URItemPickupMgrComponent::ReportClosestPickupUpdated ()
{
   if (R_IS_VALID_WORLD && OnClosestPickupUpdated.IsBound ()) OnClosestPickupUpdated.Broadcast ();
}

void URItemPickupMgrComponent::CheckClosestPickup ()
{
   FVector  MgrLoc = GetComponentLocation ();
   FRotator MgrRot = GetComponentRotation ();
   FVector  MgrDir = MgrRot.Vector ();
   MgrDir.Normalize ();

   TWeakObjectPtr<const ARItemPickup> newClosestPickup;

   TArray<TWeakObjectPtr<const ARItemPickup> > NewList;

   for (TWeakObjectPtr<const ARItemPickup> ItPickup : CurrentPickups) {
      if (!ItPickup.IsValid ()) continue;

      // If valid
      NewList.Add (ItPickup);

      // First one
      if (!newClosestPickup.IsValid ()) {
         newClosestPickup = ItPickup;
         continue;
      }

      if (SelectionMethod == ERClosestPickupSelectionMethod::Distance) {

         if ( FVector::Distance (MgrLoc,         ItPickup->GetActorLocation ())
            < FVector::Distance (MgrLoc, newClosestPickup->GetActorLocation ()))
         {
            newClosestPickup = ItPickup;
         }

      } else {

         // --- It Pickup angle
         FVector ItDir = ItPickup->GetActorLocation () - MgrLoc;
         ItDir.Normalize ();
         float ItAngle = URUtilLibrary::GetAngle (MgrDir, ItDir);

         // --- Selected Pickup Angle
         FVector CurrentDir = newClosestPickup->GetActorLocation () - MgrLoc;
         CurrentDir.Normalize ();
         float CurrentAngle = URUtilLibrary::GetAngle (MgrDir, CurrentDir);

         // Update selection
         if (ItAngle < CurrentAngle) newClosestPickup = ItPickup;
      }
   }

   // Check if one of the items is invalid
   if (NewList.Num () != CurrentPickups.Num ()) {
      CurrentPickups = NewList;
      ReportPickupListUpdated ();
   }

   if (newClosestPickup != ClosestPickup) {
      ClosestPickup = newClosestPickup;
      ReportClosestPickupUpdated ();
   }
}

