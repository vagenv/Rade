// Copyright 2015-2023 Vagen Ayrapetyan

#include "RWorldDamageMgr.h"
#include "RUtilLib/RCheck.h"
#include "RUtilLib/RLog.h"

//=============================================================================
//                   Static calls
//=============================================================================

URWorldDamageMgr* URWorldDamageMgr::GetInstance (UObject* WorldContextObject)
{
   if (!ensure (WorldContextObject)) return nullptr;

   UWorld *World = WorldContextObject->GetWorld ();
   if (!ensure (World)) return nullptr;

   AGameStateBase *GameState = World->GetGameState ();
   if (!ensure (GameState)) return nullptr;

   URWorldDamageMgr* DamageMgr = GameState->FindComponentByClass<URWorldDamageMgr>();
   return DamageMgr;
}

//=============================================================================
//                   Member calls
//=============================================================================

URWorldDamageMgr::URWorldDamageMgr ()
{
}

void URWorldDamageMgr::ReportDamage (AActor*             Victim,
                                     float               Amount,
                                     const URDamageType* Type,
                                     AActor*             Causer)
{
   R_RETURN_IF_NOT_ADMIN;
   if (!ensure (Victim)) return;
   if (!ensure (Type))   return;
   if (!ensure (Causer)) return;
   OnAnyRDamage.Broadcast (Victim, Amount, Type, Causer);
}

void URWorldDamageMgr::ReportDeath (AActor*             Victim,
                                    AActor*             Causer,
                                    const URDamageType* Type)
{
   R_RETURN_IF_NOT_ADMIN;
   if (!ensure (Victim)) return;
   if (!ensure (Causer)) return;
   if (!ensure (Type))   return;
   OnDeath.Broadcast (Victim, Causer, Type);
}

void URWorldDamageMgr::ReportRevive (AActor* Victim)
{
   R_RETURN_IF_NOT_ADMIN;
   if (!ensure (Victim)) return;
   OnRevive.Broadcast (Victim);
}

