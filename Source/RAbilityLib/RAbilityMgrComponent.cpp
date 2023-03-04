// Copyright 2015-2023 Vagen Ayrapetyan

#include "RAbilityMgrComponent.h"
#include "RUtilLib/RLog.h"
#include "RUtilLib/RCheck.h"

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
   const UWorld *world = GetWorld ();
   if (!ensure (world)) return;


   if (R_IS_NET_ADMIN) {

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
//                 Save / Load
//=============================================================================

void URAbilityMgrComponent::OnSave (FBufferArchive &SaveData)
{

}

void URAbilityMgrComponent::OnLoad (FMemoryReader &LoadData)
{

}

//=============================================================================
//                 UTIL
//=============================================================================
URAbilityMgrComponent* URAbilityMgrComponent::Get (AActor* Target)
{
   if (!ensure (Target)) return nullptr;
   URAbilityMgrComponent* AbilityMgr = nullptr;
   {
      TArray<URAbilityMgrComponent*> AbilityMgrList;
      Target->GetComponents (AbilityMgrList);
      if (AbilityMgrList.Num ()) AbilityMgr = AbilityMgrList[0];
   }
   if (!ensure (AbilityMgr)) return nullptr;
   return AbilityMgr;
}

