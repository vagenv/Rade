// Copyright 2015-2023 Vagen Ayrapetyan


#include "RLoadingScreenSettings.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/Font.h"

#define LOCTEXT_NAMESPACE "RLoadingScreen"

FLoadingWidgetSettings::FLoadingWidgetSettings ()
	: LoadingText (LOCTEXT("Loading", "LOADING"))
{
}
//FLoadingCompleteTextSettings::FLoadingCompleteTextSettings() : LoadingCompleteText(LOCTEXT("Loading Complete", "Loading is complete! Press any key to continue...")) {}


ULoadingScreenSettings::ULoadingScreenSettings(const FObjectInitializer& Initializer) : Super(Initializer)
{
	StartScreen.TipWidget.TipWrapAt   = 1000.0f;
	StartScreen.bShowWidgetOverlay    = false;
	LoadingScreen.TipWidget.TipWrapAt = 1000.0f;
	// Set default font
	if (!IsRunningDedicatedServer ()) {
		static ConstructorHelpers::FObjectFinder<UFont> RobotoFontObj(TEXT("/Engine/EngineFonts/Roboto"));
		  StartScreen.TipWidget.Appearance.Font = FSlateFontInfo(RobotoFontObj.Object, 20, FName("Normal"));
		LoadingScreen.TipWidget.Appearance.Font = FSlateFontInfo(RobotoFontObj.Object, 20, FName("Normal"));
		  StartScreen.LoadingWidget.Appearance.Font = FSlateFontInfo(RobotoFontObj.Object, 32, FName("Bold"));
		LoadingScreen.LoadingWidget.Appearance.Font = FSlateFontInfo(RobotoFontObj.Object, 32, FName("Bold"));
		  StartScreen.LoadingCompleteTextSettings.Appearance.Font = FSlateFontInfo(RobotoFontObj.Object, 24, FName("Normal"));
		LoadingScreen.LoadingCompleteTextSettings.Appearance.Font = FSlateFontInfo(RobotoFontObj.Object, 24, FName("Normal"));
	}
}

#undef LOCTEXT_NAMESPACE

