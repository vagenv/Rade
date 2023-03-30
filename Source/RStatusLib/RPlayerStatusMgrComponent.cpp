// Copyright 2015-2023 Vagen Ayrapetyan

#include "RPlayerStatusMgrComponent.h"
#include "RWorldStatusMgr.h"
#include "RActiveStatusEffect.h"

#include "RUtilLib/RUtil.h"
#include "RUtilLib/RLog.h"
#include "RUtilLib/RCheck.h"
#include "RDamageLib/RDamageType.h"
#include "RDamageLib/RWorldDamageMgr.h"
#include "RExperienceLib/RExperienceMgrComponent.h"

#include "Net/UnrealNetwork.h"

URPlayerStatusMgrComponent::URPlayerStatusMgrComponent ()
{
   CoreStats_Base = FRCoreStats (10);
}

void URPlayerStatusMgrComponent::GetLifetimeReplicatedProps (TArray<FLifetimeProperty> &OutLifetimeProps) const
{
   Super::GetLifetimeReplicatedProps (OutLifetimeProps);

   DOREPLIFETIME (URPlayerStatusMgrComponent, CoreStats_Base);
   DOREPLIFETIME (URPlayerStatusMgrComponent, CoreStats_Added);
   DOREPLIFETIME (URPlayerStatusMgrComponent, SubStats_Base);
   DOREPLIFETIME (URPlayerStatusMgrComponent, SubStats_Added);
}
void URPlayerStatusMgrComponent::OnRep_Stats ()
{
   if (R_IS_VALID_WORLD) OnStatsUpdated.Broadcast ();
}

void URPlayerStatusMgrComponent::BeginPlay ()
{
   Super::BeginPlay ();

   // Exp Mgr
   ExperienceMgr = URUtil::GetComponent<URExperienceMgrComponent> (GetOwner ());

   if (R_IS_NET_ADMIN) {

      FTimerHandle MyHandle;
      GetOwner ()->GetWorldTimerManager ().SetTimer (MyHandle,
                                                     this,
                                                     &URPlayerStatusMgrComponent::RecalcStatus,
                                                     1,
                                                     false);

      // Recalc on revive
      OnRevive.AddDynamic (this, &URPlayerStatusMgrComponent::RecalcStatus);


      if (ExperienceMgr) {
         ExperienceMgr->OnLevelUp.AddDynamic (this, &URPlayerStatusMgrComponent::LeveledUp);
      }

      // Save/Load Status
      if (bSaveLoad) {
         // Careful with collision of 'UniqueSaveId'
         FString UniqueSaveId = GetOwner ()->GetName () + "_PlayerStatusMgr";
         Init_Save (this, UniqueSaveId);
      }
   }
}

//=============================================================================
//                 Level up
//=============================================================================

void URPlayerStatusMgrComponent::LeveledUp ()
{
   R_RETURN_IF_NOT_ADMIN;
   if (!ensure (WorldStatusMgr)) return;
   if (!ensure (ExperienceMgr))  return;

   float ExtraStats = WorldStatusMgr->GetLevelUpExtraStatGain (ExperienceMgr->GetCurrentLevel ());

   if (ExtraStats) CoreStats_Extra += ExtraStats;

   FRCoreStats DeltaStats = WorldStatusMgr->GetLevelUpStatGain (ExperienceMgr->GetCurrentLevel ());
   if (!DeltaStats.Empty ()) CoreStats_Base += DeltaStats;
   if (!DeltaStats.Empty () || ExtraStats)
      RecalcStatus ();
}

//=============================================================================
//                 Recalc stats
//=============================================================================

void URPlayerStatusMgrComponent::RecalcStatus ()
{
   R_RETURN_IF_NOT_ADMIN;
   RecalcCoreStats ();
   RecalcSubStats ();
   RecalcStatusValues ();

   SetHealth (Health);
   SetStamina (Stamina);
   SetMana (Mana);

   if (R_IS_VALID_WORLD) OnStatsUpdated.Broadcast ();
}

void URPlayerStatusMgrComponent::RecalcCoreStats ()
{
   R_RETURN_IF_NOT_ADMIN;
   FRCoreStats CoreStats_Total_New = GetCoreStats_Base ();

   // Flat
   for (const FRPassiveStatusEffectWithTag &ItEffect : GetPassiveEffectsWithTag ()) {
      if (ItEffect.Value.Scale == ERStatusEffectScale::FLAT) {
         if (ItEffect.Value.Target == ERStatusEffectTarget::STR) CoreStats_Total_New.STR += ItEffect.Value.Value;
         if (ItEffect.Value.Target == ERStatusEffectTarget::AGI) CoreStats_Total_New.AGI += ItEffect.Value.Value;
         if (ItEffect.Value.Target == ERStatusEffectTarget::INT) CoreStats_Total_New.INT += ItEffect.Value.Value;
      }
   }
   // Percentage
   for (const FRPassiveStatusEffectWithTag &ItEffect : GetPassiveEffectsWithTag ()) {
      if (ItEffect.Value.Scale == ERStatusEffectScale::PERCENT) {
         if (ItEffect.Value.Target == ERStatusEffectTarget::STR) CoreStats_Total_New.STR *= ((100. + ItEffect.Value.Value) / 100.);
         if (ItEffect.Value.Target == ERStatusEffectTarget::AGI) CoreStats_Total_New.AGI *= ((100. + ItEffect.Value.Value) / 100.);
         if (ItEffect.Value.Target == ERStatusEffectTarget::INT) CoreStats_Total_New.INT *= ((100. + ItEffect.Value.Value) / 100.);
      }
   }
   CoreStats_Added = CoreStats_Total_New - GetCoreStats_Base ();
}

void URPlayerStatusMgrComponent::RecalcSubStats ()
{
   R_RETURN_IF_NOT_ADMIN;
   if (!ensure (WorldStatusMgr)) return;

   FRCoreStats StatsTotal = GetCoreStats_Total ();
   float EvasionTotal     = WorldStatusMgr->GetAgiToEvasion     (StatsTotal.AGI);
   float CriticalTotal    = WorldStatusMgr->GetAgiToCritical    (StatsTotal.AGI);
   float AttackSpeedTotal = WorldStatusMgr->GetAgiToAttackSpeed (StatsTotal.AGI);
   float MoveSpeedTotal   = 0;

   // Flat
   for (const FRPassiveStatusEffectWithTag &ItEffect : GetPassiveEffectsWithTag ()) {
      if (ItEffect.Value.Scale == ERStatusEffectScale::FLAT) {
         if (ItEffect.Value.Target == ERStatusEffectTarget::Evasion)     EvasionTotal     += ItEffect.Value.Value;
         if (ItEffect.Value.Target == ERStatusEffectTarget::Critical)    CriticalTotal    += ItEffect.Value.Value;
         if (ItEffect.Value.Target == ERStatusEffectTarget::AttackSpeed) AttackSpeedTotal += ItEffect.Value.Value;
         if (ItEffect.Value.Target == ERStatusEffectTarget::MoveSpeed)   MoveSpeedTotal   += ItEffect.Value.Value;
      }
   }
   // Percentage
   for (const FRPassiveStatusEffectWithTag &ItEffect : GetPassiveEffectsWithTag ()) {
      if (ItEffect.Value.Scale == ERStatusEffectScale::PERCENT) {
         if (ItEffect.Value.Target == ERStatusEffectTarget::Evasion)     EvasionTotal     *= ((100. + ItEffect.Value.Value) / 100.);
         if (ItEffect.Value.Target == ERStatusEffectTarget::Critical)    CriticalTotal    *= ((100. + ItEffect.Value.Value) / 100.);
         if (ItEffect.Value.Target == ERStatusEffectTarget::AttackSpeed) AttackSpeedTotal *= ((100. + ItEffect.Value.Value) / 100.);
         if (ItEffect.Value.Target == ERStatusEffectTarget::MoveSpeed)   MoveSpeedTotal   *= ((100. + ItEffect.Value.Value) / 100.);
      }
   }

   FRCoreStats StatsCurrent = GetCoreStats_Base ();
   SubStats_Base.Evasion      = WorldStatusMgr->GetAgiToEvasion     (StatsCurrent.AGI);
   SubStats_Added.Evasion     = EvasionTotal - SubStats_Base.Evasion;
   SubStats_Base.Critical     = WorldStatusMgr->GetAgiToCritical    (StatsCurrent.AGI);
   SubStats_Added.Critical    = CriticalTotal - SubStats_Base.Critical;
   SubStats_Base.AttackSpeed  = WorldStatusMgr->GetAgiToAttackSpeed (StatsCurrent.AGI);
   SubStats_Added.AttackSpeed = CriticalTotal - SubStats_Base.Critical;
   SubStats_Base.MoveSpeed    = MoveSpeedTotal;
   SubStats_Added.MoveSpeed   = 0;

   CriticalChance = CriticalTotal;
   EvasionChance  = EvasionTotal;
}

void URPlayerStatusMgrComponent::RecalcStatusValues ()
{
   R_RETURN_IF_NOT_ADMIN;
   if (!ensure (WorldStatusMgr)) return;

   // --- Status
   FRCoreStats StatsTotal = GetCoreStats_Total ();
   Health.Max     = WorldStatusMgr->GetStrToHealthMax    (StatsTotal.STR);
   Health.Regen   = WorldStatusMgr->GetStrToHealthRegen  (StatsTotal.STR);
   Stamina.Max    = WorldStatusMgr->GetAgiToStaminaMax   (StatsTotal.AGI);
   Stamina.Regen  = WorldStatusMgr->GetAgiToStaminaRegen (StatsTotal.AGI);
   Mana.Max       = WorldStatusMgr->GetIntToManaMax      (StatsTotal.INT);
   Mana.Regen     = WorldStatusMgr->GetIntToManaRegen    (StatsTotal.INT);

   // Flat
   for (const FRPassiveStatusEffectWithTag &ItEffect : GetPassiveEffectsWithTag ()) {
      if (ItEffect.Value.Scale == ERStatusEffectScale::FLAT) {
         if (ItEffect.Value.Target == ERStatusEffectTarget::HealthMax)    Health.Max    += ItEffect.Value.Value;
         if (ItEffect.Value.Target == ERStatusEffectTarget::HealthRegen)  Health.Regen  += ItEffect.Value.Value;
         if (ItEffect.Value.Target == ERStatusEffectTarget::StaminaMax)   Stamina.Max   += ItEffect.Value.Value;
         if (ItEffect.Value.Target == ERStatusEffectTarget::StaminaRegen) Stamina.Regen += ItEffect.Value.Value;
         if (ItEffect.Value.Target == ERStatusEffectTarget::ManaMax)      Mana.Max      += ItEffect.Value.Value;
         if (ItEffect.Value.Target == ERStatusEffectTarget::ManaRegen)    Mana.Regen    += ItEffect.Value.Value;
      }
   }
   // Percentage
   for (const FRPassiveStatusEffectWithTag &ItEffect : GetPassiveEffectsWithTag ()) {
      if (ItEffect.Value.Scale == ERStatusEffectScale::PERCENT) {
         if (ItEffect.Value.Target == ERStatusEffectTarget::HealthMax)    Health.Max    *= ((100. + ItEffect.Value.Value) / 100.);
         if (ItEffect.Value.Target == ERStatusEffectTarget::HealthRegen)  Health.Regen  *= ((100. + ItEffect.Value.Value) / 100.);
         if (ItEffect.Value.Target == ERStatusEffectTarget::StaminaMax)   Stamina.Max   *= ((100. + ItEffect.Value.Value) / 100.);
         if (ItEffect.Value.Target == ERStatusEffectTarget::StaminaRegen) Stamina.Regen *= ((100. + ItEffect.Value.Value) / 100.);
         if (ItEffect.Value.Target == ERStatusEffectTarget::ManaMax)      Mana.Max      *= ((100. + ItEffect.Value.Value) / 100.);
         if (ItEffect.Value.Target == ERStatusEffectTarget::ManaRegen)    Mana.Regen    *= ((100. + ItEffect.Value.Value) / 100.);
      }
   }
}

//=============================================================================
//                 Extra stat Points
//=============================================================================

float URPlayerStatusMgrComponent::GetCoreStats_Extra () const
{
   return CoreStats_Extra;
}

bool URPlayerStatusMgrComponent::AddExtraStat (FRCoreStats ExtraStat)
{
   float TotalUse = ExtraStat.STR + ExtraStat.AGI + ExtraStat.INT;
   if (TotalUse > GetCoreStats_Extra ()) return false;

   CoreStats_Base  += ExtraStat;
   CoreStats_Extra -= TotalUse;

   RecalcStatus ();
   return true;
}

//=============================================================================
//                 Core and Sub Stats
//=============================================================================

FRCoreStats URPlayerStatusMgrComponent::GetCoreStats_Base () const
{
   return CoreStats_Base;
}
FRCoreStats URPlayerStatusMgrComponent::GetCoreStats_Added () const
{
   return CoreStats_Added;
}
FRCoreStats URPlayerStatusMgrComponent::GetCoreStats_Total () const
{
   return GetCoreStats_Base () + GetCoreStats_Added ();
}

FRSubStats URPlayerStatusMgrComponent::GetSubStats_Base () const
{
   return SubStats_Base;
}
FRSubStats URPlayerStatusMgrComponent::GetSubStats_Added () const
{
   return SubStats_Added;
}
FRSubStats URPlayerStatusMgrComponent::GetSubStats_Total () const
{
   return GetSubStats_Base () + GetSubStats_Added ();
}

bool URPlayerStatusMgrComponent::HasStats (const FRCoreStats &RequiredStats) const
{
   return GetCoreStats_Total ().MoreThan (RequiredStats);
}

//=============================================================================
//                 Save / Load
//=============================================================================

void URPlayerStatusMgrComponent::OnSave (FBufferArchive &SaveData)
{
   // SaveData << Health << Mana << Stamina;
   // SaveData << CoreStats_Base;
}

void URPlayerStatusMgrComponent::OnLoad (FMemoryReader &LoadData)
{
   // LoadData << Health << Mana << Stamina;
   // LoadData << CoreStats_Base;
}

