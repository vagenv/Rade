// Copyright 2015-2023 Vagen Ayrapetyan


#include "SVerticalLoadingWidget.h"
#include "../RLoadingScreenSettings.h"
#include "Widgets/Layout/SSpacer.h"
#include "Widgets/Images/SImage.h"
#include "Slate/DeferredCleanupSlateBrush.h"
#include "Widgets/Text/STextBlock.h"

void SVerticalLoadingWidget::Construct(const FArguments& InArgs, const FLoadingWidgetSettings& Settings)
{
	bPlayReverse = Settings.ImageSequenceSettings.bPlayReverse;

	// Root is a Vertical Box
	TSharedRef<SVerticalBox> Root = SNew(SVerticalBox);

	// Construct Loading Icon Widget
	ConstructLoadingIcon(Settings);

	EVisibility LoadingTextVisibility;

	if (Settings.LoadingText.IsEmpty())
	{
		LoadingTextVisibility = EVisibility::Collapsed;
	}
	else
	{
		LoadingTextVisibility = EVisibility::SelfHitTestInvisible;
	}

	// If loading text is on the top
	if (Settings.bLoadingTextTopPosition)
	{
		// Add Loading Text on the top first
		Root.Get().AddSlot()
			.HAlign(Settings.TextAlignment.HorizontalAlignment)
			.VAlign(Settings.TextAlignment.VerticalAlignment)
			.AutoHeight()
			[
				SNew(STextBlock)
				.Visibility(LoadingTextVisibility)
				.ColorAndOpacity(Settings.Appearance.ColorAndOpacity)
				.Font(Settings.Appearance.Font)
				.ShadowOffset(Settings.Appearance.ShadowOffset)
				.ShadowColorAndOpacity(Settings.Appearance.ShadowColorAndOpacity)
				.Justification(Settings.Appearance.Justification)
				.Text(Settings.LoadingText)
			];

		// Add a Spacer in middle
		Root.Get().AddSlot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			.AutoHeight()
			[
				SNew(SSpacer)
				.Size(FVector2D(0.0f, Settings.Space))
			];

		// Add Loading Icon at the bottom finally
		Root.Get().AddSlot()
			.HAlign(Settings.LoadingIconAlignment.HorizontalAlignment)
			.VAlign(Settings.LoadingIconAlignment.VerticalAlignment)
			.AutoHeight()
			[
				LoadingIcon
			];
	}

	// If loading text is at the bottom
	else
	{
		// Add Loading Icon on the top
		Root.Get().AddSlot()
			.HAlign(Settings.LoadingIconAlignment.HorizontalAlignment)
			.VAlign(Settings.LoadingIconAlignment.VerticalAlignment)
			.AutoHeight()
			[
				LoadingIcon
			];

		// Add a Spacer in middle
		Root.Get().AddSlot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			.AutoHeight()
			[
				SNew(SSpacer)
				.Size(FVector2D(0.0f, Settings.Space))
			];

		// Add Loading Text at the bottom
		Root.Get().AddSlot()
			.HAlign(Settings.TextAlignment.HorizontalAlignment)
			.VAlign(Settings.TextAlignment.VerticalAlignment)
			.AutoHeight()
			[
				SNew(STextBlock)
				.Visibility(LoadingTextVisibility)
				.ColorAndOpacity(Settings.Appearance.ColorAndOpacity)
				.Font(Settings.Appearance.Font)
				.ShadowOffset(Settings.Appearance.ShadowOffset)
				.ShadowColorAndOpacity(Settings.Appearance.ShadowColorAndOpacity)
				.Justification(Settings.Appearance.Justification)
				.Text(Settings.LoadingText)
			];
	}

	// Add root to this widget
	ChildSlot
	[
		Root
	];
}
