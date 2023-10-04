// Copyright 2015-2023 Vagen Ayrapetyan

#include "RPlayerStatusMgrComponent.h"
#include "RWorldStatusMgr.h"
#include "RActiveStatusEffect.h"

#include "RUtilLib/RUtil.h"
#include "RUtilLib/RLog.h"
#include "RUtilLib/RCheck.h"
#include "RDamageLib/RDamageTypes.h"
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
   DOREPLIFETIME (URPlayerStatusMgrComponent, LevelUpExtraStat);
   DOREPLIFETIME (URPlayerStatusMgrComponent, SubStats_Base);
   DOREPLIFETIME (URPlayerStatusMgrComponent, SubStats_Added);
}
void URPlayerStatusMgrComponent::OnRep_Stats ()
{
   ReportStatsUpdated ();
}

void URPlayerStatusMgrComponent::ReportStatsUpdated ()
{
   if (R_IS_VALID_WORLD && OnStatsUpdated.IsBound ()) OnStatsUpdated.Broadcast ();
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
   if (!ensure (IsValid (WorldStatusMgr))) return;
   if (!ensure (IsValid (ExperienceMgr)))  return;

   float ExtraStats = WorldStatusMgr->GetLevelUpExtraGain (ExperienceMgr->GetCurrentLevel ());

   if (ExtraStats) LevelUpExtraStat += ExtraStats;

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

   ReportStatsUpdated ();
}

void URPlayerStatusMgrComponent::RecalcCoreStats ()
{
   R_RETURN_IF_NOT_ADMIN;
   FRCoreStats CoreStats_Total_New = GetCoreStats_Base ();

   // Flat
   for (const FRPassiveStatusEffectWithTag &ItEffect : GetPassiveEffectsWithTag ()) {
      if (ItEffect.Value.EffectTarget == ERStatusEffectTarget::STR) CoreStats_Total_New.STR += ItEffect.Value.Flat;
      if (ItEffect.Value.EffectTarget == ERStatusEffectTarget::AGI) CoreStats_Total_New.AGI += ItEffect.Value.Flat;
      if (ItEffect.Value.EffectTarget == ERStatusEffectTarget::INT) CoreStats_Total_New.INT += ItEffect.Value.Flat;
   }
   // Percentage
   for (const FRPassiveStatusEffectWithTag &ItEffect : GetPassiveEffectsWithTag ()) {
      if (ItEffect.Value.EffectTarget == ERStatusEffectTarget::STR) CoreStats_Total_New.STR *= ((100. + ItEffect.Value.Percent) / 100.);
      if (ItEffect.Value.EffectTarget == ERStatusEffectTarget::AGI) CoreStats_Total_New.AGI *= ((100. + ItEffect.Value.Percent) / 100.);
      if (ItEffect.Value.EffectTarget == ERStatusEffectTarget::INT) CoreStats_Total_New.INT *= ((100. + ItEffect.Value.Percent) / 100.);
   }
   CoreStats_Added = CoreStats_Total_New - GetCoreStats_Base ();
}

void URPlayerStatusMgrComponent::RecalcSubStats ()
{
   R_RETURN_IF_NOT_ADMIN;
   if (!ensure (IsValid (WorldStatusMgr))) return;

   FRCoreStats StatsTotal = GetCoreStats_Total ();
   float EvasionTotal     = WorldStatusMgr->GetAgiToEvasion     (StatsTotal.AGI);
   float CriticalTotal    = WorldStatusMgr->GetAgiToCritical    (StatsTotal.AGI);
   float AttackSpeedTotal = WorldStatusMgr->GetAgiToAttackSpeed (StatsTotal.AGI);
   float MoveSpeedTotal   = 0;

   // Flat
   for (const FRPassiveStatusEffectWithTag &ItEffect : GetPassiveEffectsWithTag ()) {
      if (ItEffect.Value.EffectTarget == ERStatusEffectTarget::Evasion)     EvasionTotal     += ItEffect.Value.Flat;
      if (ItEffect.Value.EffectTarget == ERStatusEffectTarget::Critical)    CriticalTotal    += ItEffect.Value.Flat;
      if (ItEffect.Value.EffectTarget == ERStatusEffectTarget::AttackSpeed) AttackSpeedTotal += ItEffect.Value.Flat;
      if (ItEffect.Value.EffectTarget == ERStatusEffectTarget::MoveSpeed)   MoveSpeedTotal   += ItEffect.Value.Flat;
   }
   // Percentage
   for (const FRPassiveStatusEffectWithTag &ItEffect : GetPassiveEffectsWithTag ()) {
      if (ItEffect.Value.EffectTarget == ERStatusEffectTarget::Evasion)     EvasionTotal     *= ((100. + ItEffect.Value.Percent) / 100.);
      if (ItEffect.Value.EffectTarget == ERStatusEffectTarget::Critical)    CriticalTotal    *= ((100. + ItEffect.Value.Percent) / 100.);
      if (ItEffect.Value.EffectTarget == ERStatusEffectTarget::AttackSpeed) AttackSpeedTotal *= ((100. + ItEffect.Value.Percent) / 100.);
      if (ItEffect.Value.EffectTarget == ERStatusEffectTarget::MoveSpeed)   MoveSpeedTotal   *= ((100. + ItEffect.Value.Percent) / 100.);
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
   if (!ensure (IsValid (WorldStatusMgr))) return;

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
      if (ItEffect.Value.EffectTarget == ERStatusEffectTarget::HealthMax)    Health.Max    += ItEffect.Value.Flat;
      if (ItEffect.Value.EffectTarget == ERStatusEffectTarget::HealthRegen)  Health.Regen  += ItEffect.Value.Flat;
      if (ItEffect.Value.EffectTarget == ERStatusEffectTarget::StaminaMax)   Stamina.Max   += ItEffect.Value.Flat;
      if (ItEffect.Value.EffectTarget == ERStatusEffectTarget::StaminaRegen) Stamina.Regen += ItEffect.Value.Flat;
      if (ItEffect.Value.EffectTarget == ERStatusEffectTarget::ManaMax)      Mana.Max      += ItEffect.Value.Flat;
      if (ItEffect.Value.EffectTarget == ERStatusEffectTarget::ManaRegen)    Mana.Regen    += ItEffect.Value.Flat;
   }
   // Percentage
   for (const FRPassiveStatusEffectWithTag &ItEffect : GetPassiveEffectsWithTag ()) {
      if (ItEffect.Value.EffectTarget == ERStatusEffectTarget::HealthMax)    Health.Max    *= ((100. + ItEffect.Value.Flat) / 100.);
      if (ItEffect.Value.EffectTarget == ERStatusEffectTarget::HealthRegen)  Health.Regen  *= ((100. + ItEffect.Value.Flat) / 100.);
      if (ItEffect.Value.EffectTarget == ERStatusEffectTarget::StaminaMax)   Stamina.Max   *= ((100. + ItEffect.Value.Flat) / 100.);
      if (ItEffect.Value.EffectTarget == ERStatusEffectTarget::StaminaRegen) Stamina.Regen *= ((100. + ItEffect.Value.Flat) / 100.);
      if (ItEffect.Value.EffectTarget == ERStatusEffectTarget::ManaMax)      Mana.Max      *= ((100. + ItEffect.Value.Flat) / 100.);
      if (ItEffect.Value.EffectTarget == ERStatusEffectTarget::ManaRegen)    Mana.Regen    *= ((100. + ItEffect.Value.Flat) / 100.);
   }

   // Clamp current values;
   Health.Current  = FMath::Clamp (Health.Current,  0, Health.Max);
   Mana.Current    = FMath::Clamp (Mana.Current,    0, Mana.Max);
   Stamina.Current = FMath::Clamp (Stamina.Current, 0, Stamina.Max);
}

//=============================================================================
//                 Level Up Extra stat Points
//=============================================================================

float URPlayerStatusMgrComponent::GetLevelUpExtraStat () const
{
   return LevelUpExtraStat;
}

bool URPlayerStatusMgrComponent::AssignLevelUpExtraStat (FRCoreStats StatValues)
{
   float TotalUse = StatValues.STR + StatValues.AGI + StatValues.INT;
   if (TotalUse > GetLevelUpExtraStat ()) return false;

   CoreStats_Base   += StatValues;
   LevelUpExtraStat -= TotalUse;

   RecalcStatus ();
   return true;
}

void URPlayerStatusMgrComponent::AssignLevelUpExtraStat_Server_Implementation (FRCoreStats StatValues)
{
   AssignLevelUpExtraStat (StatValues);
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
   SaveData << Health << Mana << Stamina;
   SaveData << CoreStats_Base << LevelUpExtraStat;
}

void URPlayerStatusMgrComponent::OnLoad (FMemoryReader &LoadData)
{
   LoadData << Health << Mana << Stamina;
   LoadData << CoreStats_Base << LevelUpExtraStat;

   ReportStatsUpdated ();
}

