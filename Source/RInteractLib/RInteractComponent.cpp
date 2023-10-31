// Copyright 2015-2023 Vagen Ayrapetyan

#include "RInteractComponent.h"
#include "RWorldInteractMgr.h"
#include "RUtilLib/RUtil.h"
#include "RUtilLib/RLog.h"
#include "RUtilLib/RTimer.h"

void URInteractComponent::BeginPlay ()
{
   Super::BeginPlay ();
   RegisterInteract ();
}

void URInteractComponent::EndPlay (const EEndPlayReason::Type EndPlayReason)
{
   UnregisterInteract ();
   Super::EndPlay (EndPlayReason);
}

void URInteractComponent::RegisterInteract ()
{
   if (URWorldInteractMgr *WorldInteractMgr = URWorldInteractMgr::GetInstance (this)) {
      WorldInteractMgr->RegisterInteract (this);
   } else {
      FTimerHandle RetryHandle;
      RTIMER_START (RetryHandle,
                    this, &URInteractComponent::RegisterInteract,
                    1, false);
   }
}
void URInteractComponent::UnregisterInteract ()
{
   if (URWorldInteractMgr *WorldInteractMgr = URWorldInteractMgr::GetInstance (this)) {
      WorldInteractMgr->UnregisterInteract (this);
   }
}

//=============================================================================
//         Can this Interact be selected
//=============================================================================

void URInteractComponent::SetCanInteract (bool CanInteract_)
{
   if (CanInteract == CanInteract_) return;
   CanInteract = CanInteract_;
   if (R_IS_VALID_WORLD && OnIsInteractableUpdated.IsBound ()) OnIsInteractableUpdated.Broadcast ();
}

bool URInteractComponent::GetCanInteract () const
{
   return CanInteract;
}

//=============================================================================
//         Is this Interact selected by someone
//=============================================================================

void URInteractComponent::SetIsInteracted (bool IsInteracted_)
{
   if (IsInteracted == IsInteracted_) return;
   IsInteracted = IsInteracted_;
   if (R_IS_VALID_WORLD && OnIsInteractedUpdated.IsBound ()) OnIsInteractedUpdated.Broadcast ();
}

bool URInteractComponent::GetIsInteracted () const
{
   return IsInteracted;
}

