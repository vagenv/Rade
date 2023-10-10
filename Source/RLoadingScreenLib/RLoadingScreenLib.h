// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "Modules/ModuleManager.h"

struct FALoadingScreenSettings;
class  UTexture2D;


class FRLoadingScreenModule : public IModuleInterface
{
public:

	// Called right after the module DLL has been loaded and the module object has been created
	virtual void StartupModule () override;

	// Called before the module is unloaded, right before the module object is destroyed.
	virtual void ShutdownModule () override;

	// return True for "gameplay modules", or false for engine code modules, plugins, etc.
	virtual bool IsGameModule () const override;

	// Singleton-like access to this module's interface. This is just for convenience!
	// Beware of calling this during the shutdown phase, though. Your module might have been unloaded already.
	static inline FRLoadingScreenModule& Get () {
		return FModuleManager::LoadModuleChecked<FRLoadingScreenModule> ("RLoadingScreen");
	}

	// Checks to see if this module is loaded and ready. It is only valid to call Get() if IsAvailable() returns true.
	// return True if the module is loaded and ready to use
	static inline bool IsAvailable () {
		return FModuleManager::Get ().IsModuleLoaded ("RLoadingScreen");
	}


	TArray<UTexture2D*> GetBackgroundImages();

	// Check if "bPreloadBackgroundImages" option is enabled
	bool IsPreloadBackgroundImagesEnabled();

	// Is showing Startup Loading Screen?
	bool IsStartupLoadingScreen () { return bIsStartupLoadingScreen; }

	// Load all background images from settings into array
	void LoadBackgroundImages ();

	// Remove all background images from array
	void RemoveAllBackgroundImages ();

private:
	// Loading screen callback, it won't be called if we've already explicitly setup the loading screen
	void PreSetupLoadingScreen ();

	// Setup loading screen settings
	void SetupLoadingScreen (const FALoadingScreenSettings& LoadingScreenSettings);

	// Shuffle the movies list
	void ShuffleMovies (TArray<FString>& MoviesList);
private:

	// Startup background images array
	UPROPERTY()
		TArray<TObjectPtr<UTexture2D> > StartupBackgroundImages;

	// Default background images array
	UPROPERTY()
		TArray<TObjectPtr<UTexture2D> > DefaultBackgroundImages;

	bool bIsStartupLoadingScreen = false;
};

