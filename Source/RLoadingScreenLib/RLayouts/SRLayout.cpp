// Copyright 2015-2023 Vagen Ayrapetyan


#include "SRLayout.h"
#include "Engine/UserInterfaceSettings.h"

float SRLayout::PointSizeToSlateUnits (float PointSize)
{
	const float SlateFreeTypeHorizontalResolutionDPI = 96.0f;
	const float FreeTypeNativeDPI = 72.0;
	const float PixelSize = PointSize * (SlateFreeTypeHorizontalResolutionDPI / FreeTypeNativeDPI);
	return PixelSize;
}

float SRLayout::GetDPIScale () const
{
	const FVector2D DrawSize = GetTickSpaceGeometry ().ToPaintGeometry ().GetLocalSize ();
	const FIntPoint Size ((int32)DrawSize.X, (int32)DrawSize.Y);

	return GetDefault<UUserInterfaceSettings> ()->GetDPIScaleBasedOnSize (Size);
}

