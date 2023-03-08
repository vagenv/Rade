// Copyright 2015-2023 Vagen Ayrapetyan

#include "RTargetableComponent.h"
#include "RTargetableMgr.h"
#include "RUtilLib/RLog.h"

void URTargetableComponent::BeginPlay ()
{
   Super::BeginPlay ();
   if (URTargetableMgr *Mgr = URTargetableMgr::GetInstance (this)) {
      Mgr->AddTarget (this);
   }
}

void URTargetableComponent::EndPlay (const EEndPlayReason::Type EndPlayReason)
{
   if (URTargetableMgr *Mgr = URTargetableMgr::GetInstance (this)) {
      Mgr->RmTarget (this);
   }
   Super::EndPlay (EndPlayReason);
}

