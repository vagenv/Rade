// Copyright 2015-2023 Vagen Ayrapetyan

#include "RWorldMapMgr.h"
#include "RMapPointComponent.h"
#include "RUtilLib/RCheck.h"
#include "RUtilLib/RUtil.h"
#include "RUtilLib/RLog.h"

//=============================================================================
//                   Static calls
//=============================================================================

URWorldMapMgr* URWorldMapMgr::GetInstance (const UObject* WorldContextObject)
{
   return URUtil::GetWorldInstance<URWorldMapMgr> (WorldContextObject);
}

//=============================================================================
//                   Member calls
//=============================================================================

TArray<URMapPointComponent*> URWorldMapMgr::GetMapPointList () const
{
   TArray<URMapPointComponent*> Result;
   for (const TWeakObjectPtr<URMapPointComponent> &ItInteract : MapPointList) {
      if (ItInteract.IsValid ()) Result.Add (ItInteract.Get ());
   }
   return Result;
}

//=============================================================================
//                   Register / Unregister
//=============================================================================

void URWorldMapMgr::RegisterMapPoint (URMapPointComponent* Interact)
{
   if (!ensure (Interact)) return;
   MapPointList.Add (Interact);
   if (R_IS_VALID_WORLD && OnListUpdated.IsBound ()) OnListUpdated.Broadcast ();
}

void URWorldMapMgr::UnregisterMapPoint (URMapPointComponent* Interact)
{
   if (!ensure (Interact)) return;
   MapPointList.Remove (Interact);
   if (R_IS_VALID_WORLD && OnListUpdated.IsBound ()) OnListUpdated.Broadcast ();
}


