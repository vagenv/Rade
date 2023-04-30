// Copyright 2015-2023 Vagen Ayrapetyan

#include "RUIDescription.h"

bool FRUIDescription::operator == (const FRUIDescription &res) const noexcept
{
   return (  Label   == res.Label
          && Tooltip == res.Tooltip
          && Icon    == res.Icon);
}

bool URUIDescriptionUtilLibrary::FRUIDescription_EqualEqual (const FRUIDescription& A, const FRUIDescription& B)
{
   return A == B;
}