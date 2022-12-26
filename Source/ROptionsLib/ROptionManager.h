// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "ROptionManager.generated.h"

// Manager for Audio/Video/Input
UCLASS()
class ROPTIONSLIB_API UROptionManager : public UBlueprintFunctionLibrary
{
   GENERATED_BODY()
public:

   //////////////////////////////////////////////////////////////////////////////////////////////////////////////

   //               Input Settings

   // Get All Action Inputs
   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Settings")
      static void GetAllActionInput(TArray<FText>&InputActions, TArray<FText>&InputKeys);

   // Get Action Input with Name
   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Settings")
      static void GetActionInput(const FName& ActionName, FText& ActionKey);

   // Set Action Input with Name
   UFUNCTION(BlueprintCallable, Category = "Rade|Settings")
      static void SetActionInput(const FName& ActionName, const FText& ActionKey);

   //////////////////////////////////////////////////////////////////////////////////////////////////////////////

   //               Audio Settings

   // Get Global Sound Volume
   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Settings")
      static void GetGlobalSoundVolume(UObject* WorldContextObject, float & Volume);

   // Set Global Sound Volume
   UFUNCTION(BlueprintCallable, Category = "Rade|Settings")
      static void SetGlobalSoundVolume(UObject* WorldContextObject, const float NewVolume);


    //////////////////////////////////////////////////////////////////////////////////////////////////////////////

    //                Graphics Quality

   // Get Video Quality Settings
   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Settings")
      static bool GetVideoQualitySettings(int32& ResolutionQuality,
                                          int32& ViewDistance ,
                                          int32& AntiAliasing,
                                          int32& TextureQuality,
                                          int32& ShadowQuality,
                                          int32& EffectQuality,
                                          int32& PostProcessQuality);
   // Set Video Quality Settings
   UFUNCTION(BlueprintCallable, Category = "Rade|Settings")
      static bool SetVideoQualitySettings(const int32 ResolutionQuality,
                                          const int32 ViewDistance,
                                          const int32 AntiAliasing,
                                          const int32 TextureQuality,
                                          const int32 ShadowQuality,
                                          const int32 EffectQuality,
                                          const int32 PostProcessQuality);

   //////////////////////////////////////////////////////////////////////////////////////////////////////////////

   //               Screen  Resolution


   // Update Screen Resolution
   UFUNCTION(BlueprintCallable, Category = "Rade|Settings")
      static bool ChangeScreenResolution(const int32 Width, const int32 Height, const bool Fullscreen);

   // Set Screen Resolution without current
   UFUNCTION(BlueprintCallable, Category = "Rade|Settings")
      static bool SetScreenResolution(const int32 Width, const int32 Height, const bool Fullscreen);

   // Get a list of supported resolution
   UFUNCTION(BlueprintPure, Category = "Rade|Settings")
      static bool GetSupportedScreenResolutions(TArray<FString>& Resolutions);

   // Get current Screen Resolution
   UFUNCTION(BlueprintPure, Category = "Rade|Settings")
      static bool GetScreenResolutions(int32 & Width, int32 & Height);

   // Save Video Quality
   UFUNCTION(BlueprintCallable, Category = "Rade|Settings")
      static bool SaveVideoModeAndQuality();

private:
   // Try to get the Game User Settings
   static UGameUserSettings* GetGameUserSettings();
};

