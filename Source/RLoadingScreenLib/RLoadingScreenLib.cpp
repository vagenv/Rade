// Copyright 2015-2023 Vagen Ayrapetyan


#include "RLoadingScreenLib.h"
#include "MoviePlayer.h"
#include "RLoadingScreenSettings.h"
#include "RWidgets/SCenterLayout.h"
#include "RWidgets/SClassicLayout.h"
#include "RWidgets/SLetterboxLayout.h"
#include "RWidgets/SSidebarLayout.h"
#include "RWidgets/SDualSidebarLayout.h"
#include "Framework/Application/SlateApplication.h"
#include "RLoadingScreenLibrary.h"
#include "Engine/Texture2D.h"

#define LOCTEXT_NAMESPACE "FRLoadingScreenModule"

void FRLoadingScreenModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	if (!IsRunningDedicatedServer() && FSlateApplication::IsInitialized())
	{
		const ULoadingScreenSettings* Settings = GetDefault<ULoadingScreenSettings>();

		if (IsMoviePlayerEnabled())
		{
			GetMoviePlayer()->OnPrepareLoadingScreen().AddRaw(this, &FRLoadingScreenModule::PreSetupLoadingScreen);
		}

		// If PreloadBackgroundImages option is check, load all background images into memory
		if (Settings->bPreloadBackgroundImages)
		{
			LoadBackgroundImages();
		}

		// Prepare the startup screen, the PreSetupLoadingScreen callback won't be called
		// if we've already explicitly setup the loading screen
		bIsStartupLoadingScreen = true;
		SetupLoadingScreen(Settings->StartupLoadingScreen);
	}
}

void FRLoadingScreenModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	if (!IsRunningDedicatedServer())
	{
		// TODO: Unregister later
		GetMoviePlayer()->OnPrepareLoadingScreen().RemoveAll(this);
	}
}

bool FRLoadingScreenModule::IsGameModule() const
{
	return true;
}

TArray<UTexture2D*> FRLoadingScreenModule::GetBackgroundImages()
{
	return bIsStartupLoadingScreen ? StartupBackgroundImages : DefaultBackgroundImages;
}

void FRLoadingScreenModule::PreSetupLoadingScreen()
{
	UE_LOG(LogTemp, Warning, TEXT("PreSetupLoadingScreen"));
	const bool bIsEnableLoadingScreen = URLoadingScreenLibrary::GetIsEnableLoadingScreen();
	if (bIsEnableLoadingScreen)
	{
		const ULoadingScreenSettings* Settings = GetDefault<ULoadingScreenSettings>();
		bIsStartupLoadingScreen = false;
		SetupLoadingScreen(Settings->DefaultLoadingScreen);
	}
}

void FRLoadingScreenModule::SetupLoadingScreen(const FALoadingScreenSettings& LoadingScreenSettings)
{
	TArray<FString> MoviesList = LoadingScreenSettings.MoviePaths;

	// Shuffle the movies list
	if (LoadingScreenSettings.bShuffle == true)
	{
		ShuffleMovies(MoviesList);
	}

	if (LoadingScreenSettings.bSetDisplayMovieIndexManually == true)
	{
		MoviesList.Empty();

		// Show specific movie if valid otherwise show original movies list
		if (LoadingScreenSettings.MoviePaths.IsValidIndex(URLoadingScreenLibrary::GetDisplayMovieIndex()))
		{
			MoviesList.Add(LoadingScreenSettings.MoviePaths[URLoadingScreenLibrary::GetDisplayMovieIndex()]);
		}
		else
		{
			MoviesList = LoadingScreenSettings.MoviePaths;
		}
	}

	FLoadingScreenAttributes LoadingScreen;
	LoadingScreen.MinimumLoadingScreenDisplayTime = LoadingScreenSettings.MinimumLoadingScreenDisplayTime;
	LoadingScreen.bAutoCompleteWhenLoadingCompletes = LoadingScreenSettings.bAutoCompleteWhenLoadingCompletes;
	LoadingScreen.bMoviesAreSkippable = LoadingScreenSettings.bMoviesAreSkippable;
	LoadingScreen.bWaitForManualStop = LoadingScreenSettings.bWaitForManualStop;
	LoadingScreen.bAllowInEarlyStartup = LoadingScreenSettings.bAllowInEarlyStartup;
	LoadingScreen.bAllowEngineTick = LoadingScreenSettings.bAllowEngineTick;
	LoadingScreen.MoviePaths = MoviesList;
	LoadingScreen.PlaybackType = LoadingScreenSettings.PlaybackType;

	if (LoadingScreenSettings.bShowWidgetOverlay)
	{
		const ULoadingScreenSettings* Settings = GetDefault<ULoadingScreenSettings>();

		switch (LoadingScreenSettings.Layout)
		{
		case ERLoadingScreenLayout::ALSL_Classic:
			LoadingScreen.WidgetLoadingScreen = SNew(SClassicLayout, LoadingScreenSettings, Settings->Classic);
			break;
		case ERLoadingScreenLayout::ALSL_Center:
			LoadingScreen.WidgetLoadingScreen = SNew(SCenterLayout, LoadingScreenSettings, Settings->Center);
			break;
		case ERLoadingScreenLayout::ALSL_Letterbox:
			LoadingScreen.WidgetLoadingScreen = SNew(SLetterboxLayout, LoadingScreenSettings, Settings->Letterbox);
			break;
		case ERLoadingScreenLayout::ALSL_Sidebar:
			LoadingScreen.WidgetLoadingScreen = SNew(SSidebarLayout, LoadingScreenSettings, Settings->Sidebar);
			break;
		case ERLoadingScreenLayout::ALSL_DualSidebar:
			LoadingScreen.WidgetLoadingScreen = SNew(SDualSidebarLayout, LoadingScreenSettings, Settings->DualSidebar);
			break;
		}

	}

	GetMoviePlayer()->SetupLoadingScreen(LoadingScreen);
}

void FRLoadingScreenModule::ShuffleMovies(TArray<FString>& MoviesList)
{
	if (MoviesList.Num() > 0)
	{
		int32 LastIndex = MoviesList.Num() - 1;
		for (int32 i = 0; i <= LastIndex; ++i)
		{
			int32 Index = FMath::RandRange(i, LastIndex);
			if (i != Index)
			{
				MoviesList.Swap(i, Index);
			}
		}
	}
}

void FRLoadingScreenModule::LoadBackgroundImages()
{
	// Empty all background images array
	RemoveAllBackgroundImages();

	const ULoadingScreenSettings* Settings = GetDefault<ULoadingScreenSettings>();

	// Preload startup background images
	for (auto& Image : Settings->StartupLoadingScreen.Background.Images)
	{
		UTexture2D* LoadedImage = Cast<UTexture2D>(Image.TryLoad());
		if (LoadedImage)
		{
			StartupBackgroundImages.Add(LoadedImage);
		}
	}

	// Preload default background images
	for (auto& Image : Settings->DefaultLoadingScreen.Background.Images)
	{
		UTexture2D* LoadedImage = Cast<UTexture2D> (Image.TryLoad());
		if (LoadedImage)
		{
			DefaultBackgroundImages.Add(LoadedImage);
		}
	}
}

void FRLoadingScreenModule::RemoveAllBackgroundImages()
{
	StartupBackgroundImages.Empty();
	DefaultBackgroundImages.Empty();
}

bool FRLoadingScreenModule::IsPreloadBackgroundImagesEnabled()
{
	return GetDefault<ULoadingScreenSettings>()->bPreloadBackgroundImages;
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_GAME_MODULE ( FRLoadingScreenModule, RLoadingScreenLib)

