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
}

float URDamageType::CalcDamage (float Damage, const FRDamageResistance &Resistance) const
{
   float NewDamage = (Damage - Resistance.Flat) * (100. - Resistance.Percent) / 100.;
   float MinDamage = 0;           // Can't heal
   float MaxDamage = Damage * 2;  // Limit

   return FMath::Clamp (NewDamage, MinDamage , MaxDamage);
}

//=============================================================================
//                 Util Library
//=============================================================================


TArray<FRDamageResistance> URDamageUtilLibrary::MergeResistance (
   const TArray<FRDamageResistanceWithTag>& Resistence)
{
   TArray<FRDamageResistance> Result;
   for (const FRDamageResistanceWithTag& ItResistance : Resistence) {
      bool found = false;
      // Combine
      for (FRDamageResistance& ItRes : Result) {
         if (ItRes.DamageType == ItResistance.Value.DamageType) {
            found = true;
            ItRes.Flat    += ItResistance.Value.Flat;
            ItRes.Percent += ItResistance.Value.Percent;
            break;
         }
      }
      // Add new entry
      if (!found) Result.Add (ItResistance.Value);
   }
   return Result;
}