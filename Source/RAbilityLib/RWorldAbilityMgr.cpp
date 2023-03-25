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

