// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "SLoadingScreenLayout.h"

struct FALoadingScreenSettings;
struct FLetterboxLayoutSettings;

/**
 * Letterbox layout loading screen
 */
class SLetterboxLayout : public SLoadingScreenLayout
{
public:
	SLATE_BEGIN_ARGS(SLetterboxLayout) {}

	SLATE_END_ARGS()

	/**
	 * Construct this widget
	 */
	void Construct(const FArguments& InArgs, const FALoadingScreenSettings& Settings, const FLetterboxLayoutSettings& LayoutSettings);
};
