// Copyright 2015-2023 Vagen Ayrapetyan

#include "RAbilityTypes.h"

// ============================================================================
//                   AbilityInfo
// ============================================================================

bool FRAbilityInfo::operator == (const FRAbilityInfo &AbilityInfo) const noexcept
{
   return (  AbilityClass == AbilityInfo.AbilityClass
          && Description  == AbilityInfo.Description);
}

bool FRAbilityInfo::operator != (const FRAbilityInfo &res) const noexcept
{
   return !(*this == res);
}

bool URAbilityInfolLibrary::AbilityInfo_EqualEqual (const FRAbilityInfo& A, const FRAbilityInfo& B)
{
   return A == B;
}

bool URAbilityInfolLibrary::AbilityInfo_NotEqual (const FRAbilityInfo& A, const FRAbilityInfo& B)
{
   return A != B;
}

bool URAbilityInfolLibrary::AbilityInfo_IsEmpty (const FRAbilityInfo& AbilityInfo)
{
   return AbilityInfo.IsEmpty ();
}

