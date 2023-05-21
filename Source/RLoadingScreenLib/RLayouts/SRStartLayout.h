// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "SRLayout.h"

struct FALoadingScreenSettings;
struct FStartLayoutSettings;

// Start loading screen
class SRStartLayout : public SRLayout
{
public:
	SLATE_BEGIN_ARGS(SRStartLayout) {}

	SLATE_END_ARGS()


	void Construct (const FArguments					 &InArgs,
						 const FALoadingScreenSettings &ScreenSettings,
						 const FStartLayoutSettings    &LayoutSettings);
};
