// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "Widgets/SCompoundWidget.h"

/**
 * Loading screen base theme
 */
class SLoadingScreenLayout : public SCompoundWidget
{
public:
	static float PointSizeToSlateUnits(float PointSize);
protected:
	float GetDPIScale() const;
};
