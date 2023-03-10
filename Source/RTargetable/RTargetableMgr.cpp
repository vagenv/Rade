// Copyright 2015-2023 Vagen Ayrapetyan

#include "RTargetableMgr.h"
#include "RTargetableComponent.h"
#include "RUtilLib/RLog.h"

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

URTargetableComponent* URTargetableMgr::Find (FVector Origin, FRotator Direction, TArray<AActor*> FilterOut)
{
   // Camera look direction
   FVector SearchDir = Direction.Vector ();
   SearchDir.Normalize ();
   URTargetableComponent* CurrentTarget = nullptr;

   // --- Iterate over Targets
   for (URTargetableComponent* ItTarget : TargetableList) {
      if (!ItTarget) continue;
      if (!ItTarget->GetIsTargetable ()) continue;

      // Check blacklist
      if (FilterOut.Contains (ItTarget->GetOwner ())) continue;

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

float URTargetableMgr::GetAngle (const FVector v1, const FVector v2)
{
   return (acosf (FVector::DotProduct (v1, v2))) * (180 / 3.1415926);
}
