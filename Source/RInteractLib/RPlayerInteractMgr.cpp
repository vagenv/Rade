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
   RTIMER_START (InteractCheckHandle,
                 this, &URPlayerInteractMgr::InteractCheck,
                 SearchFrequency,
                 true);
}

void URPlayerInteractMgr::EndPlay (const EEndPlayReason::Type EndPlayReason)
{
   RTIMER_STOP (InteractCheckHandle, this);
   Super::EndPlay (EndPlayReason);
}

//=============================================================================
//                         Init
//=============================================================================

void URPlayerInteractMgr::FindWorldInteractMgr ()
{
   WorldInteractMgr = URWorldInteractMgr::GetInstance (this);
   if (!WorldInteractMgr.IsValid ()) {
      FTimerHandle RetryHandle;
      RTIMER_START (RetryHandle,
                    this, &URPlayerInteractMgr::FindWorldInteractMgr,
                    1, false);
      return;
   }
}

//=============================================================================
//                         Get functions
//=============================================================================

bool URPlayerInteractMgr::IsInteracting () const
{
	return (InteractCurrent.IsValid ());
}

URInteractComponent* URPlayerInteractMgr::GetCurrentInteract () const
{
   return InteractCurrent.IsValid () ? InteractCurrent.Get () : nullptr;
}


//=============================================================================
//                         Functions
//=============================================================================

// Check if Interact is valid
void URPlayerInteractMgr::InteractCheck ()
{
   if (!WorldInteractMgr.IsValid ()) return;

   FVector  PlayerLocation   = GetOwner ()->GetActorLocation ();
   FVector  CameraLocation   = GetComponentLocation ();
   FRotator CameraRotation   = GetComponentRotation ();
   FVector  CameraForwardDir = CameraRotation.Vector ();
   CameraForwardDir.Normalize ();

   URInteractComponent* InteractNew
      = WorldInteractMgr->Find (PlayerLocation, CameraLocation, CameraForwardDir);

   // Same interact target
   if (InteractNew == InteractCurrent) return;


   // Notify the old Interact
   if (InteractCurrent.IsValid ()) {
      InteractCurrent->SetIsInteracted (false);
   }

   // Reset the state
   InteractCurrent = nullptr;

   // Set new Interact and notify it
   if (IsValid (InteractNew)) {
      InteractCurrent = InteractNew;
      InteractCurrent->SetIsInteracted (true);

      // Force InteractCheck if target actor was destroyed 
      //FActorDestroyedSignature OnTargetDestroy;
      //InteractCurrent->GetOwner ()->OnDestroyed;
   }

   // Report
   ReportInteractUpdate ();

   // Notify server instance
   if (R_IS_NET_CLIENT) SetInteractCurrent_Server (InteractNew);
}

void URPlayerInteractMgr::SetInteractCurrent_Server_Implementation (URInteractComponent* NewInteract)
{
   InteractCurrent = NewInteract;
   ReportInteractUpdate ();
}

void URPlayerInteractMgr::ReportInteractUpdate () const
{
   if (R_IS_VALID_WORLD && OnInteractUpdated.IsBound ()) OnInteractUpdated.Broadcast ();
}

