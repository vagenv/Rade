// Copyright 2015-2023 Vagen Ayrapetyan

#include "RWorldAssetMgr.h"
#include "RUtilLib/RUtil.h"
#include "RUtilLib/RCheck.h"
#include "RUtilLib/RLog.h"

//=============================================================================
//                   Static calls
//=============================================================================

URWorldAssetMgr* URWorldAssetMgr::GetInstance (const UObject* WorldContextObject)
{
   return URUtil::GetWorldInstance<URWorldAssetMgr> (WorldContextObject);
}

//=============================================================================
//                   Member calls
//=============================================================================

URWorldAssetMgr::URWorldAssetMgr ()
{
}

