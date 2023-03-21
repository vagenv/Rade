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

void URWorldDamageMgr::ReportDamage (AActor* DamageTarget,
                                float Damage,
                                const URDamageType* DamageType,
                                AActor* DamageCauser)
{
   R_RETURN_IF_NOT_ADMIN;
   OnAnyRDamage.Broadcast (DamageTarget, Damage, DamageType, DamageCauser);
}

void URWorldDamageMgr::ReportDeath (AActor* Victim,
                               AActor* Causer,
                               const URDamageType* DamageType)
{
   R_RETURN_IF_NOT_ADMIN;
   OnDeath.Broadcast (Victim, Causer, DamageType);
}

void URWorldDamageMgr::ReportRevive (AActor* WhoRevived)
{
   R_RETURN_IF_NOT_ADMIN;
   OnRevive.Broadcast (WhoRevived);
}

void URWorldDamageMgr::ReportStatusEffect (UActorComponent* Effect, AActor* Causer, AActor* Target)
{
   R_RETURN_IF_NOT_ADMIN;
   OnStatusEffect.Broadcast (Effect, Causer, Target);
}
