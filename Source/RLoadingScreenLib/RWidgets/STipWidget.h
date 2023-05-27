// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "Widgets/SCompoundWidget.h"

struct FTipSettings;

/**
 * Tip widget
 */
class STipWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(STipWidget) {}

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, const FTipSettings& Settings);
};
