// Copyright 2015-2023 Vagen Ayrapetyan

#include "RStatusEffect.h"
#include "RUtilLib/RLog.h"

FRStatusEffect FRStatusEffect::operator + (const FRStatusEffect &obj) const
{
   FRStatusEffect res;
   res.Scale = Scale;
   res.Target = Target;
   res.Value = Value + obj.Value;
   return res;
}

