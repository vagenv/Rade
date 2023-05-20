// Copyright 2015-2023 Vagen Ayrapetyan

#include "RLoadingScreenLibrary.h"
#include "MoviePlayer.h"
#include "RLoadingScreenLib.h"

int32 URLoadingScreenLibrary::DisplayBackgroundIndex = -1;
int32 URLoadingScreenLibrary::DisplayTipTextIndex = -1;
int32 URLoadingScreenLibrary::DisplayMovieIndex = -1;
bool  URLoadingScreenLibrary::bShowLoadingScreen = true;

void URLoadingScreenLibrary::SetDisplayBackgroundIndex(int32 BackgroundIndex)
{
	URLoadingScreenLibrary::DisplayBackgroundIndex = BackgroundIndex;
}

void URLoadingScreenLibrary::SetDisplayTipTextIndex(int32 TipTextIndex)
{
	URLoadingScreenLibrary::DisplayTipTextIndex = TipTextIndex;
}

void URLoadingScreenLibrary::SetDisplayMovieIndex(int32 MovieIndex)
{
	URLoadingScreenLibrary::DisplayMovieIndex = MovieIndex;
}

void URLoadingScreenLibrary::SetEnableLoadingScreen(bool bIsEnableLoadingScreen)
{
	bShowLoadingScreen = bIsEnableLoadingScreen;
}

void URLoadingScreenLibrary::StopLoadingScreen()
{
	GetMoviePlayer()->StopMovie();
}

void URLoadingScreenLibrary::PreloadBackgroundImages()
{
	if (FRLoadingScreenModule::IsAvailable())
	{
		FRLoadingScreenModule& LoadingScreenModule = FRLoadingScreenModule::Get();
		LoadingScreenModule.LoadBackgroundImages();
		if (LoadingScreenModule.IsPreloadBackgroundImagesEnabled())
		{
			LoadingScreenModule.LoadBackgroundImages();
		}
	}
}

void URLoadingScreenLibrary::RemovePreloadedBackgroundImages()
{
	if (FRLoadingScreenModule::IsAvailable())
	{
		FRLoadingScreenModule& LoadingScreenModule = FRLoadingScreenModule::Get();
		LoadingScreenModule.RemoveAllBackgroundImages();
	}
}

