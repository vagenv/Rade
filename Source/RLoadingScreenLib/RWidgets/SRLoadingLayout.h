// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "SLoadingScreenLayout.h"

struct FALoadingScreenSettings;
struct FLoadingLayoutSettings;

/**
 * Classic layout loading screen
 */
class SRLoadingLayout : public SLoadingScreenLayout
{
public:
	SLATE_BEGIN_ARGS(SRLoadingLayout) {}

	SLATE_END_ARGS()

	/**
	 * Construct this widget
	 */
	void Construct(const FArguments& InArgs, const FALoadingScreenSettings& Settings, const FLoadingLayoutSettings& LayoutSettings);
};
