// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "SLoadingScreenLayout.h"

struct FALoadingScreenSettings;
struct FStartLayoutSettings;

// Start loading screen
class SRStartLayout : public SLoadingScreenLayout
{
public:
	SLATE_BEGIN_ARGS(SRStartLayout) {}

	SLATE_END_ARGS()


	void Construct(const FArguments& InArgs, const FALoadingScreenSettings& Settings, const FStartLayoutSettings& LayoutSettings);
};
