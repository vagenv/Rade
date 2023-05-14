// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "SLoadingScreenLayout.h"

struct FALoadingScreenSettings;
struct FCenterLayoutSettings;

/**
 * Center layout loading screen
 */
class SCenterLayout : public SLoadingScreenLayout
{
public:
	SLATE_BEGIN_ARGS(SCenterLayout) {}

	SLATE_END_ARGS()

	/**
	 * Construct this widget
	 */
	void Construct(const FArguments& InArgs, const FALoadingScreenSettings& Settings, const FCenterLayoutSettings& LayoutSettings);
};
