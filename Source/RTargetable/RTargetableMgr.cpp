// Copyright 2015-2023 Vagen Ayrapetyan

#include "RTargetableMgr.h"
#include "RTargetableComponent.h"
#include "RTargetingComponent.h"
#include "RUtilLib/RLog.h"

#include "DrawDebugHelpers.h"

//=============================================================================
//                   Static calls
//=============================================================================

URTargetableMgr* URTargetableMgr::GetInstance (UObject* WorldContextObject)
{
   if (!ensure (WorldContextObject)) return nullptr;

   UWorld *World = WorldContextObject->GetWorld ();
   if (!ensure (World)) return nullptr;

   AGameStateBase *GameState = World->GetGameState ();
   if (!ensure (GameState)) return nullptr;

   URTargetableMgr* TargetableMgr = GameState->FindComponentByClass<URTargetableMgr>();
   return TargetableMgr;
}

//=============================================================================
//                   Member calls
//=============================================================================

URTargetableMgr::URTargetableMgr ()
{
}

void URTargetableMgr::AddTarget (URTargetableComponent * Target)
{
   if (!ensure (Target)) return;
   TargetableList.Add (Target);
   OnListUpdated.Broadcast ();
}

void URTargetableMgr::RmTarget  (URTargetableComponent * Target)
{
   if (!ensure (Target)) return;
   TargetableList.Remove (Target);
   OnListUpdated.Broadcast ();
}

URTargetableComponent* URTargetableMgr::Find (URTargetingComponent*          Targeter,
                                              TArray<AActor*>                FilterOutActors,
                                              TArray<URTargetableComponent*> FilterOutTargets)
{
   if (!ensure (Targeter)) return nullptr;
   FVector  Origin    = Targeter->GetComponentLocation ();
   FRotator Direction = Targeter->GetComponentRotation ();

   // Camera look direction
   FVector SearchDir = Direction.Vector ();
   SearchDir.Normalize ();
   URTargetableComponent* CurrentTarget = nullptr;

   // --- Iterate over available Targets
   for (URTargetableComponent* ItTarget : TargetableList) {
      if (!ItTarget) continue;
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
      float ItAngle = URTargetableMgr::GetAngle (SearchDir, ItDir);
      if (ItAngle > SearchAngle) continue;

      // First target
      if (!CurrentTarget) {
         CurrentTarget = ItTarget;
         continue;
      }

      // --- Check if smaller angle
      FVector CurrentDir = CurrentTarget->GetComponentLocation () - Origin;
      CurrentDir.Normalize ();
      float CurrentAngle = URTargetableMgr::GetAngle (SearchDir, CurrentDir);

      if (ItAngle < CurrentAngle) CurrentTarget = ItTarget;
   }

   return CurrentTarget;
}

URTargetableComponent* URTargetableMgr::FindNear (URTargetingComponent*          Targeter,
                                                  URTargetableComponent*         CurrentTarget,
                                                  float                          InputOffsetX,
                                                  float                          InputOffsetY,
                                                  TArray<AActor*>                FilterOutActors,
                                                  TArray<URTargetableComponent*> FilterOutTargets)
{
   if (!ensure (Targeter))      return nullptr;
   if (!ensure (CurrentTarget)) return nullptr;
   FVector Origin     = Targeter->GetComponentLocation ();
   FVector ForwardDir = Targeter->GetForwardVector ();
   FVector RightDir   = Targeter->GetRightVector ();
   FVector UpDir      = Targeter->GetUpVector ();
   FVector TargetLoc  = CurrentTarget->GetComponentLocation ();

   // Direction of search
   FVector InputDir = UpDir    * InputOffsetY * -1 *InputVWeight // Reverse vertical offset
                    + RightDir * InputOffsetX *     InputHWeight;
   InputDir.Normalize ();

   //DrawDebugLine (GetWorld (), TargetLoc, TargetLoc + InputDir * 500, FColor::Green, false, 5, 0, 5);

   // Minimal acceptable new target dot / angle
   float MinDot = 0.3;

   // New Target values
   URTargetableComponent* NewTarget         = nullptr;
   float                  NewTargetValue    = 0;
   float                  NewTargetDot      = 0;
   float                  NewTargetDistance = 0;

   // --- Iterate over available Targets
   for (URTargetableComponent* ItTarget : TargetableList) {
      if (!ItTarget) continue;
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
      if (URTargetableMgr::GetAngle (ForwardDir, ItDir) > SearchAngle) continue;

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

float URTargetableMgr::GetAngle (FVector v1, FVector v2)
{
   v1.Normalize ();
   v2.Normalize ();
   return (acosf (FVector::DotProduct (v1, v2))) * (180 / 3.1415926);
}
