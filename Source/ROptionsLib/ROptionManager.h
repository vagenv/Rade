// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "ROptionManager.generated.h"

USTRUCT(BlueprintType)
struct ROPTIONSLIB_API FRScreenResolution
{
   GENERATED_BODY()

   UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 Width = 0;

   UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 Height = 0;

   bool operator == (const FRScreenResolution &res) const noexcept;
};


USTRUCT(BlueprintType)
struct ROPTIONSLIB_API FRVideoQuality
{
   GENERATED_BODY()

   UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
      float ResolutionQuality = 0;

   UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
      int32 ViewDistance = 0;

   UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
      int32 AntiAliasing = 0;

   UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
      int32 TextureQuality = 0;

   UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
      int32 ShadowQuality = 0;

   UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
      int32 EffectQuality = 0;

   UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
      int32 PostProcessQuality = 0;
};

// Manager for Audio/Video/Input
UCLASS()
class ROPTIONSLIB_API UROptionManager : public UBlueprintFunctionLibrary
{
   GENERATED_BODY()

   // Try to get the Game User Settings
   static UGameUserSettings* GetGameUserSettings ();
public:

   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Settings")
      static FString ToString (const FRScreenResolution &Resolution);

   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Settings")
      static FRScreenResolution ToResolution (const FString &Resolution);

   UFUNCTION(BlueprintPure, meta=(DisplayName="Equal (FRScreenResolution)", CompactNodeTitle="==", BlueprintThreadSafe))
	   static bool EqualEqual_FRScreenResolution (const FRScreenResolution& A, const FRScreenResolution& B);



   //==========================================================================
   //               Screen  Resolution
   //==========================================================================


   // Get a list of supported resolution
   UFUNCTION(BlueprintCallable, Category = "Rade|Settings")
      static bool GetSupportedScreenResolutions (TArray<FRScreenResolution>& Resolutions);

   // Get a current resolution
   UFUNCTION(BlueprintCallable, Category = "Rade|Settings")
      static bool GetCurrentScreenResolution (FRScreenResolution &Resolutions);

   // Update Screen Resolution
   UFUNCTION(BlueprintCallable, Category = "Rade|Settings")
      static bool ChangeScreenResolution (const FRScreenResolution &Resolution, EWindowMode::Type WindowMode);


   //==========================================================================
   //                Graphics Quality
   //==========================================================================

   // --- Values

   // ResolutionQuality [10.0f - 100.0f];
   // Other Value 0:low, 1:medium, 2:high, 3:epic, 4:cinematic

   // Get Video Quality Settings
   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Settings")
      static bool GetVideoQualitySettings (float& ResolutionQuality,
                                           int32& ViewDistance ,
                                           int32& AntiAliasing,
                                           int32& TextureQuality,
                                           int32& ShadowQuality,
                                           int32& EffectQuality,
                                           int32& PostProcessQuality);
   // Set Video Quality Settings
   UFUNCTION(BlueprintCallable, Category = "Rade|Settings")
      static bool SetVideoQualitySettings (float ResolutionQuality,
                                           int32 ViewDistance,
                                           int32 AntiAliasing,
                                           int32 TextureQuality,
                                           int32 ShadowQuality,
                                           int32 EffectQuality,
                                           int32 PostProcessQuality);

   // Save Video Quality
   UFUNCTION(BlueprintCallable, Category = "Rade|Settings")
      static bool SaveVideoModeAndQuality ();

   //==========================================================================
   //               Input Settings
   //==========================================================================

   // Get All Action Inputs
   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Settings")
      static void GetAllActionInput(TArray<FText>&InputActions, TArray<FText>&InputKeys);

   // Get Action Input with Name
   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Settings")
      static void GetActionInput (const FName& ActionName, FText& ActionKey);

   // Set Action Input with Name
   UFUNCTION(BlueprintCallable, Category = "Rade|Settings")
      static void SetActionInput (const FName& ActionName, const FText& ActionKey);

   //==========================================================================
   //               Audio Settings
   //==========================================================================

   // Get Global Sound Volume
   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Settings")
      static void GetGlobalSoundVolume (UObject* WorldContextObject, float & Volume);

   // Set Global Sound Volume
   UFUNCTION(BlueprintCallable, Category = "Rade|Settings")
      static void SetGlobalSoundVolume (UObject* WorldContextObject, const float NewVolume);

};

