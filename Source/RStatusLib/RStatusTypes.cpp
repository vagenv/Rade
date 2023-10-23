// Copyright 2015-2023 Vagen Ayrapetyan

#include "RStatusTypes.h"
#include "RUtilLib/RLog.h"

//=============================================================================
//                 Core Stats
//=============================================================================

FRCoreStats::FRCoreStats ()
{
}

FRCoreStats::FRCoreStats (float CoreStat)
{
   STR = CoreStat;
   AGI = CoreStat;
   INT = CoreStat;
}

bool FRCoreStats::IsEmpty () const noexcept
{
   return (  !FMath::TruncToInt64 (STR)
          && !FMath::TruncToInt64 (AGI)
          && !FMath::TruncToInt64 (INT));
}

bool FRCoreStats::MoreThan (const FRCoreStats &CoreStat) const noexcept{
   return (
         STR >= CoreStat.STR
      && AGI >= CoreStat.AGI
      && INT >= CoreStat.INT
   );
}

bool FRCoreStats::operator == (const FRCoreStats &CoreStat) const noexcept
{
   return FMath::TruncToInt64 (10. * this->STR) == FMath::TruncToInt64 (10. * CoreStat.STR)
       && FMath::TruncToInt64 (10. * this->AGI) == FMath::TruncToInt64 (10. * CoreStat.AGI)
       && FMath::TruncToInt64 (10. * this->INT) == FMath::TruncToInt64 (10. * CoreStat.INT);
}

bool FRCoreStats::operator != (const FRCoreStats &CoreStat) const noexcept
{
   return !(*this == CoreStat);
}

FRCoreStats FRCoreStats::operator + (const FRCoreStats &CoreStat) const noexcept
{
   FRCoreStats res;
   res.STR = STR + CoreStat.STR;
   res.AGI = AGI + CoreStat.AGI;
   res.INT = INT + CoreStat.INT;
   return res;
}

FRCoreStats FRCoreStats::operator - (const FRCoreStats &CoreStat) const noexcept
{
   FRCoreStats res;
   res.STR = STR - CoreStat.STR;
   res.AGI = AGI - CoreStat.AGI;
   res.INT = INT - CoreStat.INT;
   return res;
}

FRCoreStats& FRCoreStats::operator += (const FRCoreStats &CoreStat) noexcept
{
   FRCoreStats &Current = *this;
   Current = Current + CoreStat;
   return Current;
}

//=============================================================================
//                 Extra Stats
//=============================================================================


bool FRSubStats::IsEmpty () const noexcept
{
   return (
         !FMath::TruncToInt64 (Evasion    )
      && !FMath::TruncToInt64 (Critical   )
      && !FMath::TruncToInt64 (MoveSpeed  )
      && !FMath::TruncToInt64 (AttackSpeed)
      && !FMath::TruncToInt64 (AttackPower)
   );
}

bool FRSubStats::operator == (const FRSubStats &SubStat) const noexcept
{
   return FMath::TruncToInt64 (10. * this->Evasion)     == FMath::TruncToInt64 (10. * SubStat.Evasion)
       && FMath::TruncToInt64 (10. * this->Critical)    == FMath::TruncToInt64 (10. * SubStat.Critical)
       && FMath::TruncToInt64 (10. * this->MoveSpeed)   == FMath::TruncToInt64 (10. * SubStat.MoveSpeed)
       && FMath::TruncToInt64 (10. * this->AttackSpeed) == FMath::TruncToInt64 (10. * SubStat.AttackSpeed)
       && FMath::TruncToInt64 (10. * this->AttackPower) == FMath::TruncToInt64 (10. * SubStat.AttackPower);
}

bool FRSubStats::operator != (const FRSubStats &SubStat) const noexcept
{
   return !(*this == SubStat);
}

FRSubStats FRSubStats::operator + (const FRSubStats &SubStat) const noexcept
{
   FRSubStats res;
   res.Evasion     = Evasion     + SubStat.Evasion    ;
   res.Critical    = Critical    + SubStat.Critical   ;
   res.MoveSpeed   = MoveSpeed   + SubStat.MoveSpeed  ;
   res.AttackSpeed = AttackSpeed + SubStat.AttackSpeed;
   res.AttackPower = AttackPower + SubStat.AttackPower;
   return res;
}

FRSubStats FRSubStats::operator - (const FRSubStats &SubStat) const noexcept
{
   FRSubStats res;
   res.Evasion     = Evasion     - SubStat.Evasion    ;
   res.Critical    = Critical    - SubStat.Critical   ;
   res.MoveSpeed   = MoveSpeed   - SubStat.MoveSpeed  ;
   res.AttackSpeed = AttackSpeed - SubStat.AttackSpeed;
   res.AttackPower = AttackPower - SubStat.AttackPower;
   return res;
}

FRSubStats& FRSubStats::operator += (const FRSubStats &SubStat) noexcept
{
   FRSubStats &Current = *this;
   Current = Current + SubStat;
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

