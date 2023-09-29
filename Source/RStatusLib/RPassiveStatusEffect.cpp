// Copyright 2015-2023 Vagen Ayrapetyan

#include "RPassiveStatusEffect.h"

//=============================================================================
//                 Passive Effect
//=============================================================================

TArray<FRPassiveStatusEffect> URPassiveStatusEffectUtilLibrary::MergeEffects (
   const TArray<FRPassiveStatusEffectWithTag>& Effects)
{
   TArray<FRPassiveStatusEffect> Result;
   for (const FRPassiveStatusEffectWithTag& ItEffects : Effects) {
      bool found = false;
      // Combine
      for (FRPassiveStatusEffect& ItRes : Result) {
         if (ItRes.EffectTarget == ItEffects.Value.EffectTarget) {
            found = true;
            ItRes.Flat    += ItEffects.Value.Flat;
            ItRes.Percent += ItEffects.Value.Percent;
            break;
         }
      }
      // Add new entry and not zero
      if (!found && (ItEffects.Value.Flat || ItEffects.Value.Percent ))
         Result.Add (ItEffects.Value);
   }

   return Result;
}

//=============================================================================
//                 Status Effect Library
//=============================================================================

// bool URPassiveStatusEffectUtilLibrary::SetStatusEffect_Passive (
//    AActor *Target,
//    const FString &Tag,
//    const TArray<FRPassiveStatusEffect> &Effects)
// {
//    // --- Check Values
//    if (!ensure (Target))                  return false;
//    if (!ensure (Target->HasAuthority ())) return false;
//    if (!ensure (!Tag.IsEmpty ()))         return false;
//    URStatusMgrComponent* StatusMgr = URUtil::GetComponent<URStatusMgrComponent> (Target);
//    if (!ensure (StatusMgr))               return false;

//    // --- Action
//    return StatusMgr->SetPassiveEffects (Tag, Effects);
// }

// bool URPassiveStatusEffectUtilLibrary::RmStatusEffect_Passive (
//    AActor *Target,
//    const FString &Tag)
// {
//    // --- Check Values
//    if (!ensure (Target))                  return false;
//    if (!ensure (Target->HasAuthority ())) return false;
//    if (!ensure (!Tag.IsEmpty ()))         return false;
//    URStatusMgrComponent* StatusMgr = URUtil::GetComponent<URStatusMgrComponent> (Target);
//    if (!ensure (StatusMgr))               return false;

//    // --- Action
//    return StatusMgr->RmPassiveEffects (Tag);
// }

