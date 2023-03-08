// Copyright 2015-2023 Vagen Ayrapetyan

#include "RTargetableMgr.h"

#include "RUtilLib/RLog.h"

//=============================================================================
//                   Static calls
//=============================================================================

URTargetableMgr* URTargetableMgr::GetInstance (UObject* WorldContextObject)
{
   if (!ensure (WorldContextObject)) return nullptr;

   UWorld *World = WorldContextObject->GetWorld ();
   if (!ensure (World)) return nullptr;

   AGameModeBase *GameMode = World->GetAuthGameMode ();
   if (!ensure (GameMode)) return nullptr;

   URTargetableMgr* TargetableMgr = GameMode->FindComponentByClass<URTargetableMgr>();
   return TargetableMgr;
}

//=============================================================================
//                   Member calls
//=============================================================================

URTargetableMgr::URTargetableMgr ()
{
}
