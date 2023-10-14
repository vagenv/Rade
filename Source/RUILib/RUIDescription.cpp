// Copyright 2015-2023 Vagen Ayrapetyan

#include "RUIDescription.h"

bool FRUIDescription::operator == (const FRUIDescription &Description) const noexcept
{
   return (  Label   == Description.Label
          && Tooltip == Description.Tooltip
          && Icon    == Description.Icon);
}

bool FRUIDescription::operator != (const FRUIDescription &Description) const noexcept
{
   return !(*this == Description);
}

bool URUIDescriptionUtilLibrary::FRUIDescription_IsEmpty (const FRUIDescription& Description)
{
   return Description.IsEmpty ();
}

bool URUIDescriptionUtilLibrary::FRUIDescription_EqualEqual (const FRUIDescription& A, const FRUIDescription& B)
{
   return A == B;
}

bool URUIDescriptionUtilLibrary::FRUIDescription_NotEqual (const FRUIDescription& A, const FRUIDescription& B)
{
   return A != B;
}

