// Copyright 2015-2023 Vagen Ayrapetyan

#include "RAbilityMgrComponent.h"
#include "RAbilityTypes.h"

#include "RUtilLib/RUtil.h"
#include "RUtilLib/RLog.h"
#include "RUtilLib/RCheck.h"
#include "RUtilLib/RTimer.h"
#include "RUtilLib/RWorldAssetMgr.h"
#include "RExperienceLib/RExperienceMgrComponent.h"

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

URAbility* URAbilityMgrComponent::GetAbility (const TSoftClassPtr<URAbility> Ability_)
{
   if (!ensure (!Ability_.IsNull ())) return nullptr;
   URAbility* Result = nullptr;
   TArray<URAbility*> AbilityList;
   GetOwner ()->GetComponents (AbilityList);
   for (URAbility* ItAbility : AbilityList) {
      if (ItAbility->GetClass () == Ability_) {
         Result = ItAbility;
         break;
      }
   }

   return Result;
}

bool URAbilityMgrComponent::AddAbility (const TSoftClassPtr<URAbility> Ability_)
{
   R_RETURN_IF_NOT_ADMIN_BOOL;
   if (!ensure (AbilityPoints > 0)) return false;
   if (!ensure (!Ability_.IsNull ())) return false;
   if (!ensure (!URAbilityMgrComponent::GetAbility (Ability_))) return false;


   URWorldAssetMgr::LoadAsync (Ability_.GetUniqueID (), this, [this] (UObject* LoadedContent) {
      if (UClass* AbilityClass = Cast<UClass> (LoadedContent)) {
         URAbility* Ability = URUtil::AddComponent<URAbility> (GetOwner (), AbilityClass);
         if (IsValid (Ability)) {
            AbilityPoints--;
            ReportAbilityPointUpdated ();
         }
      }
   });

   return true;
}

bool URAbilityMgrComponent::RmAbility (const TSoftClassPtr<URAbility> Ability_)
{
   R_RETURN_IF_NOT_ADMIN_BOOL;
   if (!ensure (!Ability_.IsNull ())) return false;
   URAbility* Ability = URAbilityMgrComponent::GetAbility (Ability_);
   if (!Ability) return false;

   AbilityPoints++;
   ReportAbilityPointUpdated ();

   Ability->DestroyComponent ();
   return true;
}

void URAbilityMgrComponent::ReportAbilityListUpdated ()
{
   if (R_IS_VALID_WORLD && OnAbilityListUpdated.IsBound ()) OnAbilityListUpdated.Broadcast ();
}

//=============================================================================
//                 Server versions of the functions
//=============================================================================
void URAbilityMgrComponent::AddAbility_Server_Implementation (const TSoftClassPtr<URAbility> &Ability)
{
   AddAbility (Ability);
}
void URAbilityMgrComponent::RmAbility_Server_Implementation (const TSoftClassPtr<URAbility> &Ability)
{
   RmAbility (Ability);
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

   int SaveAbilityPoints;

   // Serialize Data
   TArray<FString> LoadAbiltities;
   LoadData << LoadAbiltities;
   LoadData << SaveAbilityPoints;

   // --- Create Abilities of Class.
   for (const FString &ItAbilityPath : LoadAbiltities) {

      TSoftClassPtr<URAbility> AbilityClass (ItAbilityPath);

      if (!AbilityClass.IsValid ()) {
         R_LOG_PRINTF ("Failed to create AbilityClass from path [%s]", *ItAbilityPath);
      }

      AbilityPoints++;
      if (!AddAbility (AbilityClass)) {
         R_LOG_PRINTF ("Failed to Add Ability from SoftClass [%s]", *AbilityClass.ToString ());
      }
   }

   AbilityPoints = SaveAbilityPoints;

   ReportAbilityPointUpdated ();
}

