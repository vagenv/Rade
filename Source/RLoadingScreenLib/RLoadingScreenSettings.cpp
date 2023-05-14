// Copyright 2015-2023 Vagen Ayrapetyan


#include "RLoadingScreenSettings.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/Font.h"

#define LOCTEXT_NAMESPACE "RLoadingScreen"

FLoadingWidgetSettings::FLoadingWidgetSettings() : LoadingText(LOCTEXT("Loading", "LOADING")) {}
//FLoadingCompleteTextSettings::FLoadingCompleteTextSettings() : LoadingCompleteText(LOCTEXT("Loading Complete", "Loading is complete! Press any key to continue...")) {}


ULoadingScreenSettings::ULoadingScreenSettings(const FObjectInitializer& Initializer) : Super(Initializer)
{
	StartupLoadingScreen.TipWidget.TipWrapAt = 1000.0f;
	StartupLoadingScreen.bShowWidgetOverlay = false;
	DefaultLoadingScreen.TipWidget.TipWrapAt = 1000.0f;
	// Set default font
	if (!IsRunningDedicatedServer())
	{
		static ConstructorHelpers::FObjectFinder<UFont> RobotoFontObj(TEXT("/Engine/EngineFonts/Roboto"));
		StartupLoadingScreen.TipWidget.Appearance.Font = FSlateFontInfo(RobotoFontObj.Object, 20, FName("Normal"));
		DefaultLoadingScreen.TipWidget.Appearance.Font = FSlateFontInfo(RobotoFontObj.Object, 20, FName("Normal"));
		StartupLoadingScreen.LoadingWidget.Appearance.Font = FSlateFontInfo(RobotoFontObj.Object, 32, FName("Bold"));
		DefaultLoadingScreen.LoadingWidget.Appearance.Font = FSlateFontInfo(RobotoFontObj.Object, 32, FName("Bold"));
		StartupLoadingScreen.LoadingCompleteTextSettings.Appearance.Font = FSlateFontInfo(RobotoFontObj.Object, 24, FName("Normal"));
		DefaultLoadingScreen.LoadingCompleteTextSettings.Appearance.Font = FSlateFontInfo(RobotoFontObj.Object, 24, FName("Normal"));
	}
}

#undef LOCTEXT_NAMESPACE


