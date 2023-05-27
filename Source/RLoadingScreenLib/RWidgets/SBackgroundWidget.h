// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "Widgets/SCompoundWidget.h"

struct FBackgroundSettings;
class FDeferredCleanupSlateBrush;

/**
 * Background widget
 */
class SBackgroundWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SBackgroundWidget) {}

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, const FBackgroundSettings& Settings);

private:
	TSharedPtr<FDeferredCleanupSlateBrush> ImageBrush;
};
