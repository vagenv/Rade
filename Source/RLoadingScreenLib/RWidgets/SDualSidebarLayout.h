// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "SLoadingScreenLayout.h"

struct FALoadingScreenSettings;
struct FDualSidebarLayoutSettings;

/**
 * Dual Sidebar Layout
 */
class SDualSidebarLayout : public SLoadingScreenLayout
{
public:
	SLATE_BEGIN_ARGS(SDualSidebarLayout) {}

	SLATE_END_ARGS()

	/**
	 * Construct this widget
	 */
	void Construct(const FArguments& InArgs, const FALoadingScreenSettings& Settings, const FDualSidebarLayoutSettings& LayoutSettings);
};
