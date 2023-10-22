// Copyright 2015-2023 Vagen Ayrapetyan

#include "RAbilityMgrComponent.h"
#include "RAbility.h"

#include "RUtilLib/RUtil.h"
#include "RUtilLib/RLog.h"
#include "RUtilLib/RCheck.h"
#include "RUtilLib/RTimer.h"
#include "RUtilLib/RWorldAssetMgr.h"
#include "RExperienceLib/RExperienceMgrComponent.h"
#include "RStatusLib/RPlayerStatusMgrComponent.h"

#include "Net/UnrealNetwork.h"

//=============================================================================
//                 Core
//=============================================================================

URAbilityMgrComponent::URAbilityMgrComponent ()
{
   PrimaryComponentTick.bCanEverTick = true;
   PrimaryComponentTick.bStartWithTickEnabled = true;
   SetIsReplicatedByDefault (true);
}

// Replication
void URAbilityMgrComponent::GetLifetimeReplicatedProps (TArray<FLifetimeProperty> &OutLifetimeProps) const
{
   Super::GetLifetimeReplicatedProps (OutLifetimeProps);
   DOREPLIFETIME (URAbilityMgrComponent, AbilityPoints);
}

//=============================================================================
//                 Core
//=============================================================================

void URAbilityMgrComponent::BeginPlay ()
{
   Super::BeginPlay ();

   if (R_IS_NET_ADMIN) {
      if (URExperienceMgrComponent* ExperienceMgr = URUtil::GetComponent<URExperienceMgrComponent> (GetOwner ())) {
         ExperienceMgr->OnLevelUp.AddDynamic (this, &URAbilityMgrComponent::LeveledUp);
      }
   }

   ConnectToSaveMgr ();
}

void URAbilityMgrComponent::EndPlay (const EEndPlayReason::Type EndPlayReason)
{
   Super::EndPlay (EndPlayReason);
}

//=============================================================================
//                 Ability Points
//=============================================================================

void URAbilityMgrComponent::LeveledUp ()
{
   R_RETURN_IF_NOT_ADMIN;

   AbilityPoints++;
   ReportAbilityPointUpdated ();
}

int URAbilityMgrComponent::GetAbilityPoint () const
{
   return AbilityPoints;
}

void URAbilityMgrComponent::OnRep_Points ()
{
   ReportAbilityPointUpdated ();
}

void URAbilityMgrComponent::ReportAbilityPointUpdated ()
{
   if (R_IS_VALID_WORLD && OnAbilityPointUpdated.IsBound ()) OnAbilityPointUpdated.Broadcast ();
}

//=============================================================================
//                 Add / RM
//=============================================================================

bool URAbilityMgrComponent::CanAddAbility (const FRAbilityInfo& AbilityInfo) const
{
   if (GetAbility (AbilityInfo) != nullptr) {
      return false;
   }
   if (AbilityPoints < AbilityInfo.LearnPointPrice) return false;

   if (AbilityInfo.RequiredAbilities.Num ()) {
      // --- Get all user abilities
      TArray<URAbility*> AbilityList;
      GetOwner ()->GetComponents (AbilityList);
      TMap<FString, bool> AbilityMap;
      for (const auto &ItAbility : AbilityList) {
         AbilityMap.Add (ItAbility->GetClass ()->GetPathName (), true);
      }

      // --- Iterate through required abilities
      for (const auto &ItAbility : AbilityInfo.RequiredAbilities) {

         // No ability found on user
         if (!AbilityMap.Contains (ItAbility.ToString ())) return false;
      }
   }

   // Check stats
   if (!AbilityInfo.RequiredStats.IsEmpty ()) {
      if (URPlayerStatusMgrComponent* StatusMgr = URUtil::GetComponent<URPlayerStatusMgrComponent> (GetOwner ())) {
         if (!StatusMgr->GetCoreStats_Total ().MoreThan (AbilityInfo.RequiredStats)) return false;
      } else {
         return false;
      }
   }

   return true;
}

URAbility* URAbilityMgrComponent::GetAbility (const FRAbilityInfo& AbilityInfo) const
{
   if (!ensure (!AbilityInfo.IsEmpty ())) return nullptr;

   URAbility* Result = nullptr;
   TArray<URAbility*> AbilityList;
   GetOwner ()->GetComponents (AbilityList);
   for (URAbility* ItAbility : AbilityList) {
      if (ItAbility->GetClass () == AbilityInfo.AbilityClass) {
         Result = ItAbility;
         break;
      }
   }

   return Result;
}

bool URAbilityMgrComponent::AddAbility (const FRAbilityInfo& AbilityInfo)
{
   R_RETURN_IF_NOT_ADMIN_BOOL;
   if (!ensure (!AbilityInfo.IsEmpty ())) return false;
   if (!ensure (AbilityPoints > AbilityInfo.LearnPointPrice)) return false;
   if (!ensure (!URAbilityMgrComponent::GetAbility (AbilityInfo))) return false;

   int LearnCost = AbilityInfo.LearnPointPrice;

   URWorldAssetMgr::LoadAsync (AbilityInfo.AbilityClass.GetUniqueID (),
      this, [this, LearnCost] (UObject* LoadedContent) {
      if (UClass* AbilityClass = Cast<UClass> (LoadedContent)) {
         if (URUtil::AddComponent<URAbility> (GetOwner (), AbilityClass)) {
            AbilityPoints -= LearnCost;
            ReportAbilityPointUpdated ();
         }
      }
   });

   return true;
}

bool URAbilityMgrComponent::RmAbility (const FRAbilityInfo& AbilityInfo)
{
   R_RETURN_IF_NOT_ADMIN_BOOL;
   if (!ensure (!AbilityInfo.IsEmpty ())) return false;
   URAbility* Ability = URAbilityMgrComponent::GetAbility (AbilityInfo);
   if (!Ability) return false;

   Ability->DestroyComponent ();
   AbilityPoints += AbilityInfo.LearnPointPrice;
   ReportAbilityPointUpdated ();

   return true;
}

void URAbilityMgrComponent::ReportAbilityListUpdated ()
{
   if (R_IS_VALID_WORLD && OnAbilityListUpdated.IsBound ()) OnAbilityListUpdated.Broadcast ();
}

//=============================================================================
//                 Server versions of the functions
//=============================================================================
void URAbilityMgrComponent::AddAbility_Server_Implementation (const FRAbilityInfo& AbilityInfo)
{
   AddAbility (AbilityInfo);
}
void URAbilityMgrComponent::RmAbility_Server_Implementation (const FRAbilityInfo& AbilityInfo)
{
   RmAbility (AbilityInfo);
}

//=============================================================================
//                 Save / Load
//=============================================================================

void URAbilityMgrComponent::ConnectToSaveMgr ()
{
   if (!bSaveLoad || !R_IS_NET_ADMIN) return;

   // Careful with collision of 'UniqueSaveId'
   FString UniqueSaveId = GetOwner ()->GetName () + "_AbilityMgr";

   if (!InitSaveInterface (this, UniqueSaveId)) {
      FTimerHandle RetryHandle;
      RTIMER_START (RetryHandle, this, &URAbilityMgrComponent::ConnectToSaveMgr, 1, false);
   }
}

void URAbilityMgrComponent::OnSave (FBufferArchive &SaveData)
{
   TArray<FString> SaveAbiltities;

   // --- Get list of abilities as FString
   TArray<URAbility*> AbilityList;
   GetOwner ()->GetComponents (AbilityList);
   for (URAbility* ItAbility : AbilityList) {
      SaveAbiltities.Add (ItAbility->GetClass ()->GetPathName ());
   }

   // Serialize Data
   SaveData << SaveAbiltities;
   SaveData << AbilityPoints;
}

void URAbilityMgrComponent::OnLoad (FMemoryReader &LoadData)
{
   // Remove all current Abilities
   TArray<URAbility*> AbilityList;
   GetOwner ()->GetComponents (AbilityList);
   for (URAbility* ItAbility : AbilityList) {
      ItAbility->DestroyComponent ();
   }
   AbilityList.Empty ();

   // Serialize Data
   TArray<FString> LoadAbiltities;
   LoadData << LoadAbiltities;
   LoadData << AbilityPoints;

   // --- Create Abilities of Class.
   for (const FString &ItAbilityPath : LoadAbiltities) {

      TSoftClassPtr<URAbility> AbilityClass (ItAbilityPath);
      if (!AbilityClass.IsValid ()) {
         R_LOG_PRINTF ("Failed to create AbilityClass from path [%s]", *ItAbilityPath);
      }

      // --- Force add abilities
      URWorldAssetMgr::LoadAsync (AbilityClass.GetUniqueID (),
         this, [this] (UObject* LoadedContent) {
         if (UClass* AbilityClass = Cast<UClass> (LoadedContent)) {
            if (URUtil::AddComponent<URAbility> (GetOwner (), AbilityClass)) {
            }
         }
      });
   }

   ReportAbilityPointUpdated ();
}

