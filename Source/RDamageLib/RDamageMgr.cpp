// Copyright 2015-2023 Vagen Ayrapetyan

#include "RDamageMgr.h"
#include "RUtilLib/RCheck.h"
#include "RUtilLib/RLog.h"

//=============================================================================
//                   Static calls
//=============================================================================

URDamageMgr* URDamageMgr::GetInstance (UObject* WorldContextObject)
{
   if (!ensure (WorldContextObject)) return nullptr;

   UWorld *World = WorldContextObject->GetWorld ();
   if (!ensure (World)) return nullptr;

   AGameStateBase *GameState = World->GetGameState ();
   if (!ensure (GameState)) return nullptr;

   URDamageMgr* DamageMgr = GameState->FindComponentByClass<URDamageMgr>();
   return DamageMgr;
}

//=============================================================================
//                   Member calls
//=============================================================================

URDamageMgr::URDamageMgr ()
{
}

void URDamageMgr::ReportDamage (AActor* DamageTarget,
                                float Damage,
                                const URDamageType* DamageType,
                                AActor* DamageCauser)
{
   R_RETURN_IF_NOT_ADMIN;
   OnAnyRDamage.Broadcast (DamageTarget, Damage, DamageType, DamageCauser);
}

void URDamageMgr::ReportDeath (AActor* Victim,
                               AActor* Causer,
                               const URDamageType* DamageType)
{
   R_RETURN_IF_NOT_ADMIN;
   OnDeath.Broadcast (Victim, Causer, DamageType);
}

void URDamageMgr::ReportRevive (AActor* WhoRevived)
{
   R_RETURN_IF_NOT_ADMIN;
   OnRevive.Broadcast (WhoRevived);
}

void URDamageMgr::ReportStatusEffect (URActiveStatusEffect* Effect, AActor* Causer, AActor* Target)
{
   R_RETURN_IF_NOT_ADMIN;
   OnStatusEffect.Broadcast (Effect, Causer, Target);
}
