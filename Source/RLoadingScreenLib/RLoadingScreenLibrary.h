// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "RLoadingScreenLibrary.generated.h"

// Function Library
UCLASS()
class RLOADINGSCREENLIB_API URLoadingScreenLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
private:
	static int32 DisplayBackgroundIndex;
	static int32 DisplayTipTextIndex;
	static int32 DisplayMovieIndex;
	static bool  bShowLoadingScreen;
public:

	// Set which background will be displayed on the loading screen by index.
	// The "SetDisplayBackgroundManually" option in Background setting needs to be "true" to use this function.
	UFUNCTION(BlueprintCallable, Category = "Loading Screen")
		static void SetDisplayBackgroundIndex (int32 BackgroundIndex);

	// Set which text will be displayed on the loading screen by index.
	// The "SetDisplayTipTextManually" option in Tip Widget setting needs to be "true" to use this function.
	UFUNCTION(BlueprintCallable, Category = "Loading Screen")
		static void SetDisplayTipTextIndex (int32 TipTextIndex);

	// Set which movie will be displayed on the loading screen by index.
	// The "SetDisplayMovieIndexManually" option needs to be "true" to use this function.
	UFUNCTION(BlueprintCallable, Category = "Loading Screen")
		static void SetDisplayMovieIndex (int32 MovieIndex);


	// Set enable/disable the loading screen for next levels
	UFUNCTION(BlueprintCallable, Category = "Loading Screen")
		static void SetEnableLoadingScreen (bool bIsEnableLoadingScreen);


	// Get enable/disable the loading screen for next levels
	UFUNCTION(BlueprintPure, Category = "Loading Screen")
		static inline bool GetIsEnableLoadingScreen () { return bShowLoadingScreen; }

	// Stop the loading screen. To use this function, you must enable the "bAllowEngineTick" option.
	// Call this function in BeginPlay event to stop the Loading Screen (works with Delay node).
	UFUNCTION(BlueprintCallable, Category = "Loading Screen")
		static void StopLoadingScreen ();


	static inline int32 GetDisplayBackgroundIndex () { return DisplayBackgroundIndex; }
	static inline int32 GetDisplayTipTextIndex    () { return DisplayTipTextIndex; }
	static inline int32 GetDisplayMovieIndex      () { return DisplayMovieIndex; }

	//
	// Load all background images into memory
	//
	// Only use this function when:
	// "bPreloadBackgroundImages" option is checked,
	// and you called "RemovePreloadedBackgroundImages" before.
	//
	// You don't need to use this if you never call
	// "RemovePreloadedBackgroundImages" function,
	// since the background images are still in the memory
	//
	// Note: Call this function before calling "OpenLevel" node.
	//
	//
	UFUNCTION(BlueprintCallable, Category = "Loading Screen")
		static void PreloadBackgroundImages();

	// 
	// Remove all preloaded background images
	// 
	// Only need to use this function when
	// "bPreloadBackgroundImages" is checked.
	// 
	// You need to call "PreloadBackgroundImages"
	// to manually load all background images again.
	// 
	// 
	UFUNCTION(BlueprintCallable, Category = "Loading Screen")
		static void RemovePreloadedBackgroundImages();
};

