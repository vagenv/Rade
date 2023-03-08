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

   AGameModeBase *GameMode = World->GetAuthGameMode ();
   if (!ensure (GameMode)) return nullptr;

   URTargetableMgr* TargetableMgr = GameMode->FindComponentByClass<URTargetableMgr>();
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

      // Check blacklist
      if (FilterOut.Contains (ItTarget->GetOwner ())) continue;

      // Check distance
      if (FVector::Dist (ItTarget->GetComponentLocation (), Origin) > SearchDistance) continue;

      // --- Check angle
      FVector ItDir = ItTarget->GetComponentLocation () - Origin;
      ItDir.Normalize ();
      float ItAngle = (acosf (FVector::DotProduct (SearchDir, ItDir))) * (180 / 3.1415926);
      if (ItAngle > SearchAngle) continue;

      // First target
      if (!CurrentTarget) {
         CurrentTarget = ItTarget;
         continue;
      }

      // --- Check if smaller angle
      FVector CurrentDir = CurrentTarget->GetComponentLocation () - Origin;
      CurrentDir.Normalize ();
      float CurrentAngle = (acosf (FVector::DotProduct (SearchDir, CurrentDir))) * (180 / 3.1415926);

      if (ItAngle < CurrentAngle) CurrentTarget = ItTarget;
   }

   return CurrentTarget;
}
