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

FRMapPointInfo URMapPointComponent::GetMapPointInfo () const
{
   URWorldMapMgr *WorldMapMgr = URWorldMapMgr::GetInstance (this);
   if (!ensure (WorldMapMgr)) return FRMapPointInfo ();
   return WorldMapMgr->GetMapPointInfo_Actor (GetOwner ());
}

//=============================================================================
//         Is this map point visible on map
//=============================================================================

void URMapPointComponent::SetIsVisible (bool IsVisible_)
{
   if (IsVisible == IsVisible_) return;
   IsVisible = IsVisible_;
   if (IsVisible) RegisterMapPoint ();
   else           UnregisterMapPoint ();
}

bool URMapPointComponent::GetIsVisible () const
{
   return IsVisible;
}

