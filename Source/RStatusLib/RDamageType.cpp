// Copyright 2015-2023 Vagen Ayrapetyan

#include "RDamageType.h"
#include "RUtilLib/RLog.h"

URDamageType::URDamageType ()
{
   // Default damage Curve
   FRichCurve* ResistanceToDamageCurveData = ResistanceToDamage.GetRichCurve ();

   // Resistance to reduction percent
   ResistanceToDamageCurveData->AddKey (   0,  0);
   ResistanceToDamageCurveData->AddKey (  10,  5);
   ResistanceToDamageCurveData->AddKey (  20, 10);
   ResistanceToDamageCurveData->AddKey (  40, 20);
   ResistanceToDamageCurveData->AddKey (  80, 30);
   ResistanceToDamageCurveData->AddKey ( 160, 40);
   ResistanceToDamageCurveData->AddKey ( 320, 50);
   ResistanceToDamageCurveData->AddKey ( 640, 60);
   ResistanceToDamageCurveData->AddKey (1280, 70);
   ResistanceToDamageCurveData->AddKey (2560, 80);
   ResistanceToDamageCurveData->AddKey (5120, 90);
}

float URDamageType::CalcDamage (float Damage, float Resistance) const
{
   const FRichCurve* ResistanceToDamageCurveData = ResistanceToDamage.GetRichCurveConst ();
   if (!ensure (ResistanceToDamageCurveData)) return Damage;
   return (100 - ResistanceToDamageCurveData->Eval (Resistance)) * Damage / 100;
}

URDamageType_Fall::URDamageType_Fall ()
{
   // Exception transform velocity to damage
   FRichCurve* FallDamageCurveData = FallDamageCurve.GetRichCurve ();
   FallDamageCurveData->Reset ();
   FallDamageCurveData->AddKey (1000, 0); // Minimum
   FallDamageCurveData->AddKey (1500, 40);
   FallDamageCurveData->AddKey (2000, 100);
}

float URDamageType_Fall::CalcDamage (float Velocity, float Resistance) const
{
   const FRichCurve* ResistanceToDamageCurveData = ResistanceToDamage.GetRichCurveConst ();
   if (!ensure (ResistanceToDamageCurveData)) return 0;

   const FRichCurve* FallDamageCurveData = FallDamageCurve.GetRichCurveConst ();
   if (!ensure (FallDamageCurveData)) return 0;

   return (100 - ResistanceToDamageCurveData->Eval (Resistance))
         * FallDamageCurveData->Eval (Velocity)
         / 100;
}

