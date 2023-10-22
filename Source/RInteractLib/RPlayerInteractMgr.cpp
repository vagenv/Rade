// Copyright 2015-2023 Vagen Ayrapetyan

#include "RPlayerInteractMgr.h"
#include "RInteractComponent.h"
#include "RWorldInteractMgr.h"
#include "RUtilLib/RUtil.h"
#include "RUtilLib/RCheck.h"
#include "RUtilLib/RLog.h"
#include "RUtilLib/RTimer.h"

#include "Net/UnrealNetwork.h"

//=============================================================================
//                         Core
//=============================================================================

URPlayerInteractMgr::URPlayerInteractMgr ()
{
   PrimaryComponentTick.bCanEverTick = false;
}

void URPlayerInteractMgr::BeginPlay ()
{
   Super::BeginPlay ();
   FindWorldInteractMgr ();
}

void URPlayerInteractMgr::EndPlay (const EEndPlayReason::Type EndPlayReason)
{
   RTIMER_STOP (UpdateInteractListHandle, this);
   RTIMER_STOP (UpdateInteractCurrentHandle, this);
   Super::EndPlay (EndPlayReason);
}

//=============================================================================
//                         Init
//=============================================================================

void URPlayerInteractMgr::FindWorldInteractMgr ()
{
   AActor* LocalPlayer = URUtil::GetLocalRadePlayer (this);

   WorldInteractMgr = URWorldInteractMgr::GetInstance (this);
   if (!WorldInteractMgr.IsValid () || !LocalPlayer) {
      FTimerHandle RetryHandle;
      RTIMER_START (RetryHandle,
                    this, &URPlayerInteractMgr::FindWorldInteractMgr,
                    1, false);
      return;
   }

   if (LocalPlayer != GetOwner ()) {
      return;
   }


   RTIMER_START (UpdateInteractListHandle,
                 this, &URPlayerInteractMgr::UpdateInteractList,
                 UpdateInteractListFrequency,
                 true);

   RTIMER_START (UpdateInteractCurrentHandle,
                 this, &URPlayerInteractMgr::UpdateInteractCurrent,
                 UpdateInteractCurrentFrequency,
                 true);

   // First update is instant
   UpdateInteractList ();
   UpdateInteractCurrent ();
}

//=============================================================================
//                         Current Interact
//=============================================================================

bool URPlayerInteractMgr::IsInteracting () const
{
   return (InteractCurrent.IsValid ());
}

URInteractComponent* URPlayerInteractMgr::GetCurrentInteract () const
{
   return InteractCurrent.IsValid () ? InteractCurrent.Get () : nullptr;
}

void URPlayerInteractMgr::UpdateInteractCurrent ()
{
   if (!WorldInteractMgr.IsValid ()) return;

   // Search for new interact target
   URInteractComponent* Closest = FindClosestInteract ();

   // Same interact target
   if (Closest == InteractCurrent) {
      return;
   }

   // Notify the old Interact
   if (InteractCurrent.IsValid ()) {
      InteractCurrent->SetIsInteracted (false);
   }

   // Reset the state
   InteractCurrent = nullptr;

   // Set new Interact and notify it
   if (IsValid (Closest)) {
      InteractCurrent = Closest;
      InteractCurrent->SetIsInteracted (true);

      // Force InteractCheck if target actor was destroyed
      //FActorDestroyedSignature OnTargetDestroy;
      //InteractCurrent->GetOwner ()->OnDestroyed;
   }

   // Report
   ReportInteractCurrentUpdated ();

   // Notify server instance
   if (R_IS_NET_CLIENT) SetInteractCurrent_Server (InteractCurrent.Get ());
}

void URPlayerInteractMgr::ReportInteractCurrentUpdated () const
{
   if (R_IS_VALID_WORLD && OnCurrentInteractUpdated.IsBound ()) OnCurrentInteractUpdated.Broadcast ();
}

void URPlayerInteractMgr::SetInteractCurrent_Server_Implementation (URInteractComponent* NewInteract)
{
   InteractCurrent = NewInteract;
   ReportInteractCurrentUpdated ();
}

//=============================================================================
//                         Interact list
//=============================================================================

TArray<URInteractComponent*> URPlayerInteractMgr::GetInteractList () const
{
   return InteractList;
}

void URPlayerInteractMgr::UpdateInteractList ()
{
   if (!WorldInteractMgr.IsValid ()) return;
   InteractList = WorldInteractMgr->Find (GetOwner ()->GetActorLocation (), UpdateInteractListDistance);
}

//=============================================================================
//                         Search
//=============================================================================

URInteractComponent* URPlayerInteractMgr::FindClosestInteract () const
{
   URInteractComponent* Closest      = nullptr;
   float                ClosestAngle = CurrentAngleMax; // Minimum

   FVector  PlayerLocation  = GetOwner ()->GetActorLocation ();
   FVector  CameraLocation  = GetComponentLocation ();
   FRotator CameraRotation  = GetComponentRotation ();
   FVector  CameraDirection = CameraRotation.Vector ();
   CameraDirection.Normalize ();

   for (const auto &ItInteract : GetInteractList ()) {
      // Check Interact
      if (!IsValid (ItInteract)) continue;
      if (!ItInteract->GetIsInteractable ()) continue;

      FVector ItLocation  = ItInteract->GetComponentLocation ();
      FVector ItDirection = ItLocation - CameraLocation;
      ItDirection.Normalize ();
      float ItAngle    = URUtil::GetAngle  (CameraDirection, ItDirection);
      float ItDistance = FVector::Distance (PlayerLocation,  ItLocation);

      if (ItDistance > CurrentDistanceMax) continue;
      if (ItAngle > ClosestAngle) continue;

      Closest      = ItInteract;
      ClosestAngle = ItAngle;
   }

   return Closest;
}

