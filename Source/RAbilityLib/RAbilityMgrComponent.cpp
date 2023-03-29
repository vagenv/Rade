// Copyright 2015-2023 Vagen Ayrapetyan

#include "RAbilityMgrComponent.h"
#include "RAbilityTypes.h"
#include "RWorldAbilityMgr.h"

#include "RUtilLib/RUtil.h"
#include "RUtilLib/RLog.h"
#include "RUtilLib/RCheck.h"
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
}

//=============================================================================
//                 Core
//=============================================================================

void URAbilityMgrComponent::BeginPlay ()
{
   Super::BeginPlay ();

   // Balancing Mgr
   WorldAbilityMgr = URWorldAbilityMgr::GetInstance (this);

   // Exp Mgr
   ExperienceMgr = URUtil::GetComponent<URExperienceMgrComponent> (GetOwner ());

   if (R_IS_NET_ADMIN) {

      for (const TSubclassOf<URAbility> It : DefaultAbilities) {
         AbilityPoints++;
         AddAbility (It);
      }


      if (ExperienceMgr) {
         ExperienceMgr->OnLevelUp.AddDynamic (this, &URAbilityMgrComponent::LeveledUp);
      }

      // Save/Load Status
      if (bSaveLoad) {
         // Careful with collision of 'UniqueSaveId'
         FString UniqueSaveId = GetOwner ()->GetName () + "_AbilityMgr";
         Init_Save (this, UniqueSaveId);
      }
   }
}

void URAbilityMgrComponent::EndPlay (const EEndPlayReason::Type EndPlayReason)
{
   Super::EndPlay (EndPlayReason);
}

void URAbilityMgrComponent::TickComponent (float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
   Super::TickComponent (DeltaTime, TickType, ThisTickFunction);
}

//=============================================================================
//                 Ability Points
//=============================================================================

void URAbilityMgrComponent::LeveledUp ()
{
   R_RETURN_IF_NOT_ADMIN;
   if (!ensure (WorldAbilityMgr)) return;
   if (!ensure (ExperienceMgr))  return;

   AbilityPoints++;
   OnAbilityPointUpdated.Broadcast ();
}


//=============================================================================
//                 Ability Points
//=============================================================================

int URAbilityMgrComponent::GetAbilityPoint () const
{
   return AbilityPoints;
}

//=============================================================================
//                 Add / RM
//=============================================================================

URAbility* URAbilityMgrComponent::GetAbility (const TSubclassOf<URAbility> Ability_)
{
   if (!ensure (Ability_)) return nullptr;
   URAbility* Result = nullptr;
   if (Ability_) {
      TArray<URAbility*> AbilityList;
      GetOwner ()->GetComponents (AbilityList);
      for (URAbility *ItAbility : AbilityList) {
         if (ItAbility->GetClass () == Ability_) {
            Result = ItAbility;
            break;
         }
      }
   }
   return Result;
}

bool URAbilityMgrComponent::AddAbility (const TSubclassOf<URAbility> Ability_)
{
   R_RETURN_IF_NOT_ADMIN_BOOL;
   if (!ensure (AbilityPoints > 0)) return false;

   if (!ensure (Ability_)) return false;
   URAbility* Ability = URAbilityMgrComponent::GetAbility (Ability_);
   if (!ensure (!Ability)) return false;

   Ability = URUtil::AddComponent<URAbility> (GetOwner (), Ability_);
   if (Ability) {
      AbilityPoints--;
      OnAbilityPointUpdated.Broadcast ();
   }
   return Ability != nullptr;
}

bool URAbilityMgrComponent::RMAbility (const TSubclassOf<URAbility> Ability_)
{
   R_RETURN_IF_NOT_ADMIN_BOOL;
   if (!ensure (Ability_)) return false;
   URAbility* Ability = URAbilityMgrComponent::GetAbility (Ability_);
   if (!ensure (Ability)) return false;

   AbilityPoints++;
   OnAbilityPointUpdated.Broadcast ();

   Ability->DestroyComponent ();
   return true;
}

//=============================================================================
//                 Save / Load
//=============================================================================

void URAbilityMgrComponent::OnSave (FBufferArchive &SaveData)
{

}

void URAbilityMgrComponent::OnLoad (FMemoryReader &LoadData)
{

}

