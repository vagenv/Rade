// Copyright 2015-2023 Vagen Ayrapetyan

#include "RStatusTypes.h"
#include "RUtilLib/RLog.h"

//=============================================================================
//                 Core Stats
//=============================================================================

FRCoreStats::FRCoreStats ()
{
}
FRCoreStats::FRCoreStats (float Value)
{
   STR = Value;
   AGI = Value;
   INT = Value;
}

bool FRCoreStats::Empty () const
{
   return (
         STR == 0.
      && AGI == 0.
      && INT == 0.
   );
}

bool FRCoreStats::MoreThan (const FRCoreStats &stat) const {
   return (
         STR >= stat.STR
      && AGI >= stat.AGI
      && INT >= stat.INT
   );
}

FRCoreStats FRCoreStats::operator + (const FRCoreStats &stat) const
{
   FRCoreStats res;
   res.STR = STR + stat.STR;
   res.AGI = AGI + stat.AGI;
   res.INT = INT + stat.INT;
   return res;
}

FRCoreStats FRCoreStats::operator - (const FRCoreStats &stat) const
{
   FRCoreStats res;
   res.STR = STR - stat.STR;
   res.AGI = AGI - stat.AGI;
   res.INT = INT - stat.INT;
   return res;
}

FRCoreStats& FRCoreStats::operator += (const FRCoreStats &stat)
{
   FRCoreStats &Current = *this;
   Current = Current + stat;
   return Current;
}

//=============================================================================
//                 Extra Stats
//=============================================================================

FRSubStats FRSubStats::operator + (const FRSubStats &stat) const
{
   FRSubStats res;
   res.Evasion     = Evasion     + stat.Evasion    ;
   res.Critical    = Critical    + stat.Critical   ;
   res.MoveSpeed   = MoveSpeed   + stat.MoveSpeed  ;
   res.AttackSpeed = AttackSpeed + stat.AttackSpeed;
   res.AttackPower = AttackPower + stat.AttackPower;
   return res;
}

FRSubStats FRSubStats::operator - (const FRSubStats &stat) const
{
   FRSubStats res;
   res.Evasion     = Evasion     - stat.Evasion    ;
   res.Critical    = Critical    - stat.Critical   ;
   res.MoveSpeed   = MoveSpeed   - stat.MoveSpeed  ;
   res.AttackSpeed = AttackSpeed - stat.AttackSpeed;
   res.AttackPower = AttackPower - stat.AttackPower;
   return res;
}

FRSubStats& FRSubStats::operator += (const FRSubStats &stat)
{
   FRSubStats &Current = *this;
   Current = Current + stat;
   return Current;
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

