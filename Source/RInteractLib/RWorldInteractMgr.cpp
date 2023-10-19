// Copyright 2015-2023 Vagen Ayrapetyan

#include "RWorldInteractMgr.h"
#include "RInteractComponent.h"
#include "RUtilLib/RCheck.h"
#include "RUtilLib/RUtil.h"
#include "RUtilLib/RLog.h"

//=============================================================================
//                   Static calls
//=============================================================================

URWorldInteractMgr* URWorldInteractMgr::GetInstance (const UObject* WorldContextObject)
{
   return URUtil::GetWorldInstance<URWorldInteractMgr> (WorldContextObject);
}

//=============================================================================
//                   Member calls
//=============================================================================

URWorldInteractMgr::URWorldInteractMgr ()
{
   FRichCurve* SearchAnglePointData = SearchAnglePoint.GetRichCurve ();
   SearchAnglePointData->AddKey ( 5, 0.6);
   SearchAnglePointData->AddKey (10, 0.5);
   SearchAnglePointData->AddKey (20, 0.4);
   SearchAnglePointData->AddKey (30, 0.3);
   SearchAnglePointData->AddKey (60, 0.1);
   SearchAnglePointData->AddKey (90, 0.0);

   FRichCurve* SearchDistancePointData = SearchDistancePoint.GetRichCurve ();
   SearchDistancePointData->AddKey (1000, 1.0);
   SearchDistancePointData->AddKey (2000, 0.8);
   SearchDistancePointData->AddKey (3000, 0.2);
   SearchDistancePointData->AddKey (4000, 0.1);
   SearchDistancePointData->AddKey (5000, 0.0);
}

const TArray<URInteractComponent*> URWorldInteractMgr::GetInteractList () const
{
   TArray<URInteractComponent*> Result;
   for (const auto &ItInteract : InteractList) {
      if (ItInteract.IsValid ()) Result.Add (ItInteract.Get ());
   }
   return Result;
}

bool URWorldInteractMgr::IsValidInteract (
   const URInteractComponent *Interact,
   const FVector             &PlayerLocation,
   const FVector             &CameraLocation,
   const FVector             &CameraDirection) const
{
   // Check Interact
   if (!IsValid (Interact)) return false;

   // Check if Interact is enabled
   if (!Interact->GetIsInteractable ()) return false;

   // --- Check max distance
   float Distance = FVector::Dist (Interact->GetComponentLocation (), PlayerLocation);
   if (Distance > SearchDistanceMax) return false;

   // --- Check max angle
   FVector Direction = Interact->GetComponentLocation () - CameraLocation;
   Direction.Normalize ();
   float Angle = URUtil::GetAngle (CameraDirection, Direction);
   if (Angle > SearchAngleMax) return false;

   return true;
}

float URWorldInteractMgr::GetInteractPoint (
   const URInteractComponent *Interact,
   const FVector             &PlayerLocation,
   const FVector             &CameraLocation,
   const FVector             &CameraDirection) const
{
   if (!IsValidInteract (Interact, PlayerLocation, CameraLocation, CameraDirection)) return 0;

   // --- Get values
   FVector Direction = Interact->GetComponentLocation () - CameraLocation;
   Direction.Normalize ();
   float Angle    = URUtil::GetAngle (CameraDirection, Direction);
   float Distance = FVector::Dist (Interact->GetComponentLocation (), PlayerLocation);

   // Calculate Interact priority
   float Points = URUtil::GetRuntimeFloatCurveValue (SearchAnglePoint,    Angle)
                + URUtil::GetRuntimeFloatCurveValue (SearchDistancePoint, Distance);

   //R_LOG_PRINTF ("Interact:[%s] Distance:[%.1f] Angle:[%.1f] Point:[%.1f]",
   //              *Interact->GetOwner ()->GetName (), Distance, Angle, Points);
   return Points;
}

//=============================================================================
//                   Register / Unregister
//=============================================================================

void URWorldInteractMgr::RegisterInteract (URInteractComponent* Interact)
{
   if (!ensure (Interact)) return;
   InteractList.Add (Interact);
   if (R_IS_VALID_WORLD && OnListUpdated.IsBound ()) OnListUpdated.Broadcast ();
}

void URWorldInteractMgr::UnregisterInteract (URInteractComponent* Interact)
{
   if (!ensure (Interact)) return;
   InteractList.Remove (Interact);
   if (R_IS_VALID_WORLD && OnListUpdated.IsBound ()) OnListUpdated.Broadcast ();
}

//=============================================================================
//                   Find calls
//=============================================================================

URInteractComponent* URWorldInteractMgr::Find (
   const FVector &PlayerLocation,
   const FVector &CameraLocation,
   const FVector &CameraDirection)
{
   // New Interact values
   URInteractComponent* NewInteract      = nullptr;
   float                NewInteractPoint = 0;

   // --- Iterate over available Interacts
   for (URInteractComponent* ItInteract : GetInteractList ()) {

      // Calculate Interact priority
      float ItPoint = GetInteractPoint (ItInteract, PlayerLocation, CameraLocation, CameraDirection);
      if (ItPoint < 0.01) continue;

      //R_LOG_PRINTF ("Interact:[%s] Point:[%.1f]",
      //              *ItInteract->GetOwner ()->GetName (), ItPoint);

      // --- Check if better Interact
      if (ItPoint > NewInteractPoint) {
         NewInteract      = ItInteract;
         NewInteractPoint = ItPoint;
      }
   }

   return NewInteract;
}


