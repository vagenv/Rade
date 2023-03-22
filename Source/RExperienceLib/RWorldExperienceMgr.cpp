// Copyright 2015-2023 Vagen Ayrapetyan

#include "RWorldExperienceMgr.h"
#include "RExperienceMgrComponent.h"
#include "RUtilLib/RLog.h"
#include "RUtilLib/RUtil.h"
#include "RUtilLib/RCheck.h"
#include "RDamageLib/RWorldDamageMgr.h"

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

void URWorldExperienceMgr::BeginPlay ()
{
   Super::BeginPlay ();

   if (R_IS_NET_ADMIN) {
      if (URWorldDamageMgr *DamageMgr = URUtil::GetComponent<URWorldDamageMgr> (GetOwner ())) {
         DamageMgr->OnAnyRDamage.AddDynamic (this, &URWorldExperienceMgr::OnDamage);
         DamageMgr->OnDeath.AddDynamic (this, &URWorldExperienceMgr::OnDeath);
      }
   }
}


void URWorldExperienceMgr::OnDamage (AActor*             Victim,
                                     float               Amount,
                                     const URDamageType* Type,
                                     AActor*             Causer)
{
   R_RETURN_IF_NOT_ADMIN;
   if (!ensure (Victim)) return;
   if (!ensure (Type))   return;
   if (!ensure (Causer)) return;
}

void URWorldExperienceMgr::OnDeath (AActor* Victim,
                                    AActor* Causer,
                                    const URDamageType* Type)
{
   R_RETURN_IF_NOT_ADMIN;
   if (!ensure (Victim)) return;
   if (!ensure (Causer)) return;
   if (!ensure (Type))   return;
}

