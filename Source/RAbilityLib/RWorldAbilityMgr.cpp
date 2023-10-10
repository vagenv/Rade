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
      FString TablePath = URUtilLibrary::GetTablePath (AbilityTable);
      FString ContextString;
      TArray<FName> RowNames = AbilityTable->GetRowNames ();
      for (const FName& ItRowName : RowNames) {
         FRAbilityInfo* ItRow = AbilityTable->FindRow<FRAbilityInfo> (ItRowName, ContextString);

         if (!ItRow) {
            R_LOG_PRINTF ("Invalid FRAbilityInfo in row [%s] table [%s]", *ItRowName.ToString (), *TablePath);
            continue;
         }

         if (ItRow->AbilityClass.IsNull ()) {
            R_LOG_PRINTF ("Invalid Ability Class in row [%s] table [%s]", *ItRowName.ToString (), *TablePath);
            continue;
         }

         FString AbilityClassPath = ItRow->AbilityClass.ToString ();
         MapAbility.Add (AbilityClassPath, *ItRow);
      }
   }
}

void URWorldAbilityMgr::BeginPlay ()
{
   Super::BeginPlay ();
}

FRAbilityInfo URWorldAbilityMgr::GetAbilityInfo_Object (const URAbility* Ability) const
{
   FRAbilityInfo Result;
   if (ensure (IsValid (Ability))) {
      FString AbilityClassPath = Ability->GetClass ()->GetPathName();
      if (MapAbility.Contains (AbilityClassPath)) {
         Result = MapAbility[AbilityClassPath];
      } else {
         R_LOG_PRINTF ("Error. [%s] Ability not found in [%s]",
                       *AbilityClassPath,
                       *URUtilLibrary::GetTablePath (AbilityTable));
      }
   }

   return Result;
}

FRAbilityInfo URWorldAbilityMgr::GetAbilityInfo_Class (const TSoftClassPtr<URAbility> AbilityClass) const
{
   FRAbilityInfo Result;
   if (ensure (!AbilityClass.IsNull ())) {
      FString AbilityClassPath = AbilityClass->GetPathName ();
      if (MapAbility.Contains (AbilityClassPath)) {
         Result = MapAbility[AbilityClassPath];
      } else {
         R_LOG_PRINTF ("Error. [%s] Ability not found in [%s]",
                       *AbilityClassPath,
                       *URUtilLibrary::GetTablePath (AbilityTable));
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
   if (!ensure (IsValid (Ability))) return;
   if (R_IS_VALID_WORLD) OnAddAbility.Broadcast (Ability);
}

void URWorldAbilityMgr::ReportRmAbility (URAbility* Ability)
{
   if (!ensure (IsValid (Ability))) return;
   if (R_IS_VALID_WORLD) OnRmAbility.Broadcast (Ability);
}

void URWorldAbilityMgr::ReportUseAbility (URAbility* Ability)
{
   if (!ensure (IsValid (Ability))) return;
   if (R_IS_VALID_WORLD) OnUseAbility.Broadcast (Ability);
}

