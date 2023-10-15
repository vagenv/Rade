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

   UniqueEffectTag = GetName ();
}


void URAbility_Aura_StatusEffect::SetIsEnabled (bool Enabled)
{
   if (!Enabled) RemoveEffects ();
   Super::SetIsEnabled (Enabled);
}

void URAbility_Aura_StatusEffect::CheckRange ()
{
   Super::CheckRange ();
   ApplyEffects ();
}


void URAbility_Aura_StatusEffect::ApplyEffects ()
{
   if (!ensure (R_IS_NET_ADMIN)) return;
   if (!PassiveEffects.Num () && !ActiveEffects.Num ()) return;

   TArray<TWeakObjectPtr<URStatusMgrComponent> > AffectedMgr;

   for (const AActor* ItActor : GetAffectedActors ()) {
      if (URStatusMgrComponent* StatusMgr = URUtil::GetComponent<URStatusMgrComponent> (ItActor)) {

         // Perform check only if there are passive effects
         if (PassiveEffects.Num ()) {

            // Update list of affected target
            AffectedMgr.Add (StatusMgr);

            // Remove from list of targets, from which effects will be removed.
            LastAffectedMgr.Remove (StatusMgr);

            // Set flag only if not set already
            if (!StatusMgr->HasPassiveEffectWithTag (UniqueEffectTag)) {
               StatusMgr->SetPassiveEffects (UniqueEffectTag, PassiveEffects);
            }
         }

         // Refresh active status effects
         for (const TSoftClassPtr<URActiveStatusEffect>& ItEffect : ActiveEffects) {
            StatusMgr->AddActiveStatusEffect (GetOwner (), ItEffect);
         }
      }
   }

   // Will only remove effects from stale targets
   RemoveEffects ();

   // New target list is updated
   LastAffectedMgr = AffectedMgr;
}

void URAbility_Aura_StatusEffect::RemoveEffects ()
{
   if (!ensure (R_IS_NET_ADMIN)) return;
   for (const TWeakObjectPtr<URStatusMgrComponent> &ItStatusMgr : LastAffectedMgr) {
      if (ItStatusMgr.IsValid () && ItStatusMgr->HasPassiveEffectWithTag (UniqueEffectTag))
         ItStatusMgr->RmPassiveEffects (UniqueEffectTag);
   }
}

