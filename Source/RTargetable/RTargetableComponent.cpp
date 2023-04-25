// Copyright 2015-2023 Vagen Ayrapetyan

#include "RTargetableComponent.h"
#include "RWorldTargetMgr.h"
#include "RUtilLib/RCheck.h"
#include "RUtilLib/RLog.h"

void URTargetableComponent::BeginPlay ()
{
   Super::BeginPlay ();
   if (URWorldTargetMgr *Mgr = URWorldTargetMgr::GetInstance (this)) {
      Mgr->AddTarget (this);
   }
}

void URTargetableComponent::EndPlay (const EEndPlayReason::Type EndPlayReason)
{
   if (URWorldTargetMgr *Mgr = URWorldTargetMgr::GetInstance (this)) {
      Mgr->RmTarget (this);
   }
   Super::EndPlay (EndPlayReason);
}

//=============================================================================
//         Can this target be selected
//=============================================================================

void URTargetableComponent::SetIsTargetable (bool CanFind)
{
   IsTargetable = CanFind;
   if (R_IS_VALID_WORLD && OnIsTargetable.IsBound ()) OnIsTargetable.Broadcast ();
}

bool URTargetableComponent::GetIsTargetable () const
{
   return IsTargetable;
}

//=============================================================================
//         Is this target be selected
//=============================================================================

void URTargetableComponent::SetIsTargeted (bool CanTarget)
{
   IsTargeted = CanTarget;
   if (R_IS_VALID_WORLD && OnIsTargeted.IsBound ()) OnIsTargeted.Broadcast ();
}

bool URTargetableComponent::GetIsTargeted () const
{
   return IsTargeted;
}

