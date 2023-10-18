// Copyright 2015-2023 Vagen Ayrapetyan

#include "RWorldStatusMgr.h"
#include "RUtilLib/RLog.h"
#include "RUtilLib/RUtil.h"
#include "RUtilLib/RCheck.h"

//=============================================================================
//                   Static calls
//=============================================================================

URWorldStatusMgr* URWorldStatusMgr::GetInstance (const UObject* WorldContextObject)
{
   return URUtil::GetWorldInstance<URWorldStatusMgr> (WorldContextObject);
}

//=============================================================================
//                   Member calls
//=============================================================================

URWorldStatusMgr::URWorldStatusMgr ()
{
   bWantsInitializeComponent = true;

   //==========================================================================
   //                   Stat growth
   //==========================================================================

   FRichCurve* LevelUpStrGainCurveData = LevelUpStrGainCurve.GetRichCurve ();
   LevelUpStrGainCurveData->AddKey (  0, 3.0);
   LevelUpStrGainCurveData->AddKey ( 10, 2.8);
   LevelUpStrGainCurveData->AddKey ( 20, 2.5);
   LevelUpStrGainCurveData->AddKey ( 50, 2.0);
   LevelUpStrGainCurveData->AddKey (100, 1.5);
   LevelUpStrGainCurveData->AddKey (200, 1.0);

   FRichCurve* LevelUpAgiGainCurveData = LevelUpAgiGainCurve.GetRichCurve ();
   LevelUpAgiGainCurveData->AddKey (  0, 3.0);
   LevelUpAgiGainCurveData->AddKey ( 10, 2.8);
   LevelUpAgiGainCurveData->AddKey ( 20, 2.5);
   LevelUpAgiGainCurveData->AddKey ( 50, 2.0);
   LevelUpAgiGainCurveData->AddKey (100, 1.5);
   LevelUpAgiGainCurveData->AddKey (200, 1.0);

   FRichCurve* LevelUpIntGainCurveData = LevelUpIntGainCurve.GetRichCurve ();
   LevelUpIntGainCurveData->AddKey (  0, 3.0);
   LevelUpIntGainCurveData->AddKey ( 10, 2.8);
   LevelUpIntGainCurveData->AddKey ( 20, 2.5);
   LevelUpIntGainCurveData->AddKey ( 50, 2.0);
   LevelUpIntGainCurveData->AddKey (100, 1.5);
   LevelUpIntGainCurveData->AddKey (200, 1.0);

   //==========================================================================
   //                 Extra Stat
   //==========================================================================

   FRichCurve* LevelUpExtraGainCurveData = LevelUpExtraGainCurve.GetRichCurve ();
   LevelUpExtraGainCurveData->AddKey (  0, 3.0);
   LevelUpExtraGainCurveData->AddKey ( 10, 2.8);
   LevelUpExtraGainCurveData->AddKey ( 20, 2.5);
   LevelUpExtraGainCurveData->AddKey ( 50, 2.0);
   LevelUpExtraGainCurveData->AddKey (100, 1.5);
   LevelUpExtraGainCurveData->AddKey (200, 1.0);

   //==========================================================================
   //                 Status Value Curves
   //==========================================================================

   // Health MAX
   FRichCurve* StrToHealthMaxData = StrToHealthMaxCurve.GetRichCurve ();
   StrToHealthMaxData->AddKey (   0,  200); // Minimum
   StrToHealthMaxData->AddKey (   1,  210);
   StrToHealthMaxData->AddKey (  10,  300);
   StrToHealthMaxData->AddKey ( 100, 1000);
   StrToHealthMaxData->AddKey (1000, 5000);
   StrToHealthMaxData->AddKey (5000, 9999);

   // Health Regen
   FRichCurve* StrToHealthRegenData = StrToHealthRegenCurve.GetRichCurve ();
   StrToHealthRegenData->AddKey (   0,   1  ); // Minimum
   StrToHealthRegenData->AddKey (   1,   1.1);
   StrToHealthRegenData->AddKey (  10,   2  );
   StrToHealthRegenData->AddKey ( 100,  10  );
   StrToHealthRegenData->AddKey (1000,  50  );
   StrToHealthRegenData->AddKey (5000, 100  );

   // Stamina MAX
   FRichCurve* AgiToStaminaMaxData = AgiToStaminaMaxCurve.GetRichCurve ();
   AgiToStaminaMaxData->AddKey (   0, 100); // Minimum
   AgiToStaminaMaxData->AddKey (   1, 102);
   AgiToStaminaMaxData->AddKey (  10, 120);
   AgiToStaminaMaxData->AddKey ( 100, 200);
   AgiToStaminaMaxData->AddKey (1000, 400);
   AgiToStaminaMaxData->AddKey (5000, 500);

   // Stamina Regen
   FRichCurve* AgiToStaminaRegenData = AgiToStaminaRegenCurve.GetRichCurve ();
   AgiToStaminaRegenData->AddKey (   0,   10  ); // Minimum
   AgiToStaminaRegenData->AddKey (   1,   10.2);
   AgiToStaminaRegenData->AddKey (  10,   12  );
   AgiToStaminaRegenData->AddKey ( 100,   20  );
   AgiToStaminaRegenData->AddKey (1000,   40  );
   AgiToStaminaRegenData->AddKey (5000,   50  );

   // Mana MAX
   FRichCurve* IntToManaMaxData = IntToManaMaxCurve.GetRichCurve ();
   IntToManaMaxData->AddKey (   0,   50); // Minimum
   IntToManaMaxData->AddKey (   1,   60);
   IntToManaMaxData->AddKey (  10,  150);
   IntToManaMaxData->AddKey ( 100, 1000);
   IntToManaMaxData->AddKey (1000, 5000);
   IntToManaMaxData->AddKey (5000, 9999);

   // Mana Regen
   FRichCurve* IntToManaRegenData = IntToManaRegenCurve.GetRichCurve ();
   IntToManaRegenData->AddKey (   0,   0  ); // Minimum
   IntToManaRegenData->AddKey (   1,   0.1);
   IntToManaRegenData->AddKey (  10,   1  );
   IntToManaRegenData->AddKey ( 100,  10  );
   IntToManaRegenData->AddKey (1000,  50  );
   IntToManaRegenData->AddKey (5000, 100  );

   //==========================================================================
   //                 Extra Stat Curves
   //==========================================================================

   // Critical
   FRichCurve* AgiToCriticalData = AgiToCriticalCurve.GetRichCurve ();
   AgiToCriticalData->AddKey (   0,   0); // Minimum
   AgiToCriticalData->AddKey (   1,   1);
   AgiToCriticalData->AddKey (  10,   5);
   AgiToCriticalData->AddKey ( 100,  20);
   AgiToCriticalData->AddKey (1000,  30);
   AgiToCriticalData->AddKey (5000,  40);

   // Evasion
   FRichCurve* AgiToEvasionData = AgiToEvasionCurve.GetRichCurve ();
   AgiToEvasionData->AddKey (   0,    0); // Minimum
   AgiToEvasionData->AddKey (   1,    1);
   AgiToEvasionData->AddKey (  10,    5);
   AgiToEvasionData->AddKey ( 100,   20);
   AgiToEvasionData->AddKey (1000,   35);
   AgiToEvasionData->AddKey (5000,   40);

   // Attack Speed.
   // Per weapon curve for AttackSpeed -> AttacksPerSecond?
   // Min/Max for each weapon.
   FRichCurve* AgiToAttackSpeedData = AgiToAttackSpeedCurve.GetRichCurve ();
   AgiToAttackSpeedData->AddKey (0,       0); // Minimum
   AgiToAttackSpeedData->AddKey (5000, 5000); // Maximum
}


void URWorldStatusMgr::InitializeComponent ()
{
   Super::InitializeComponent ();

   // --- Parse Table and create Map for fast search
   if (StatusEffectTable) {

      MapStatusEffect.Empty ();
      FString TablePath = URUtil::GetTablePath (StatusEffectTable);
      FString ContextString;
      TArray<FName> RowNames = StatusEffectTable->GetRowNames ();
      for (const FName& ItRowName : RowNames) {
         FRActiveStatusEffectInfo* ItRow = StatusEffectTable->FindRow<FRActiveStatusEffectInfo> (ItRowName, ContextString);

         if (!ItRow) {
            R_LOG_PRINTF ("Invalid FRActiveStatusEffectInfo in row [%s] table [%s]", *ItRowName.ToString (), *TablePath);
            continue;
         }

         if (ItRow->EffectClass.IsNull ()) {
            R_LOG_PRINTF ("Invalid Effect Class in row [%s] table [%s]", *ItRowName.ToString (), *TablePath);
            continue;
         }

         MapStatusEffect.Add (ItRow->EffectClass.ToString (), *ItRow);
      }
   }
}

void URWorldStatusMgr::BeginPlay ()
{
   Super::BeginPlay ();
}

FRActiveStatusEffectInfo URWorldStatusMgr::GetEffectInfo (const URActiveStatusEffect* StatusEffect) const
{
   FRActiveStatusEffectInfo Result;
   if (ensure (StatusEffect)) {
      FString StatusClassPath = StatusEffect->GetClass ()->GetPathName ();
      if (MapStatusEffect.Contains (StatusClassPath)) {
         Result = MapStatusEffect[StatusClassPath];
      } else {
         R_LOG_PRINTF ("Error. [%s] Effect not found in [%s]",
                       *StatusClassPath,
                       *URUtil::GetTablePath (StatusEffectTable));
      }
   }

   return Result;
}

//=============================================================================
//                Status Effect
//=============================================================================

void URWorldStatusMgr::ReportStatusEffectStart (URActiveStatusEffect* Effect)
{
   if (!ensure (Effect)) return;
   if (R_IS_VALID_WORLD && OnStatusEffectStart.IsBound ()) OnStatusEffectStart.Broadcast (Effect);
}

void URWorldStatusMgr::ReportStatusEffectStop (URActiveStatusEffect* Effect)
{
   if (!ensure (Effect)) return;
   if (R_IS_VALID_WORLD && OnStatusEffectStop.IsBound ()) OnStatusEffectStop.Broadcast (Effect);
}

void URWorldStatusMgr::ReportStatusEffectRefresh (URActiveStatusEffect* Effect)
{
   if (!ensure (Effect)) return;
   if (R_IS_VALID_WORLD && OnStatusEffectRefresh.IsBound ()) OnStatusEffectRefresh.Broadcast (Effect);
}

void URWorldStatusMgr::ReportStatusEffectEnd (URActiveStatusEffect* Effect)
{
   if (!ensure (Effect)) return;
   if (R_IS_VALID_WORLD && OnStatusEffectEnd.IsBound ()) OnStatusEffectEnd.Broadcast (Effect);
}

//=============================================================================
//                Level Function
//=============================================================================

float URWorldStatusMgr::GetLevelUpExtraGain (int CurrentLevel) const
{
   return URUtil::GetRuntimeFloatCurveValue (LevelUpExtraGainCurve, CurrentLevel);
}

FRCoreStats URWorldStatusMgr::GetLevelUpStatGain (int CurrentLevel) const
{
   FRCoreStats DeltaStat;
   DeltaStat.STR = URUtil::GetRuntimeFloatCurveValue (LevelUpStrGainCurve, CurrentLevel);
   DeltaStat.AGI = URUtil::GetRuntimeFloatCurveValue (LevelUpAgiGainCurve, CurrentLevel);
   DeltaStat.INT = URUtil::GetRuntimeFloatCurveValue (LevelUpIntGainCurve, CurrentLevel);
   return DeltaStat;
}

//=============================================================================
//                Balance Function
//=============================================================================

// --- STR

float URWorldStatusMgr::GetStrToHealthMax    (float STR) const
{
   return URUtil::GetRuntimeFloatCurveValue (StrToHealthMaxCurve, STR);
}

float URWorldStatusMgr::GetStrToHealthRegen  (float STR) const
{
   return URUtil::GetRuntimeFloatCurveValue (StrToHealthRegenCurve, STR);
}

// --- AGI

float URWorldStatusMgr::GetAgiToStaminaMax   (float AGI) const
{
   return URUtil::GetRuntimeFloatCurveValue (AgiToStaminaMaxCurve, AGI);
}

float URWorldStatusMgr::GetAgiToStaminaRegen (float AGI) const
{
   return URUtil::GetRuntimeFloatCurveValue (AgiToStaminaRegenCurve, AGI);
}

float URWorldStatusMgr::GetAgiToEvasion      (float AGI) const
{
   return URUtil::GetRuntimeFloatCurveValue (AgiToEvasionCurve, AGI);
}

float URWorldStatusMgr::GetAgiToCritical     (float AGI) const
{
   return URUtil::GetRuntimeFloatCurveValue (AgiToCriticalCurve, AGI);
}

float URWorldStatusMgr::GetAgiToAttackSpeed  (float AGI) const
{
   return URUtil::GetRuntimeFloatCurveValue (AgiToAttackSpeedCurve, AGI);
}

// --- INT

float URWorldStatusMgr::GetIntToManaMax      (float INT) const
{
   return URUtil::GetRuntimeFloatCurveValue (IntToManaMaxCurve, INT);
}

float URWorldStatusMgr::GetIntToManaRegen    (float INT) const
{
   return URUtil::GetRuntimeFloatCurveValue (IntToManaRegenCurve, INT);
}

