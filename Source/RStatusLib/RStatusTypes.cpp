// Copyright 2015-2023 Vagen Ayrapetyan

#include "RStatusTypes.h"
#include "RUtilLib/RLog.h"


//=============================================================================
//                 Character Stats
//=============================================================================

FRCharacterStats::FRCharacterStats ()
{
}
FRCharacterStats::FRCharacterStats (float Value)
{
   Strength     = Value;
   Agility      = Value;
   Intelligence = Value;
}

bool FRCharacterStats::Empty () const
{
   return (
         Strength     == 0.
      && Agility      == 0.
      && Intelligence == 0.
   );
}

bool FRCharacterStats::MoreThan (const FRCharacterStats &obj) const {
   return (
         Strength     >= obj.Strength
      && Agility      >= obj.Agility
      && Intelligence >= obj.Intelligence
   );
}

FRCharacterStats FRCharacterStats::operator + (const FRCharacterStats &obj) const
{
   FRCharacterStats res;
   res.Strength     = Strength     + obj.Strength    ;
   res.Agility      = Agility      + obj.Agility     ;
   res.Intelligence = Intelligence + obj.Intelligence;
   return res;
}

FRCharacterStats FRCharacterStats::operator - (const FRCharacterStats &obj) const
{
   FRCharacterStats res;
   res.Strength     = Strength     - obj.Strength    ;
   res.Agility      = Agility      - obj.Agility     ;
   res.Intelligence = Intelligence - obj.Intelligence;
   return res;
}

//=============================================================================
//                 Status Value
//=============================================================================

void FRStatusValue::Tick (float DeltaTime)
{
   if (Current < Max && Regen)
      Current = Current + Regen * DeltaTime;
   Current = FMath::Clamp (Current, 0, Max);
}

FArchive& operator << (FArchive& Ar, FRStatusValue &Value) {
   Ar << Value.Current;
   Ar << Value.Max;
   Ar << Value.Regen;
   return Ar;
}

