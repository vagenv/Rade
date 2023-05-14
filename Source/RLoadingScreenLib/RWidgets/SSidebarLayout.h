// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "SLoadingScreenLayout.h"

struct FALoadingScreenSettings;
struct FSidebarLayoutSettings;

/**
 * Sidebar layout loading screen
 */
class SSidebarLayout : public SLoadingScreenLayout
{
public:
	SLATE_BEGIN_ARGS(SSidebarLayout) {}

	SLATE_END_ARGS()

	/**
	 * Construct this widget
	 */
	void Construct(const FArguments& InArgs, const FALoadingScreenSettings& Settings, const FSidebarLayoutSettings& LayoutSettings);
};
