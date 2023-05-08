// Copyright 2015-2023 Vagen Ayrapetyan

#include "RWorldSaveMgr.h"
#include "RSaveGame.h"
#include "RUtilLib/RUtil.h"
#include "RUtilLib/RCheck.h"

//=============================================================================
//                   Static calls
//=============================================================================

URWorldSaveMgr* URWorldSaveMgr::GetInstance (const UObject* WorldContextObject)
{
   return URUtil::GetWorldInstance<URWorldSaveMgr> (WorldContextObject);
}

//=============================================================================
//                   Member calls
//=============================================================================

URWorldSaveMgr::URWorldSaveMgr ()
{
}

void URWorldSaveMgr::ReportSave (URSaveGame* SaveGame)
{
   if (!IsValid (SaveGame)) return;
   if (R_IS_VALID_WORLD && OnSave.IsBound ()) OnSave.Broadcast (SaveGame);
}

void URWorldSaveMgr::ReportLoad (URSaveGame* SaveGame)
{
   if (!IsValid (SaveGame)) return;
   if (R_IS_VALID_WORLD && OnLoad.IsBound ()) OnLoad.Broadcast (SaveGame);
}

void URWorldSaveMgr::ReportSaveListUpdated ()
{
   if (R_IS_VALID_WORLD && OnSaveListUpdated.IsBound ()) OnSaveListUpdated.Broadcast ();
}

