// Copyright 2015-2023 Vagen Ayrapetyan

#include "RStatusTypes.h"
#include "RUtilLib/Rlog.h"

FArchive& operator << (FArchive& Ar, FRStatusValue &Value) {
   Ar << Value.Current;
   Ar << Value.Max;
   Ar << Value.Regen;
   return Ar;
}

