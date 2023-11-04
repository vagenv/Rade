// Copyright 2015-2023 Vagen Ayrapetyan

#include "RTargetComponent.h"
#include "RWorldTargetMgr.h"
#include "RUtilLib/RUtil.h"
#include "RUtilLib/RLog.h"
#include "RUtilLib/RTimer.h"

void URTargetComponent::BeginPlay ()
{
   Super::BeginPlay ();
   RegisterTarget ();
}

void URTargetComponent::EndPlay (const EEndPlayReason::Type EndPlayReason)
{
   UnregisterTarget ();
   Super::EndPlay (EndPlayReason);
}

void URTargetComponent::RegisterTarget ()
{
   if (URWorldTargetMgr *WorldTargetMgr = URWorldTargetMgr::GetInstance (this)) {
      WorldTargetMgr->RegisterTarget (this);
   } else {
      FTimerHandle RetryHandle;
      RTIMER_START (RetryHandle,
                    this, &URTargetComponent::RegisterTarget,
                    1, false);
   }
}
void URTargetComponent::UnregisterTarget ()
{
   if (URWorldTargetMgr *WorldTargetMgr = URWorldTargetMgr::GetInstance (this)) {
      WorldTargetMgr->UnregisterTarget (this);
   }
}

//=============================================================================
//         Can this target be selected
//=============================================================================

void URTargetComponent::SetIsTargetable (bool IsTargetable_)
{
   if (IsTargetable == IsTargetable_) return;
   IsTargetable = IsTargetable_;
   if (R_IS_VALID_WORLD && OnIsTargetable.IsBound ()) OnIsTargetable.Broadcast ();
}

bool URTargetComponent::GetIsTargetable () const
{
   return IsTargetable;
}

//=============================================================================
//         Is this target selected by someone
//=============================================================================

void URTargetComponent::SetIsTargeted (bool IsTargeted_)
{
   if (IsTargeted == IsTargeted_) return;
   IsTargeted = IsTargeted_;
   if (R_IS_VALID_WORLD && OnIsTargeted.IsBound ()) OnIsTargeted.Broadcast ();
}

bool URTargetComponent::GetIsTargeted () const
{
   return IsTargeted;
}

