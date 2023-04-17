// Copyright 2015-2023 Vagen Ayrapetyan

#include "RWorldAbilityMgr.h"
#include "RUtilLib/RUtil.h"
#include "RUtilLib/RCheck.h"
#include "RUtilLib/RLog.h"

//=============================================================================
//                   Static calls
//=============================================================================

URWorldAbilityMgr* URWorldAbilityMgr::GetInstance (const UObject* WorldContextObject)
{
   return URUtil::GetWorldInstance<URWorldAbilityMgr> (WorldContextObject);
}

//=============================================================================
//                   Member calls
//=============================================================================

URWorldAbilityMgr::URWorldAbilityMgr ()
{
   bWantsInitializeComponent = true;
}

void URWorldAbilityMgr::InitializeComponent ()
{
   Super::InitializeComponent ();

   // --- Parse Table and create Map for fast search
   if (AbilityTable) {
      MapAbility.Empty ();
      FString ContextString;
      TArray<FName> RowNames = AbilityTable->GetRowNames ();
      for (const FName& ItRowName : RowNames) {
         FRAbilityInfo* ItRow = AbilityTable->FindRow<FRAbilityInfo> (ItRowName, ContextString);
         if (ItRow && ItRow->AbilityClass) {
            MapAbility.Add (ItRow->AbilityClass, *ItRow);
         }
      }
   }
}

void URWorldAbilityMgr::BeginPlay ()
{
   Super::BeginPlay ();
}

FRAbilityInfo URWorldAbilityMgr::GetAbilityInfo (const URAbility* Ability) const
{
   FRAbilityInfo Result;
   if (ensure (Ability)) {
      if (MapAbility.Contains (Ability->GetClass ())) {
         Result = MapAbility[Ability->GetClass ()];
      } else {
         R_LOG_PRINTF ("Error. [%s] Ability not found in [AbilityTable]", *Ability->GetPathName ());
      }
   }

   return Result;
}

TArray<FRAbilityInfo> URWorldAbilityMgr::GetAllAbilities () const
{
   TArray<FRAbilityInfo> Result;
   for (const auto& ItAbility : MapAbility) {
      Result.Add (ItAbility.Value);
   }
   return Result;
}

void URWorldAbilityMgr::ReportAddAbility (URAbility* Ability)
{
   if (!ensure (Ability)) return;
   if (R_IS_VALID_WORLD) OnAddAbility.Broadcast (Ability);
}

void URWorldAbilityMgr::ReportRmAbility (URAbility* Ability)
{
   if (!ensure (Ability)) return;
   if (R_IS_VALID_WORLD) OnRmAbility.Broadcast (Ability);
}

void URWorldAbilityMgr::ReportUseAbility (URAbility* Ability)
{
   if (!ensure (Ability)) return;
   if (R_IS_VALID_WORLD) OnUseAbility.Broadcast (Ability);
}

