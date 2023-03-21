// Copyright 2015-2023 Vagen Ayrapetyan

#include "RExperienceComponent.h"
// #include "RTargetableMgr.h"
#include "RUtilLib/RLog.h"

void URExperienceComponent::BeginPlay ()
{
   Super::BeginPlay ();
   // if (URTargetableMgr *Mgr = URTargetableMgr::GetInstance (this)) {
   //    Mgr->AddTarget (this);
   // }
}

void URExperienceComponent::EndPlay (const EEndPlayReason::Type EndPlayReason)
{
   Super::EndPlay (EndPlayReason);
}
