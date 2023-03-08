// Copyright 2015-2023 Vagen Ayrapetyan

#include "RTargetableComponent.h"
#include "RTargetableMgr.h"
#include "RUtilLib/RLog.h"

void URTargetableComponent::BeginPlay ()
{
   Super::BeginPlay ();
   SetTargetable (true);
}

void URTargetableComponent::EndPlay (const EEndPlayReason::Type EndPlayReason)
{
   SetTargetable (false);
   Super::EndPlay (EndPlayReason);
}

void URTargetableComponent::SetTargetable (bool Enabled)
{
   IsTargetable = Enabled;
   if (URTargetableMgr *Mgr = URTargetableMgr::GetInstance (this)) {
      if (Enabled) Mgr->AddTarget (this);
      else         Mgr->RmTarget (this);
   }
}
