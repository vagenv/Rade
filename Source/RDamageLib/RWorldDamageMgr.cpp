// Copyright 2015-2023 Vagen Ayrapetyan

#include "RWorldDamageMgr.h"
#include "RUtilLib/RUtil.h"
#include "RUtilLib/RCheck.h"
#include "RUtilLib/RLog.h"

//=============================================================================
//                   Static calls
//=============================================================================

URWorldDamageMgr* URWorldDamageMgr::GetInstance (UObject* WorldContextObject)
{
   return URUtil::GetWorldInstance<URWorldDamageMgr> (WorldContextObject);
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
   if (!ensure (Victim)) return;
   if (!ensure (Type))   return;
   if (!ensure (Causer)) return;
   OnAnyRDamage.Broadcast (Victim, Amount, Type, Causer);
}

void URWorldDamageMgr::ReportDeath (AActor*             Victim,
                                    AActor*             Causer,
                                    const URDamageType* Type)
{
   if (!ensure (Victim)) return;
   if (!ensure (Causer)) return;
   if (!ensure (Type))   return;
   OnDeath.Broadcast (Victim, Causer, Type);
}

void URWorldDamageMgr::ReportRevive (AActor* Victim)
{
   if (!ensure (Victim)) return;
   OnRevive.Broadcast (Victim);
}

