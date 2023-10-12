// Copyright 2015-2023 Vagen Ayrapetyan

#include "RTargetComponent.h"
#include "RWorldTargetMgr.h"
#include "RUtilLib/RCheck.h"
#include "RUtilLib/RLog.h"

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
   if (URWorldTargetMgr *Mgr = URWorldTargetMgr::GetInstance (this)) {
      Mgr->RegisterTarget (this);
   } else {
      FTimerHandle RetryTimer;
      GetWorld ()->GetTimerManager ().SetTimer (RetryTimer,
                                                this, &URTargetComponent::RegisterTarget,
                                                1);
   }
}
void URTargetComponent::UnregisterTarget ()
{
   if (URWorldTargetMgr *Mgr = URWorldTargetMgr::GetInstance (this)) {
      Mgr->UnregisterTarget (this);
   }
}

//=============================================================================
//         Can this target be selected
//=============================================================================

void URTargetComponent::SetIsTargetable (bool CanFind)
{
   IsTargetable = CanFind;
   if (R_IS_VALID_WORLD && OnIsTargetable.IsBound ()) OnIsTargetable.Broadcast ();
}

bool URTargetComponent::GetIsTargetable () const
{
   return IsTargetable;
}

//=============================================================================
//         Is this target selected by someone
//=============================================================================

void URTargetComponent::SetIsTargeted (bool CanTarget)
{
   IsTargeted = CanTarget;
   if (R_IS_VALID_WORLD && OnIsTargeted.IsBound ()) OnIsTargeted.Broadcast ();
}

bool URTargetComponent::GetIsTargeted () const
{
   return IsTargeted;
}

