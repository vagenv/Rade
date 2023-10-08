// Copyright 2015-2023 Vagen Ayrapetyan

#include "RWorldTargetMgr.h"
#include "RTargetComponent.h"
#include "RTargetingComponent.h"
#include "RUtilLib/RCheck.h"
#include "RUtilLib/RUtil.h"
#include "RUtilLib/RLog.h"

//=============================================================================
//                   Static calls
//=============================================================================

URWorldTargetMgr* URWorldTargetMgr::GetInstance (const UObject* WorldContextObject)
{
   return URUtil::GetWorldInstance<URWorldTargetMgr> (WorldContextObject);
}

//=============================================================================
//                   Member calls
//=============================================================================

URWorldTargetMgr::URWorldTargetMgr ()
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

   FRichCurve* SearchScreenDotPointData = SearchScreenDotPoint.GetRichCurve ();
   SearchScreenDotPointData->AddKey (1.0, 1.0);
   SearchScreenDotPointData->AddKey (0.9, 1.0);
   SearchScreenDotPointData->AddKey (0.8, 0.8);
   SearchScreenDotPointData->AddKey (0.2, 0.2);
   SearchScreenDotPointData->AddKey (0.1, 0.1);
   SearchScreenDotPointData->AddKey (0.0, 0.0);

   FRichCurve* SearchScreenDistancePointData = SearchScreenDistancePoint.GetRichCurve ();
   SearchScreenDistancePointData->AddKey (0.0, 1.0);
   SearchScreenDistancePointData->AddKey (0.1, 0.4);
   SearchScreenDistancePointData->AddKey (0.2, 0.3);
   SearchScreenDistancePointData->AddKey (0.3, 0.2);
   SearchScreenDistancePointData->AddKey (0.4, 0.1);
   SearchScreenDistancePointData->AddKey (0.5, 0.0);
}

const TArray<URTargetComponent*> URWorldTargetMgr::GetTargetList () const
{
   return TargetList;
}


bool URWorldTargetMgr::IsValidTarget (
   const URTargetComponent*          Target,
   const FVector                    &SearchOrigin,
   const FVector                    &SearchDirection,
   const TArray<AActor*>            &ExcludeActors,
   const TArray<URTargetComponent*> &ExcludeTargets) const
{
   // Check target
   if (!IsValid (Target)) return false;

   // Check if target is enabled
   if (!Target->GetIsTargetable ()) return false;

   // Check if excluded
   if (ExcludeActors.Contains (Target->GetOwner ())) return false;

   // Check if excluded
   if (ExcludeTargets.Contains (Target)) return false;

   // --- Check max distance
   float Distance = FVector::Dist (Target->GetComponentLocation (), SearchOrigin);
   if (Distance > SearchDistanceMax) return false;

   // --- Check max angle
   FVector Direction = Target->GetComponentLocation () - SearchOrigin;
   Direction.Normalize ();
   float Angle = URUtilLibrary::GetAngle (SearchDirection, Direction);
   if (Angle > SearchAngleMax) return false;

   return true;
}


float URWorldTargetMgr::GetTargetPoint (
   const URTargetComponent*          Target,
   const FVector                    &SearchOrigin,
   const FVector                    &SearchDirection,
   const TArray<AActor*>            &ExcludeActors,
   const TArray<URTargetComponent*> &ExcludeTargets) const
{
   if (!IsValidTarget (Target, SearchOrigin, SearchDirection, ExcludeActors, ExcludeTargets)) return 0;

   // --- Get values
   FVector Direction = Target->GetComponentLocation () - SearchOrigin;
   Direction.Normalize ();
   float Angle    = URUtilLibrary::GetAngle (SearchDirection, Direction);
   float Distance = FVector::Dist (Target->GetComponentLocation (), SearchOrigin);

   // Calculate target priority
   float Points = URUtilLibrary::GetRuntimeFloatCurveValue (SearchAnglePoint,    Angle)
                + URUtilLibrary::GetRuntimeFloatCurveValue (SearchDistancePoint, Distance);

   //R_LOG_PRINTF ("Target:[%s] Distance:[%.1f] Angle:[%.1f] Point:[%.1f]",
   //              *Target->GetOwner ()->GetName (), Distance, Angle, Points);
   return Points;
}

//=============================================================================
//                   Register / Unregister
//=============================================================================

void URWorldTargetMgr::RegisterTarget (URTargetComponent* Target)
{
   if (!ensure (IsValid (Target))) return;
   TargetList.Add (Target);
   if (R_IS_VALID_WORLD && OnListUpdated.IsBound ()) OnListUpdated.Broadcast ();
}

void URWorldTargetMgr::UnregisterTarget (URTargetComponent* Target)
{
   if (!ensure (IsValid (Target))) return;
   TargetList.Remove (Target);
   if (R_IS_VALID_WORLD && OnListUpdated.IsBound ()) OnListUpdated.Broadcast ();
}

//=============================================================================
//                   Find calls
//=============================================================================

URTargetComponent* URWorldTargetMgr::Find_Direction (
   const FVector                    &SearchOrigin,
   const FVector                    &SearchDirection,
   const TArray<AActor*>            &ExcludeActors,
   const TArray<URTargetComponent*> &ExcludeTargets)
{
   // New Target values
   URTargetComponent* NewTarget      = nullptr;
   float              NewTargetPoint = 0;

   // --- Iterate over available Targets
   for (URTargetComponent* ItTarget : GetTargetList ()) {

      // Calculate target priority
      float ItPoint = GetTargetPoint (ItTarget, SearchOrigin, SearchDirection, ExcludeActors, ExcludeTargets);
      if (ItPoint < 0.01) continue;

      //R_LOG_PRINTF ("Target:[%s] Point:[%.1f]",
      //              *ItTarget->GetOwner ()->GetName (), ItPoint);

      // --- Check if better target
      if (ItPoint > NewTargetPoint) {
         NewTarget      = ItTarget;
         NewTargetPoint = ItPoint;
      }
   }

   return NewTarget;
}

URTargetComponent* URWorldTargetMgr::Find_Target (
   const URTargetingComponent*       Targeter,
   const TArray<AActor*>            &ExcludeActors,
   const TArray<URTargetComponent*> &ExcludeTargets)
{
   if (!ensure (IsValid (Targeter))) return nullptr;

   // Camera location and look direction
   FVector  Location = Targeter->GetComponentLocation ();
   FRotator Rotation = Targeter->GetComponentRotation ();
   FVector ForwardDir = Rotation.Vector ();
   ForwardDir.Normalize ();

   // Can't target itself
   TArray<AActor*> UpdatedExcludeActors = ExcludeActors;
   UpdatedExcludeActors.Add (Targeter->GetOwner ());

   return Find_Direction (Location, ForwardDir, UpdatedExcludeActors, ExcludeTargets);
}

URTargetComponent* URWorldTargetMgr::Find_Screen (
   const URTargetingComponent*       Targeter,
   FVector2D                         InputVector,
   const TArray<AActor*>            &ExcludeActors,
   const TArray<URTargetComponent*> &ExcludeTargets)
{
   if (!ensure (IsValid (Targeter))) return nullptr;

   FVector  SearchOrigin = Targeter->GetComponentLocation ();
   FRotator Rotation     = Targeter->GetComponentRotation ();
   InputVector.Normalize ();

   // Camera look direction
   FVector SearchDirection = Rotation.Vector ();
   SearchDirection.Normalize ();

   FVector2D TargetScreenLocation;

   // New Target values
   URTargetComponent* NewTarget = nullptr;
   float              NewPoint  = 0;

   // Local player controller
   APlayerController* PlayerController = GetWorld ()->GetFirstPlayerController ();

   if (!PlayerController) return nullptr;

   // Get Screen size
   int32 ViewportSizeX, ViewportSizeY;
   PlayerController->GetViewportSize (ViewportSizeX, ViewportSizeY);
   FVector2D ScreenCenter = FVector2D (ViewportSizeX / 2, ViewportSizeY / 2);

   float ScreenMaxDistance = FMath::Sqrt ( FMath::Square<float> (ViewportSizeX)
                                         + FMath::Square<float> (ViewportSizeY));
   
   // Get current target screen position
   bool bPlayerViewportRelative = false;

   // --- Iterate over available Targets
   for (URTargetComponent* ItTarget : GetTargetList ()) {

      if (!IsValidTarget (ItTarget, SearchOrigin, SearchDirection, ExcludeActors, ExcludeTargets)) continue;

      // Can't target the owner
      if (Targeter->GetOwner () == ItTarget->GetOwner ()) continue;


      FVector2D ItScreenLocation;
      if (!UGameplayStatics::ProjectWorldToScreen (PlayerController, 
                                                   ItTarget->GetComponentLocation (), 
                                                   ItScreenLocation,
                                                   bPlayerViewportRelative))
      {
         continue;
      }

      // Get target direction from the center of the screen
      FVector2D ItScreenDir = ItScreenLocation - ScreenCenter;
      ItScreenDir.Normalize ();

      // Get Dot value between user input and target direction from the center of the screen
      float ItDot = FVector2D::DotProduct (ItScreenDir, InputVector);

      // Wrong direction
      if (ItDot <= 0) continue;

      // Distance normaized
      float ItDistance = FVector2D::Distance (ItScreenLocation, ScreenCenter) / ScreenMaxDistance;

      // Calculate points
      float ItPoint = URUtilLibrary::GetRuntimeFloatCurveValue (SearchScreenDotPoint,      ItDot)
                    + URUtilLibrary::GetRuntimeFloatCurveValue (SearchScreenDistancePoint, ItDistance);

      //R_LOG_PRINTF ("Target:[%s] Dot:[%.2f] Distance:[%.2f] Point:[%.2f]",
      //              *(ItTarget->GetOwner ()->GetName ()), ItDot, ItDistance, ItPoint);

      if (ItPoint > NewPoint) {
         NewPoint  = ItPoint;
         NewTarget = ItTarget;
      }
   }

   return NewTarget;
}

