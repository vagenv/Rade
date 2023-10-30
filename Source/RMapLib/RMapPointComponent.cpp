// Copyright 2015-2023 Vagen Ayrapetyan

#include "RMapPointComponent.h"
#include "RWorldMapMgr.h"
#include "RUtilLib/RUtil.h"
#include "RUtilLib/RLog.h"
#include "RUtilLib/RTimer.h"

void URMapPointComponent::BeginPlay ()
{
   Super::BeginPlay ();
   RegisterMapPoint ();
}

void URMapPointComponent::EndPlay (const EEndPlayReason::Type EndPlayReason)
{
   UnregisterMapPoint ();
   Super::EndPlay (EndPlayReason);
}

void URMapPointComponent::RegisterMapPoint ()
{
   if (URWorldMapMgr *WorldMapMgr = URWorldMapMgr::GetInstance (this)) {
      WorldMapMgr->RegisterMapPoint (this);
   } else {
      FTimerHandle RetryHandle;
      RTIMER_START (RetryHandle,
                    this, &URMapPointComponent::RegisterMapPoint,
                    1, false);
   }
}
void URMapPointComponent::UnregisterMapPoint ()
{
   if (URWorldMapMgr *WorldMapMgr = URWorldMapMgr::GetInstance (this)) {
      WorldMapMgr->UnregisterMapPoint (this);
   }
}

//=============================================================================
//         Can this Interact be selected
//=============================================================================

void URMapPointComponent::SetCanFind (bool CanFind_)
{
   if (CanFind == CanFind_) return;
   CanFind = CanFind_;
   if (R_IS_VALID_WORLD && OnCanFindChanged.IsBound ()) OnCanFindChanged.Broadcast ();
}

bool URMapPointComponent::GetCanFind () const
{
   return CanFind;
}

