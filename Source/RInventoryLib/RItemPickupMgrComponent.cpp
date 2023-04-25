// Copyright 2015-2023 Vagen Ayrapetyan

#include "RItemPickupMgrComponent.h"
#include "RItemPickup.h"

#include "RUtilLib/RLog.h"
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
   FVector PlayerLoc = GetOwner ()->GetActorLocation ();

   FVector              newClosestPickupLoc;
   const ARItemPickup * newClosestPickup = nullptr;

   TArray<const ARItemPickup*> NewList;

   for (const ARItemPickup *ItPickup : CurrentPickups) {
      if (!ensure (IsValid (ItPickup))) continue;

      // If valid
      NewList.Add (ItPickup);

      FVector ItLoc = ItPickup->GetActorLocation ();

      if (!newClosestPickup) {
         newClosestPickup    = ItPickup;
         newClosestPickupLoc = ItLoc;
      }

      if (FVector::Distance (PlayerLoc, ItLoc) < FVector::Distance (PlayerLoc, newClosestPickupLoc)) {
         newClosestPickup    = ItPickup;
         newClosestPickupLoc = ItLoc;
      }
   }

   // Check if one of the items is invalid
   if (NewList.Num () != CurrentPickups.Num ()) {
      CurrentPickups = NewList;
      ReportPickupListUpdated ();
   }

   if (newClosestPickup != ClosestPickup) {
      R_LOG ("New closest pickup");
      ClosestPickup = newClosestPickup;
      ReportClosestPickupUpdated ();
   }
}

