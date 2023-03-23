// Copyright 2015-2023 Vagen Ayrapetyan

#include "RWorldStatusMgr.h"
#include "RUtilLib/RLog.h"
#include "RUtilLib/RUtil.h"
#include "RUtilLib/RCheck.h"

//=============================================================================
//                   Static calls
//=============================================================================

URWorldStatusMgr* URWorldStatusMgr::GetInstance (UObject* WorldContextObject)
{
   if (!ensure (WorldContextObject)) return nullptr;

   UWorld *World = WorldContextObject->GetWorld ();
   if (!ensure (World)) return nullptr;

   AGameStateBase *GameState = World->GetGameState ();
   if (!ensure (GameState)) return nullptr;

   return GameState->FindComponentByClass<URWorldStatusMgr>();
}

//=============================================================================
//                   Member calls
//=============================================================================

URWorldStatusMgr::URWorldStatusMgr ()
{
}

void URWorldStatusMgr::BeginPlay ()
{
   Super::BeginPlay ();
}

void URWorldStatusMgr::ReportStatusEffect (URActiveStatusEffect* Effect, AActor* Causer, AActor* Victim)
{
   R_RETURN_IF_NOT_ADMIN;
   if (!ensure (Effect)) return;
   // if (!ensure (Causer)) return;
   if (!ensure (Victim)) return;
   OnStatusEffect.Broadcast (Effect, Causer, Victim);
}

