// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "Widgets/SCompoundWidget.h"

// Base layout for loading screens
class SRLayout : public SCompoundWidget
{
public:
	static float PointSizeToSlateUnits (float PointSize);
protected:
	float GetDPIScale () const;
};

