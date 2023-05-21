// Copyright 2015-2023 Vagen Ayrapetyan

#include "RLoadingScreenLib.h"

#include "MoviePlayer.h"
#include "Framework/Application/SlateApplication.h"
#include "Engine/Texture2D.h"

#include "RLoadingScreenSettings.h"
#include "RLoadingScreenLibrary.h"

#include "RLayouts/SRLoadingLayout.h"
#include "RLayouts/SRStartLayout.h"

#include "RUtilLib/RLog.h"


#define LOCTEXT_NAMESPACE "FRLoadingScreenModule"

void FRLoadingScreenModule::StartupModule ()
{
	// This code will execute after your module is loaded into memory.
	// The exact timing is specified in the .uplugin file per-module
	if (!IsRunningDedicatedServer () && FSlateApplication::IsInitialized ()) {
		const ULoadingScreenSettings* Settings = GetDefault<ULoadingScreenSettings> ();

		if (IsMoviePlayerEnabled ()) {
			GetMoviePlayer ()->OnPrepareLoadingScreen ().AddRaw (this, &FRLoadingScreenModule::PreSetupLoadingScreen);
		}

		// If PreloadBackgroundImages option is check, load all background images into memory
		if (Settings->bPreloadBackgroundImages) {
			LoadBackgroundImages ();
		}

		// Prepare the startup screen, the PreSetupLoadingScreen callback won't be called
		// if we've already explicitly setup the loading screen
		bIsStartupLoadingScreen = true;
		SetupLoadingScreen (Settings->StartScreen);
	}
}

void FRLoadingScreenModule::ShutdownModule ()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	if (!IsRunningDedicatedServer ()) {
		// TODO: Unregister later
		GetMoviePlayer ()->OnPrepareLoadingScreen().RemoveAll (this);
	}
}

bool FRLoadingScreenModule::IsGameModule () const
{
	return true;
}

TArray<UTexture2D*> FRLoadingScreenModule::GetBackgroundImages ()
{
	return bIsStartupLoadingScreen ? StartupBackgroundImages : DefaultBackgroundImages;
}

void FRLoadingScreenModule::PreSetupLoadingScreen ()
{
	//R_LOG_STATIC ("Loading screen completed");

	const bool bIsEnableLoadingScreen = URLoadingScreenLibrary::GetIsEnableLoadingScreen ();
	if (bIsEnableLoadingScreen) {
		const ULoadingScreenSettings* Settings = GetDefault<ULoadingScreenSettings> ();
		bIsStartupLoadingScreen = false;
		SetupLoadingScreen (Settings->LoadingScreen);
	}
}

void FRLoadingScreenModule::SetupLoadingScreen (const FALoadingScreenSettings& ScreenSettings)
{
	TArray<FString> MoviesList = ScreenSettings.MoviePaths;

	// Shuffle the movies list
	if (ScreenSettings.bShuffle == true) {
		ShuffleMovies (MoviesList);
	}

	if (ScreenSettings.bSetDisplayMovieIndexManually == true) {
		MoviesList.Empty ();

		// Show specific movie if valid otherwise show original movies list
		if (ScreenSettings.MoviePaths.IsValidIndex (URLoadingScreenLibrary::GetDisplayMovieIndex())) {
			MoviesList.Add (ScreenSettings.MoviePaths[URLoadingScreenLibrary::GetDisplayMovieIndex()]);
		} else {
			MoviesList = ScreenSettings.MoviePaths;
		}
	}

	FLoadingScreenAttributes ScreenAttributes;
	ScreenAttributes.MinimumLoadingScreenDisplayTime	= ScreenSettings.MinimumLoadingScreenDisplayTime;
	ScreenAttributes.bAutoCompleteWhenLoadingCompletes = ScreenSettings.bAutoCompleteWhenLoadingCompletes;
	ScreenAttributes.bMoviesAreSkippable					= ScreenSettings.bMoviesAreSkippable;
	ScreenAttributes.bWaitForManualStop						= ScreenSettings.bWaitForManualStop;
	ScreenAttributes.bAllowInEarlyStartup					= ScreenSettings.bAllowInEarlyStartup;
	ScreenAttributes.bAllowEngineTick						= ScreenSettings.bAllowEngineTick;
	ScreenAttributes.MoviePaths								= MoviesList;
	ScreenAttributes.PlaybackType							   = ScreenSettings.PlaybackType;

	if (ScreenSettings.bShowWidgetOverlay) {
		const ULoadingScreenSettings* Settings = GetDefault<ULoadingScreenSettings> ();

		if (bIsStartupLoadingScreen) {
			ScreenAttributes.WidgetLoadingScreen = SNew (SRStartLayout, ScreenSettings, Settings->StartSettings);
		} else {
			ScreenAttributes.WidgetLoadingScreen = SNew (SRLoadingLayout, ScreenSettings, Settings->LoadingSettings);
		}
	}

	GetMoviePlayer()->SetupLoadingScreen (ScreenAttributes);
}

void FRLoadingScreenModule::ShuffleMovies(TArray<FString>& MoviesList)
{
	if (MoviesList.Num () > 0) {
		int32 LastIndex = MoviesList.Num () - 1;
		for (int32 i = 0; i <= LastIndex; ++i) {
			int32 Index = FMath::RandRange (i, LastIndex);
			if (i != Index) {
				MoviesList.Swap (i, Index);
			}
		}
	}
}

void FRLoadingScreenModule::LoadBackgroundImages ()
{
	// Empty all background images array
	RemoveAllBackgroundImages ();

	const ULoadingScreenSettings* Settings = GetDefault<ULoadingScreenSettings> ();

	// Preload startup background images
	for (const auto &ItImage : Settings->StartScreen.Background.Images) {
		UTexture2D* LoadedImage = Cast<UTexture2D>(ItImage.TryLoad());
		if (LoadedImage) {
			StartupBackgroundImages.Add (LoadedImage);
		}
	}

	// Preload default background images
	for (const auto& ItImage : Settings->LoadingScreen.Background.Images) {
		UTexture2D* LoadedImage = Cast<UTexture2D> (ItImage.TryLoad ());
		if (LoadedImage) {
			DefaultBackgroundImages.Add (LoadedImage);
		}
	}
}

void FRLoadingScreenModule::RemoveAllBackgroundImages()
{
	StartupBackgroundImages.Empty ();
	DefaultBackgroundImages.Empty ();
}

bool FRLoadingScreenModule::IsPreloadBackgroundImagesEnabled ()
{
	return GetDefault<ULoadingScreenSettings> ()->bPreloadBackgroundImages;
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_GAME_MODULE ( FRLoadingScreenModule, RLoadingScreenLib)

