// Copyright 2015-2023 Vagen Ayrapetyan

#include "RAbility.h"
#include "RAbilityMgrComponent.h"
#include "RWorldAbilityMgr.h"

#include "RUtilLib/RUtil.h"
#include "RUtilLib/RLog.h"
#include "RUtilLib/RCheck.h"
#include "RUtilLib/RTimer.h"

//=============================================================================
//                 Ability Base
//=============================================================================

URAbility::URAbility ()
{
   SetIsReplicatedByDefault (true);
}

void URAbility::BeginPlay ()
{
   Super::BeginPlay ();

   if (URUtil::GetWorld (this) && !GetOwner ()->GetInstanceComponents ().Contains (this))
      GetOwner ()->AddInstanceComponent (this);

   OwnerAbilityMgr = URUtil::GetComponent<URAbilityMgrComponent>(GetOwner ());
   PullAbilityInfo ();
}

void URAbility::EndPlay (const EEndPlayReason::Type EndPlayReason)
{
   if (OwnerAbilityMgr.IsValid ()) OwnerAbilityMgr->ReportAbilityListUpdated ();
   if (WorldAbilityMgr.IsValid ()) WorldAbilityMgr->ReportRmAbility (this);

   if (URUtil::GetWorld (this) && GetOwner ()->GetInstanceComponents ().Contains (this))
      GetOwner ()->RemoveInstanceComponent (this);

   Super::EndPlay (EndPlayReason);
}

void URAbility::PullAbilityInfo ()
{
   WorldAbilityMgr = URWorldAbilityMgr::GetInstance (this);
   if (!WorldAbilityMgr.IsValid ()) {

      FTimerHandle RetryHandle;
      RTIMER_START (RetryHandle,
                    this, &URAbility::PullAbilityInfo,
                    1, false);
      return;
   }

   AbilityInfo = WorldAbilityMgr->GetAbilityInfo_Object (this);
   if (AbilityInfo.IsEmpty ()) {
      R_LOG_PRINTF ("[%s] Recieved invalid Ability Info from WorldAbilityMgr.", *GetName ());
   }
   
   // Report that ability has been added only after pulling ability information
   WorldAbilityMgr->ReportAddAbility (this);
   if (OwnerAbilityMgr.IsValid ()) OwnerAbilityMgr->ReportAbilityListUpdated ();

   AbilityInfoLoaded ();
}

void URAbility::SetIsEnabled (bool IsEnabled_)
{
   IsEnabled = IsEnabled_;
}

bool URAbility::GetIsEnabled () const
{
   return IsEnabled;
}

void URAbility::AbilityInfoLoaded ()
{
}

FRAbilityInfo URAbility::GetAbilityInfo () const
{
   return AbilityInfo;
}

FRUIDescription URAbility::GetDescription_Implementation () const
{
   return AbilityInfo.Description;
}

