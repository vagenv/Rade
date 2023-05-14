// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "SLoadingWidget.h"

struct FLoadingWidgetSettings;

/**
 *
 */
class SHorizontalLoadingWidget : public SLoadingWidget
{
public:
	SLATE_BEGIN_ARGS(SHorizontalLoadingWidget) {}

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, const FLoadingWidgetSettings& Settings);
};
