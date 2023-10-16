// Copyright 2015-2023 Vagen Ayrapetyan

#include "RPassiveStatusEffect.h"

//=============================================================================
//                 Passive Effect
//=============================================================================

TArray<FRPassiveStatusEffect> URPassiveStatusEffectUtilLibrary::MergeEffects (
   const TArray<FRPassiveStatusEffectWithTag>& TagEffects)
{
   // --- Merge list by target
   TMap<ERStatusEffectTarget, FRPassiveStatusEffect> EffectMap;
   for (const FRPassiveStatusEffectWithTag& ItTagEffects : TagEffects) {
      for (const FRPassiveStatusEffect& ItEffect : ItTagEffects.Effects) {
         if (EffectMap.Contains (ItEffect.EffectTarget)) {
            EffectMap[ItEffect.EffectTarget].Flat    += ItEffect.Flat;
            EffectMap[ItEffect.EffectTarget].Percent += ItEffect.Percent;
         } else {
            EffectMap.Add (ItEffect.EffectTarget, ItEffect);
         }
      }
      
   }

   // Transform to Array
   TArray<FRPassiveStatusEffect> Result;
   for (const auto &ItEffect : EffectMap) {
      Result.Add (ItEffect.Value);
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

