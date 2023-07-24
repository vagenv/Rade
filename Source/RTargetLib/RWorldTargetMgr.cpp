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
}

void URWorldTargetMgr::AddTarget (URTargetComponent* Target)
{
   if (!ensure (IsValid (Target))) return;
   TargetableList.Add (Target);
   if (R_IS_VALID_WORLD && OnListUpdated.IsBound ()) OnListUpdated.Broadcast ();
}

void URWorldTargetMgr::RmTarget  (URTargetComponent* Target)
{
   if (!ensure (IsValid (Target))) return;
   TargetableList.Remove (Target);
   if (R_IS_VALID_WORLD && OnListUpdated.IsBound ()) OnListUpdated.Broadcast ();
}

URTargetComponent* URWorldTargetMgr::Find (URTargetingComponent*      Targeter,
                                           TArray<AActor*>            FilterOutActors,
                                           TArray<URTargetComponent*> FilterOutTargets)
{
   if (!ensure (IsValid (Targeter))) return nullptr;
   FVector  Origin    = Targeter->GetComponentLocation ();
   FRotator Direction = Targeter->GetComponentRotation ();

   // Camera look direction
   FVector SearchDir = Direction.Vector ();
   SearchDir.Normalize ();
   URTargetComponent* CurrentTarget = nullptr;

   // --- Iterate over available Targets
   for (URTargetComponent* ItTarget : TargetableList) {
      if (!IsValid (ItTarget)) continue;
      if (!ItTarget->GetIsTargetable ()) continue;

      // Check blacklist
      if (FilterOutActors.Contains (ItTarget->GetOwner ())) continue;

      // Check blacklist
      if (FilterOutTargets.Contains (ItTarget)) continue;

      // Check distance
      if (FVector::Dist (ItTarget->GetComponentLocation (), Origin) > SearchDistance) continue;

      // --- Check angle
      FVector ItDir = ItTarget->GetComponentLocation () - Origin;
      ItDir.Normalize ();
      float ItAngle = URUtilLibrary::GetAngle (SearchDir, ItDir);
      if (ItAngle > SearchAngle) continue;

      // First target
      if (!IsValid (CurrentTarget)) {
         CurrentTarget = ItTarget;
         continue;
      }

      // --- Check if smaller angle
      FVector CurrentDir = CurrentTarget->GetComponentLocation () - Origin;
      CurrentDir.Normalize ();
      float CurrentAngle = URUtilLibrary::GetAngle (SearchDir, CurrentDir);

      if (ItAngle < CurrentAngle) CurrentTarget = ItTarget;
   }

   return CurrentTarget;
}

URTargetComponent* URWorldTargetMgr::FindNear (URTargetingComponent*      Targeter,
                                               URTargetComponent*         CurrentTarget,
                                               float                      InputOffsetX,
                                               float                      InputOffsetY,
                                               TArray<AActor*>            FilterOutActors,
                                               TArray<URTargetComponent*> FilterOutTargets)
{
   if (!ensure (IsValid (Targeter)))      return nullptr;
   if (!ensure (IsValid (CurrentTarget))) return nullptr;
   FVector Origin     = Targeter->GetComponentLocation ();
   FVector ForwardDir = Targeter->GetForwardVector ();
   FVector RightDir   = Targeter->GetRightVector ();
   FVector UpDir      = Targeter->GetUpVector ();
   FVector TargetLoc  = CurrentTarget->GetComponentLocation ();

   // Direction of search
   FVector InputDir = UpDir    * InputOffsetY * -1 * InputVWeight // Reverse vertical offset
                    + RightDir * InputOffsetX *      InputHWeight;
   InputDir.Normalize ();

   //DrawDebugLine (GetWorld (), TargetLoc, TargetLoc + InputDir * 500, FColor::Green, false, 5, 0, 5);

   // Minimal acceptable new target dot / angle
   float MinDot = 0.3;

   // New Target values
   URTargetComponent* NewTarget         = nullptr;
   float              NewTargetValue    = 0;
   float              NewTargetDot      = 0;
   float              NewTargetDistance = 0;

   // --- Iterate over available Targets
   for (URTargetComponent* ItTarget : TargetableList) {
      if (!IsValid (ItTarget)) continue;
      if (!ItTarget->GetIsTargetable ()) continue;

      // Check blacklist
      if (FilterOutActors.Contains (ItTarget->GetOwner ())) continue;

      // Check blacklist
      if (FilterOutTargets.Contains (ItTarget)) continue;

      // Check distance
      if (FVector::Dist (ItTarget->GetComponentLocation (), Origin) > SearchDistance) continue;

      // --- Check angle respect to camera
      FVector ItDir = ItTarget->GetComponentLocation () - Origin;
      ItDir.Normalize ();
      if (URUtilLibrary::GetAngle (ForwardDir, ItDir) > SearchAngle) continue;

      // Direction with respect to current target
      ItDir = ItTarget->GetComponentLocation () - TargetLoc;
      ItDir.Normalize ();
      float ItDot      = FVector::DotProduct (InputDir, ItDir);
      float ItDistance = FVector::Dist (ItTarget->GetComponentLocation (), TargetLoc);

      // DrawDebugLine (GetWorld (), TargetLoc, TargetLoc + ItDir * 500, FColor::Red, false, 5, 0, 5);
      // R_LOG_PRINTF ("Target:[%s] Distance:[%.1f] Dot:[%.1f]",
      //               *(ItTarget->GetOwner ()->GetName ()),
      //               ItDistance,
      //               ItDot);

      // Wrong direction
      if (ItDot <= MinDot) continue;

      if (NewTarget) {

         // Angle is more important than distance.
         if ( NewTargetDot / FMath::Sqrt (NewTargetDistance)
            > ItDot        / FMath::Sqrt (ItDistance)) continue;
      }

      NewTarget         = ItTarget;
      NewTargetDot      = ItDot;
      NewTargetDistance = ItDistance;
   }

   return NewTarget;
}

