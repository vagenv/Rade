// Copyright 2015-2023 Vagen Ayrapetyan

#include "RDamageType.h"
#include "RUtilLib/RLog.h"

//=============================================================================
//                 Resistance
//=============================================================================



//=============================================================================
//                 Damage
//=============================================================================

URDamageType::URDamageType ()
{
   // // Default damage Curve
   // FRichCurve* ResistanceToDamageCurveData = ResistanceToDamage.GetRichCurve ();

   // // Resistance to reduction percent
   // ResistanceToDamageCurveData->AddKey (   0,  0);
   // ResistanceToDamageCurveData->AddKey (  10,  5);
   // ResistanceToDamageCurveData->AddKey (  20, 10);
   // ResistanceToDamageCurveData->AddKey (  40, 20);
   // ResistanceToDamageCurveData->AddKey (  80, 30);
   // ResistanceToDamageCurveData->AddKey ( 160, 40);
   // ResistanceToDamageCurveData->AddKey ( 320, 50);
   // ResistanceToDamageCurveData->AddKey ( 640, 60);
   // ResistanceToDamageCurveData->AddKey (1280, 70);
   // ResistanceToDamageCurveData->AddKey (2560, 80);
   // ResistanceToDamageCurveData->AddKey (5120, 90);
}

float URDamageType::CalcDamage (float Damage, const FRDamageResistance &Resistance) const
{
   float NewDamage = (Damage - Resistance.Flat) * (100. - Resistance.Percent) / 100.;
   float MinDamage = 0;           // Can't heal
   float MaxDamage = Damage * 2;  // Limit

   return FMath::Clamp (NewDamage, MinDamage , MaxDamage);

   // return (100 - URUtilLibrary::GetRuntimeFloatCurveValue (ResistanceToDamage, Resistance)) * Damage / 100;
}

