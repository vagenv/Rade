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

bool FRCoreStats::IsEmpty () const
{
   return (!STR && !AGI && !INT);
}

bool FRCoreStats::MoreThan (const FRCoreStats &stat) const {
   return (
         STR >= stat.STR
      && AGI >= stat.AGI
      && INT >= stat.INT
   );
}

bool FRCoreStats::operator == (const FRCoreStats &stat) const
{
   return FMath::TruncToInt64 (10. * this->STR) == FMath::TruncToInt64 (10. * stat.STR)
       && FMath::TruncToInt64 (10. * this->AGI) == FMath::TruncToInt64 (10. * stat.AGI)
       && FMath::TruncToInt64 (10. * this->INT) == FMath::TruncToInt64 (10. * stat.INT);
}

bool FRCoreStats::operator != (const FRCoreStats &stat) const
{
   return !(*this == stat);
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


bool FRSubStats::IsEmpty () const
{
   return (
      !Evasion     &&
      !Critical    &&
      !MoveSpeed   &&
      !AttackSpeed &&
      !AttackPower
   );
}

bool FRSubStats::operator == (const FRSubStats &stat) const
{
   return FMath::TruncToInt64 (10. * this->Evasion)     == FMath::TruncToInt64 (10. * stat.Evasion)
       && FMath::TruncToInt64 (10. * this->Critical)    == FMath::TruncToInt64 (10. * stat.Critical)
       && FMath::TruncToInt64 (10. * this->MoveSpeed)   == FMath::TruncToInt64 (10. * stat.MoveSpeed)
       && FMath::TruncToInt64 (10. * this->AttackSpeed) == FMath::TruncToInt64 (10. * stat.AttackSpeed)
       && FMath::TruncToInt64 (10. * this->AttackPower) == FMath::TruncToInt64 (10. * stat.AttackPower);
}

bool FRSubStats::operator != (const FRSubStats &stat) const
{
   return !(*this == stat);
}

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

//=============================================================================
//                 Status Library
//=============================================================================

bool URStatusUtilLibrary::FRCoreStats_IsEmpty (const FRCoreStats &Value)
{
   return Value.IsEmpty ();
}

bool URStatusUtilLibrary::FRCoreStats_EqualEqual (const FRCoreStats& A,
                                                  const FRCoreStats& B)
{
   return A == B;
}

bool URStatusUtilLibrary::FRCoreStats_NotEqual (const FRCoreStats& A,
                                                const FRCoreStats& B)
{
   return A != B;
}

bool URStatusUtilLibrary::FRSubStats_IsEmpty (const FRSubStats &Value)
{
   return Value.IsEmpty ();
}

bool URStatusUtilLibrary::FRSubStats_EqualEqual (const FRSubStats& A,
                                                 const FRSubStats& B)
{
   return A == B;
}

bool URStatusUtilLibrary::FRSubStats_NotEqual (const FRSubStats& A,
                                               const FRSubStats& B)
{
   return A != B;
}

FString URStatusUtilLibrary::FRStatusValue_ToString (const FRStatusValue &Value)
{
   return FString::Printf (
         TEXT("%.1f / %.1f%s"),
         Value.Current,
         Value.Max,
         FMath::Abs (Value.Regen) > 0.01 ? (*FString::Printf (TEXT (" (%+.1f)"), Value.Regen)) : TEXT ("")
      );
}

