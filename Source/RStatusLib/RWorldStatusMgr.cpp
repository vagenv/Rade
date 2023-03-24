// Copyright 2015-2023 Vagen Ayrapetyan

#include "RWorldStatusMgr.h"
#include "RUtilLib/RLog.h"
#include "RUtilLib/RUtil.h"
#include "RUtilLib/RCheck.h"

//=============================================================================
//                   Static calls
//=============================================================================

URWorldStatusMgr* URWorldStatusMgr::GetInstance (UObject* WorldContextObject)
{
   if (!ensure (WorldContextObject)) return nullptr;

   UWorld *World = WorldContextObject->GetWorld ();
   if (!ensure (World)) return nullptr;

   AGameStateBase *GameState = World->GetGameState ();
   if (!ensure (GameState)) return nullptr;

   return GameState->FindComponentByClass<URWorldStatusMgr>();
}

//=============================================================================
//                   Member calls
//=============================================================================

URWorldStatusMgr::URWorldStatusMgr ()
{
   //==========================================================================
   //                 Stat growth
   //==========================================================================

   FRichCurve* LvlStrGainCurveData = LvlStrGainCurve.GetRichCurve ();
   LvlStrGainCurveData->AddKey (  0, 3.0);
   LvlStrGainCurveData->AddKey ( 10, 2.8);
   LvlStrGainCurveData->AddKey ( 20, 2.5);
   LvlStrGainCurveData->AddKey ( 50, 2.0);
   LvlStrGainCurveData->AddKey (100, 1.5);
   LvlStrGainCurveData->AddKey (200, 1.0);

   FRichCurve* LvlAgiGainCurveData = LvlAgiGainCurve.GetRichCurve ();
   LvlAgiGainCurveData->AddKey (  0, 3.0);
   LvlAgiGainCurveData->AddKey ( 10, 2.8);
   LvlAgiGainCurveData->AddKey ( 20, 2.5);
   LvlAgiGainCurveData->AddKey ( 50, 2.0);
   LvlAgiGainCurveData->AddKey (100, 1.5);
   LvlAgiGainCurveData->AddKey (200, 1.0);

   FRichCurve* LvlIntGainCurveData = LvlIntGainCurve.GetRichCurve ();
   LvlIntGainCurveData->AddKey (  0, 3.0);
   LvlIntGainCurveData->AddKey ( 10, 2.8);
   LvlIntGainCurveData->AddKey ( 20, 2.5);
   LvlIntGainCurveData->AddKey ( 50, 2.0);
   LvlIntGainCurveData->AddKey (100, 1.5);
   LvlIntGainCurveData->AddKey (200, 1.0);

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

void URWorldStatusMgr::BeginPlay ()
{
   Super::BeginPlay ();
}

//=============================================================================
//                Status Effect
//=============================================================================


void URWorldStatusMgr::ReportStatusEffect (URActiveStatusEffect* Effect, AActor* Causer, AActor* Victim)
{
   R_RETURN_IF_NOT_ADMIN;
   if (!ensure (Effect)) return;
   // if (!ensure (Causer)) return;
   if (!ensure (Victim)) return;
   OnStatusEffectApplied.Broadcast (Effect, Causer, Victim);
}

//=============================================================================
//                Level Function
//=============================================================================


FRCoreStats URWorldStatusMgr::GetLevelUpStatGain (int CurrentLevel) const
{
   FRCoreStats DeltaStat;
   DeltaStat.STR = URUtilLibrary::GetRuntimeFloatCurveValue (LvlStrGainCurve, CurrentLevel);
   DeltaStat.AGI = URUtilLibrary::GetRuntimeFloatCurveValue (LvlAgiGainCurve, CurrentLevel);
   DeltaStat.INT = URUtilLibrary::GetRuntimeFloatCurveValue (LvlIntGainCurve, CurrentLevel);
   return DeltaStat;
}

//=============================================================================
//                Balance Function
//=============================================================================

// --- STR

float URWorldStatusMgr::GetStrToHealthMax    (float STR) const
{
   return URUtilLibrary::GetRuntimeFloatCurveValue (StrToHealthMaxCurve, STR);
}

float URWorldStatusMgr::GetStrToHealthRegen  (float STR) const
{
   return URUtilLibrary::GetRuntimeFloatCurveValue (StrToHealthRegenCurve, STR);
}

// --- AGI

float URWorldStatusMgr::GetAgiToStaminaMax   (float AGI) const
{
   return URUtilLibrary::GetRuntimeFloatCurveValue (AgiToStaminaMaxCurve, AGI);
}

float URWorldStatusMgr::GetAgiToStaminaRegen (float AGI) const
{
   return URUtilLibrary::GetRuntimeFloatCurveValue (AgiToStaminaRegenCurve, AGI);
}

float URWorldStatusMgr::GetAgiToEvasion      (float AGI) const
{
   return URUtilLibrary::GetRuntimeFloatCurveValue (AgiToEvasionCurve, AGI);
}

float URWorldStatusMgr::GetAgiToCritical     (float AGI) const
{
   return URUtilLibrary::GetRuntimeFloatCurveValue (AgiToCriticalCurve, AGI);
}

float URWorldStatusMgr::GetAgiToAttackSpeed  (float AGI) const
{
   return URUtilLibrary::GetRuntimeFloatCurveValue (AgiToAttackSpeedCurve, AGI);
}

// --- INT

float URWorldStatusMgr::GetIntToManaMax      (float INT) const
{
   return URUtilLibrary::GetRuntimeFloatCurveValue (IntToManaMaxCurve, INT);
}

float URWorldStatusMgr::GetIntToManaRegen    (float INT) const
{
   return URUtilLibrary::GetRuntimeFloatCurveValue (IntToManaRegenCurve, INT);
}

