// Copyright 2015-2023 Vagen Ayrapetyan

#include "RWorldAbilityMgr.h"
#include "RUtilLib/RUtil.h"
#include "RUtilLib/RCheck.h"
#include "RUtilLib/RLog.h"

//=============================================================================
//                   Static calls
//=============================================================================

URWorldAbilityMgr* URWorldAbilityMgr::GetInstance (UObject* WorldContextObject)
{
   return URUtil::GetWorldInstance<URWorldAbilityMgr> (WorldContextObject);
}

//=============================================================================
//                   Member calls
//=============================================================================

URWorldAbilityMgr::URWorldAbilityMgr ()
{
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

