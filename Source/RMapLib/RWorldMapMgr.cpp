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

URWorldMapMgr::URWorldMapMgr ()
{
   bWantsInitializeComponent = true;
}

void URWorldMapMgr::InitializeComponent ()
{
   Super::InitializeComponent ();

   // --- Parse Table and create Map for fast search
   if (MapPointTable) {

      MapOfMapPoints.Empty ();
      FString TablePath = URUtil::GetTablePath (MapPointTable);
      FString ContextString;
      TArray<FName> RowNames = MapPointTable->GetRowNames ();
      for (const FName& ItRowName : RowNames) {
         FRMapPointInfo* ItRow = MapPointTable->FindRow<FRMapPointInfo> (ItRowName, ContextString);

         if (!ItRow) {
            R_LOG_PRINTF ("Invalid FRMapPointInfo in row [%s] table [%s]", *ItRowName.ToString (), *TablePath);
            continue;
         }

         if (ItRow->TargetClass.IsNull ()) {
            R_LOG_PRINTF ("Invalid Target Class in row [%s] table [%s]", *ItRowName.ToString (), *TablePath);
            continue;
         }

         MapOfMapPoints.Add (ItRow->TargetClass.ToString (), *ItRow);
      }
   }
}

FRMapPointInfo URWorldMapMgr::GetMapPointInfo_Actor (const AActor* TargetActor) const
{
   FRMapPointInfo Result;
   if (ensure (TargetActor)) {
      FString AbilityClassPath = TargetActor->GetClass ()->GetPathName ();
      if (MapOfMapPoints.Contains (AbilityClassPath)) {
         Result = MapOfMapPoints[AbilityClassPath];
      } else {
         R_LOG_PRINTF ("Error. [%s] MapPointInfo not found in [%s]",
                       *AbilityClassPath,
                       *URUtil::GetTablePath (MapPointTable));
      }
   }

   return Result;
}

FRMapPointInfo URWorldMapMgr::GetMapPointInfo_Class (const TSoftClassPtr<AActor> AbilityClass) const
{
   FRMapPointInfo Result;
   if (ensure (!AbilityClass.IsNull ())) {
      FString AbilityClassPath = AbilityClass.ToString ();
      if (MapOfMapPoints.Contains (AbilityClassPath)) {
         Result = MapOfMapPoints[AbilityClassPath];
      } else {
         R_LOG_PRINTF ("Error. [%s] MapPointInfo not found in [%s]",
                       *AbilityClassPath,
                       *URUtil::GetTablePath (MapPointTable));
      }
   }

   return Result;
}

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


