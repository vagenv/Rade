// Copyright 2015-2023 Vagen Ayrapetyan

#include "RPlayerStatusMgrComponent.h"
#include "RWorldStatusMgr.h"
#include "RActiveStatusEffect.h"

#include "RUtilLib/RUtil.h"
#include "RUtilLib/RLog.h"
#include "RUtilLib/RCheck.h"
#include "RUtilLib/RTimer.h"

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

   if (R_IS_NET_ADMIN) {
      FTimerHandle TempHandle;
      RTIMER_START (TempHandle,
                    this,
                    &URPlayerStatusMgrComponent::RecalcStatus,
                    1, false);

      // Recalc on revive
      OnRevive.AddDynamic (this, &URPlayerStatusMgrComponent::RecalcStatus);
   }

   ConnectToExperienceMgr ();
   ConnectToSaveMgr ();
}

void URPlayerStatusMgrComponent::ConnectToExperienceMgr ()
{
   WorldExperienceMgr = URUtil::GetComponent<URExperienceMgrComponent> (GetOwner ());
   if (!WorldExperienceMgr.IsValid ()) {
      FTimerHandle RetryHandle;
      RTIMER_START (RetryHandle,
                    this, &URPlayerStatusMgrComponent::ConnectToExperienceMgr,
                    1, false);
      return;
   }

   // Only admin can calculate level up
   if (R_IS_NET_ADMIN) {
      WorldExperienceMgr->OnLevelUp.AddDynamic (this, &URPlayerStatusMgrComponent::LeveledUp);
   }
}

//=============================================================================
//                 Level up
//=============================================================================

void URPlayerStatusMgrComponent::LeveledUp ()
{
   R_RETURN_IF_NOT_ADMIN;
   if (!WorldStatusMgr.IsValid () || !WorldExperienceMgr.IsValid ()) return;

   float ExtraStats = WorldStatusMgr->GetLevelUpExtraGain (WorldExperienceMgr->GetCurrentLevel ());

   if (ExtraStats) LevelUpExtraStat += ExtraStats;

   FRCoreStats DeltaStats = WorldStatusMgr->GetLevelUpStatGain (WorldExperienceMgr->GetCurrentLevel ());
   if (!DeltaStats.IsEmpty ()) CoreStats_Base += DeltaStats;
   if (!DeltaStats.IsEmpty () || ExtraStats)
      RecalcStatus ();
}

//=============================================================================
//                 Recalc stats
//=============================================================================

void URPlayerStatusMgrComponent::RecalcStatus ()
{
   R_RETURN_IF_NOT_ADMIN;
   if (!WorldStatusMgr.IsValid ()) return;

   const TArray<FRPassiveStatusEffect> &Effects = GetPassiveEffects_Flat ();

   // --- Core stats
   {
      FRCoreStats CoreStats_Total_New = GetCoreStats_Base ();

      // Flat
      for (const FRPassiveStatusEffect &ItEffect : Effects) {
         if (ItEffect.EffectTarget == ERStatusEffectTarget::STR) CoreStats_Total_New.STR += ItEffect.Flat;
         if (ItEffect.EffectTarget == ERStatusEffectTarget::AGI) CoreStats_Total_New.AGI += ItEffect.Flat;
         if (ItEffect.EffectTarget == ERStatusEffectTarget::INT) CoreStats_Total_New.INT += ItEffect.Flat;
      }
      // Percentage
      for (const FRPassiveStatusEffect &ItEffect : Effects) {
         if (ItEffect.EffectTarget == ERStatusEffectTarget::STR) CoreStats_Total_New.STR *= ((100. + ItEffect.Percent) / 100.);
         if (ItEffect.EffectTarget == ERStatusEffectTarget::AGI) CoreStats_Total_New.AGI *= ((100. + ItEffect.Percent) / 100.);
         if (ItEffect.EffectTarget == ERStatusEffectTarget::INT) CoreStats_Total_New.INT *= ((100. + ItEffect.Percent) / 100.);
      }
      CoreStats_Added = CoreStats_Total_New - GetCoreStats_Base ();
   }

   // --- Sub stats
   {
      FRCoreStats StatsTotal = GetCoreStats_Total ();
      float EvasionTotal     = WorldStatusMgr->GetAgiToEvasion     (StatsTotal.AGI);
      float CriticalTotal    = WorldStatusMgr->GetAgiToCritical    (StatsTotal.AGI);
      float AttackSpeedTotal = WorldStatusMgr->GetAgiToAttackSpeed (StatsTotal.AGI);
      float AttackPowerTotal = 1;
      float MoveSpeedTotal   = 1;

      // Flat
      for (const FRPassiveStatusEffect &ItEffect : Effects) {
         if (ItEffect.EffectTarget == ERStatusEffectTarget::Evasion)     EvasionTotal     += ItEffect.Flat;
         if (ItEffect.EffectTarget == ERStatusEffectTarget::Critical)    CriticalTotal    += ItEffect.Flat;
         if (ItEffect.EffectTarget == ERStatusEffectTarget::AttackSpeed) AttackSpeedTotal += ItEffect.Flat;
         if (ItEffect.EffectTarget == ERStatusEffectTarget::MoveSpeed)   MoveSpeedTotal   += ItEffect.Flat;
      }
      // Percentage
      for (const FRPassiveStatusEffect &ItEffect : Effects) {
         if (ItEffect.EffectTarget == ERStatusEffectTarget::Evasion)     EvasionTotal     *= ((100. + ItEffect.Percent) / 100.);
         if (ItEffect.EffectTarget == ERStatusEffectTarget::Critical)    CriticalTotal    *= ((100. + ItEffect.Percent) / 100.);
         if (ItEffect.EffectTarget == ERStatusEffectTarget::AttackSpeed) AttackSpeedTotal *= ((100. + ItEffect.Percent) / 100.);
         if (ItEffect.EffectTarget == ERStatusEffectTarget::MoveSpeed)   MoveSpeedTotal   *= ((100. + ItEffect.Percent) / 100.);
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
      SubStats_Base.AttackPower  = 0;
      SubStats_Added.AttackPower = 0;

      CriticalChance = CriticalTotal;
      EvasionChance  = EvasionTotal;
   }

   // --- Status
   {
      FRCoreStats StatsTotal = GetCoreStats_Total ();
      Health.Max     = WorldStatusMgr->GetStrToHealthMax    (StatsTotal.STR);
      Health.Regen   = WorldStatusMgr->GetStrToHealthRegen  (StatsTotal.STR);
      Stamina.Max    = WorldStatusMgr->GetAgiToStaminaMax   (StatsTotal.AGI);
      Stamina.Regen  = WorldStatusMgr->GetAgiToStaminaRegen (StatsTotal.AGI);
      Mana.Max       = WorldStatusMgr->GetIntToManaMax      (StatsTotal.INT);
      Mana.Regen     = WorldStatusMgr->GetIntToManaRegen    (StatsTotal.INT);

      // Flat
      for (const FRPassiveStatusEffect &ItEffect : Effects) {
         if (ItEffect.EffectTarget == ERStatusEffectTarget::HealthMax)    Health.Max    += ItEffect.Flat;
         if (ItEffect.EffectTarget == ERStatusEffectTarget::HealthRegen)  Health.Regen  += ItEffect.Flat;
         if (ItEffect.EffectTarget == ERStatusEffectTarget::StaminaMax)   Stamina.Max   += ItEffect.Flat;
         if (ItEffect.EffectTarget == ERStatusEffectTarget::StaminaRegen) Stamina.Regen += ItEffect.Flat;
         if (ItEffect.EffectTarget == ERStatusEffectTarget::ManaMax)      Mana.Max      += ItEffect.Flat;
         if (ItEffect.EffectTarget == ERStatusEffectTarget::ManaRegen)    Mana.Regen    += ItEffect.Flat;
      }
      // Percentage
      for (const FRPassiveStatusEffect &ItEffect : Effects) {
         if (ItEffect.EffectTarget == ERStatusEffectTarget::HealthMax)    Health.Max    *= ((100. + ItEffect.Flat) / 100.);
         if (ItEffect.EffectTarget == ERStatusEffectTarget::HealthRegen)  Health.Regen  *= ((100. + ItEffect.Flat) / 100.);
         if (ItEffect.EffectTarget == ERStatusEffectTarget::StaminaMax)   Stamina.Max   *= ((100. + ItEffect.Flat) / 100.);
         if (ItEffect.EffectTarget == ERStatusEffectTarget::StaminaRegen) Stamina.Regen *= ((100. + ItEffect.Flat) / 100.);
         if (ItEffect.EffectTarget == ERStatusEffectTarget::ManaMax)      Mana.Max      *= ((100. + ItEffect.Flat) / 100.);
         if (ItEffect.EffectTarget == ERStatusEffectTarget::ManaRegen)    Mana.Regen    *= ((100. + ItEffect.Flat) / 100.);
      }

      // Clamp current values;
      Health.Current  = FMath::Clamp (Health.Current,  0, Health.Max);
      Mana.Current    = FMath::Clamp (Mana.Current,    0, Mana.Max);
      Stamina.Current = FMath::Clamp (Stamina.Current, 0, Stamina.Max);

      SetHealth (Health);
      SetStamina (Stamina);
      SetMana (Mana);
   }

   ReportStatsUpdated ();
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
   if (TotalUse > 0 && TotalUse > GetLevelUpExtraStat ()) return false;

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

void URPlayerStatusMgrComponent::ConnectToSaveMgr ()
{
	if (!bSaveLoad) return;

   // Careful with collision of 'UniqueSaveId'
   FString UniqueSaveId = GetOwner ()->GetName () + "_PlayerStatusMgr";

	if (!InitSaveInterface (this, UniqueSaveId)) {
		FTimerHandle RetryHandle;
		RTIMER_START (RetryHandle, this, &URPlayerStatusMgrComponent::ConnectToSaveMgr, 1, false);
	}
}
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

