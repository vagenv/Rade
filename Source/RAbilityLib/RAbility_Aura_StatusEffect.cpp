// Copyright 2015-2023 Vagen Ayrapetyan

#include "RAbility_Aura_StatusEffect.h"

#include "RUtilLib/RUtil.h"
#include "RUtilLib/RLog.h"
#include "RUtilLib/RCheck.h"
#include "RUtilLib/RTimer.h"
#include "RUtilLib/RWorldAssetMgr.h"
#include "RStatusLib/RStatusMgrComponent.h"

void URAbility_Aura_StatusEffect::BeginPlay ()
{
   Super::BeginPlay ();

   UniqueEffectTag = "[" + GetOwner ()->GetName () + "] (" + GetName () + ")";
}


void URAbility_Aura_StatusEffect::SetIsEnabled (bool Enabled)
{
   if (!Enabled) RemoveEffects ();
   Super::SetIsEnabled (Enabled);
}

void URAbility_Aura_StatusEffect::CheckRange ()
{
   RemoveEffects ();
   Super::CheckRange ();
   ApplyEffects ();
}

void URAbility_Aura_StatusEffect::RemoveEffects ()
{
   if (!ensure (R_IS_NET_ADMIN)) return;
   for (const AActor* ItActor : GetAffectedActors ()) {
      if (URStatusMgrComponent* StatusMgr = URUtil::GetComponent<URStatusMgrComponent> (ItActor)) {
         StatusMgr->RmPassiveEffects (UniqueEffectTag);
      }
   }
}

void URAbility_Aura_StatusEffect::ApplyEffects ()
{
   if (!ensure (R_IS_NET_ADMIN)) return;
   if (!PassiveEffects.Num () && !ActiveEffects.Num ()) return;

   for (const AActor* ItActor : GetAffectedActors ()) {
      if (URStatusMgrComponent* StatusMgr = URUtil::GetComponent<URStatusMgrComponent> (ItActor)) {
         if (PassiveEffects.Num ()) {
            StatusMgr->SetPassiveEffects (UniqueEffectTag, PassiveEffects);
         }
         for (const TSoftClassPtr<URActiveStatusEffect>& ItEffect : ActiveEffects) {
            StatusMgr->AddActiveStatusEffect (GetOwner (), ItEffect);
         }
      }
   }
}