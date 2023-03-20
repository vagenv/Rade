// Copyright 2015-2023 Vagen Ayrapetyan

#include "RPassiveStatusEffect.h"

//=============================================================================
//                 Passive Effect
//=============================================================================

FRPassiveStatusEffect FRPassiveStatusEffect::operator + (const FRPassiveStatusEffect &obj) const
{
   FRPassiveStatusEffect res;
   res.Scale = Scale;
   res.Target = Target;
   res.Value = Value + obj.Value;
   return res;
}

