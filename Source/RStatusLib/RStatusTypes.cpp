// Copyright 2015-2023 Vagen Ayrapetyan

#include "RStatusTypes.h"
#include "RUtilLib/RLog.h"

FRCharacterStats::FRCharacterStats ()
{
}
FRCharacterStats::FRCharacterStats (float Value)
{
   Strength     = Value;
   Dexterity    = Value;
   Consitution  = Value;
   Intelligence = Value;
   Wisdom       = Value;
   Charisma     = Value;
}

FArchive& operator << (FArchive& Ar, FRStatusValue &Value) {
   Ar << Value.Current;
   Ar << Value.Max;
   Ar << Value.Regen;
   return Ar;
}

