// Copyright 2015-2023 Vagen Ayrapetyan

#include "RItemPickupMgrComponent.h"
#include "RItemPickup.h"

#include "RUtilLib/RLog.h"
#include "RUtilLib/RUtil.h"
#include "RUtilLib/RCheck.h"
#include "RUtilLib/RTimer.h"

//=============================================================================
//                 Core
//=============================================================================

URItemPickupMgrComponent::URItemPickupMgrComponent ()
{
}

void URItemPickupMgrComponent::BeginPlay ()
{
   Super::BeginPlay ();
   SetCheckClosestPickupEnabled (true);
}

void URItemPickupMgrComponent::EndPlay (const EEndPlayReason::Type EndPlayReason)
{
   SetCheckClosestPickupEnabled (false);
   Super::EndPlay (EndPlayReason);
}

void URItemPickupMgrComponent::SetCheckClosestPickupEnabled (bool Enabled)
{
   if (Enabled) {
      RTIMER_START (CheckClosestPickupHandle,
                    this, &URItemPickupMgrComponent::CheckClosestPickup, 
                    CheckClosestDelay, true);
   } else {
      RTIMER_STOP (CheckClosestPickupHandle, this);
   }
}

//=============================================================================
//                 Pickup
//=============================================================================

bool URItemPickupMgrComponent::Pickup_Register (const ARItemPickup* Pickup)
{
   if (!ensure (Pickup)) return false;
   CurrentPickups.Add (Pickup);
   ReportPickupListUpdated ();
   return true;
}

bool URItemPickupMgrComponent::Pickup_Unregister (const ARItemPickup* Pickup)
{
   if (!ensure (Pickup)) return false;
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

   TWeakObjectPtr<const ARItemPickup> NewClosestPickup;

   TArray<TWeakObjectPtr<const ARItemPickup> > NewList;

   for (TWeakObjectPtr<const ARItemPickup> ItPickup : CurrentPickups) {
      if (!ItPickup.IsValid ()) continue;

      // If valid
      NewList.Add (ItPickup);

      // First one
      if (!NewClosestPickup.IsValid ()) {
         NewClosestPickup = ItPickup;
         continue;
      }

      if (SelectionMethod == ERClosestPickupSelectionMethod::Distance) {

         if ( FVector::Distance (MgrLoc,         ItPickup->GetActorLocation ())
            < FVector::Distance (MgrLoc, NewClosestPickup->GetActorLocation ()))
         {
            NewClosestPickup = ItPickup;
         }

      } else {

         // --- It Pickup angle
         FVector ItDir = ItPickup->GetActorLocation () - MgrLoc;
         ItDir.Normalize ();
         float ItAngle = URUtil::GetAngle (MgrDir, ItDir);

         // --- Selected Pickup Angle
         FVector CurrentDir = NewClosestPickup->GetActorLocation () - MgrLoc;
         CurrentDir.Normalize ();
         float CurrentAngle = URUtil::GetAngle (MgrDir, CurrentDir);

         // Update selection
         if (ItAngle < CurrentAngle) NewClosestPickup = ItPickup;
      }
   }

   // Check if one of the items is invalid
   if (NewList.Num () != CurrentPickups.Num ()) {
      CurrentPickups = NewList;
      ReportPickupListUpdated ();
   }

   if (NewClosestPickup != ClosestPickup) {
      ClosestPickup = NewClosestPickup;
      ReportClosestPickupUpdated ();
   }
}

