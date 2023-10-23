// Copyright 2015-2023 Vagen Ayrapetyan

#include "RWorldInteractMgr.h"
#include "RInteractComponent.h"
#include "RUtilLib/RCheck.h"
#include "RUtilLib/RUtil.h"
#include "RUtilLib/RLog.h"

//=============================================================================
//                   Static calls
//=============================================================================

URWorldInteractMgr* URWorldInteractMgr::GetInstance (const UObject* WorldContextObject)
{
   return URUtil::GetWorldInstance<URWorldInteractMgr> (WorldContextObject);
}

//=============================================================================
//                   Member calls
//=============================================================================

TArray<URInteractComponent*> URWorldInteractMgr::GetInteractList () const
{
   TArray<URInteractComponent*> Result;
   for (const TWeakObjectPtr<URInteractComponent> &ItInteract : InteractList) {
      if (ItInteract.IsValid ()) Result.Add (ItInteract.Get ());
   }
   return Result;
}

//=============================================================================
//                   Register / Unregister
//=============================================================================

void URWorldInteractMgr::RegisterInteract (URInteractComponent* Interact)
{
   if (!ensure (Interact)) return;
   InteractList.Add (Interact);
   if (R_IS_VALID_WORLD && OnListUpdated.IsBound ()) OnListUpdated.Broadcast ();
}

void URWorldInteractMgr::UnregisterInteract (URInteractComponent* Interact)
{
   if (!ensure (Interact)) return;
   InteractList.Remove (Interact);
   if (R_IS_VALID_WORLD && OnListUpdated.IsBound ()) OnListUpdated.Broadcast ();
}

//=============================================================================
//                   Find calls
//=============================================================================

TArray<URInteractComponent*> URWorldInteractMgr::Find (const FVector &Location, float Distance) const
{
   TArray<URInteractComponent*> Result;

   // --- Iterate over available Interacts
   for (const TWeakObjectPtr<URInteractComponent> &ItInteract : InteractList) {
      if (!ItInteract.IsValid ()) continue;

      float ItDistance = FVector::Distance (Location, ItInteract->GetComponentLocation ());
      if (ItDistance > Distance) continue;
      Result.Add (ItInteract.Get ());
   }

   return Result;
}


