// Copyright 2015-2023 Vagen Ayrapetyan

#include "RWorldExperienceMgr.h"
#include "RUtilLib/RLog.h"

//=============================================================================
//                   Static calls
//=============================================================================

URWorldExperienceMgr* URWorldExperienceMgr::GetInstance (UObject* WorldContextObject)
{
   if (!ensure (WorldContextObject)) return nullptr;

   UWorld *World = WorldContextObject->GetWorld ();
   if (!ensure (World)) return nullptr;

   AGameStateBase *GameState = World->GetGameState ();
   if (!ensure (GameState)) return nullptr;

   return GameState->FindComponentByClass<URWorldExperienceMgr>();
}

//=============================================================================
//                   Member calls
//=============================================================================

URWorldExperienceMgr::URWorldExperienceMgr ()
{
}

