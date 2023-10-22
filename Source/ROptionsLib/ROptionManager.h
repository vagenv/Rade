// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "ROptionManager.generated.h"

// ============================================================================
//                   Screen Resolution
// ============================================================================

USTRUCT(Blueprintable, BlueprintType)
struct ROPTIONSLIB_API FRScreenResolution
{
   GENERATED_BODY()

   UPROPERTY(EditAnywhere, BlueprintReadOnly)
      int32 Width = 0;

   UPROPERTY(EditAnywhere, BlueprintReadOnly)
      int32 Height = 0;

   bool operator == (const FRScreenResolution &ScreenResolution) const noexcept;
};

// ============================================================================
//                   Video Quality
// ============================================================================

USTRUCT(Blueprintable, BlueprintType)
struct ROPTIONSLIB_API FRVideoQualitySetting
{
   GENERATED_BODY()

   UPROPERTY(EditAnywhere, BlueprintReadOnly)
      bool VSyncEnabled = 0;

   UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (ClampMin = 30, ClampMax = 240))
      int32 FrameRate = 0;

   UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (ClampMin = 0.0, ClampMax = 100.))
      float ResolutionQuality = 0;

   UPROPERTY(EditAnywhere, BlueprintReadOnly)
      bool DynamicResolution = false;

   // 5 == Custom
   UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (ClampMin = 0, ClampMax = 5))
      int32 QualityPreset = 0;


   UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (ClampMin = 0, ClampMax = 4))
      int32 AntiAliasing = 0;

   UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (ClampMin = 0, ClampMax = 4))
      int32 ViewDistance = 0;

   UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (ClampMin = 0, ClampMax = 4))
      int32 TextureQuality = 0;

   UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (ClampMin = 0, ClampMax = 4))
      int32 ShadowQuality = 0;

   UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (ClampMin = 0, ClampMax = 4))
      int32 ShadingQuality = 0;

   UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (ClampMin = 0, ClampMax = 4))
      int32 GlobalIllumination = 0;

   UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (ClampMin = 0, ClampMax = 4))
      int32 ReflectionQuality = 0;

   UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (ClampMin = 0, ClampMax = 4))
      int32 FoliageQuality = 0;

   UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (ClampMin = 0, ClampMax = 4))
      int32 EffectQuality = 0;

   UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (ClampMin = 0, ClampMax = 4))
      int32 PostProcessQuality = 0;

   bool operator == (const FRVideoQualitySetting &QualitySettings) const noexcept;
};

// ============================================================================
//                   Util Function Library
// ============================================================================

UCLASS(ClassGroup=(_Rade))
class ROPTIONSLIB_API UROptionUtilFunc : public UBlueprintFunctionLibrary
{
   GENERATED_BODY()
public:

   UFUNCTION(BlueprintPure, Category = "Rade|Settings", meta=(DisplayName="ToString (FRScreenResolution)", CompactNodeTitle="ToString"))
      static FString FRScreenResolution_ToString (const FRScreenResolution &Resolution);

   UFUNCTION(BlueprintPure, Category = "Rade|Settings", meta=(DisplayName="Equal (FRScreenResolution)", CompactNodeTitle="=="))
      static bool FRScreenResolution_EqualEqual (const FRScreenResolution& A, const FRScreenResolution& B);

   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Settings")
      static FRScreenResolution FRScreenResolution_FromString (const FString &Resolution);

   UFUNCTION(BlueprintPure, Category = "Rade|Settings", meta=(DisplayName="Equal (FRVideoQualitySetting)", CompactNodeTitle="=="))
      static bool FRVideoQualitySetting_EqualEqual (const FRVideoQualitySetting& A, const FRVideoQualitySetting& B);
};

// ============================================================================
//                   Option Manager
// ============================================================================


// Manager for Audio/Video/Input
UCLASS(ClassGroup=(_Rade))
class ROPTIONSLIB_API UROptionManager : public UBlueprintFunctionLibrary
{
   GENERATED_BODY()

   // Try to get the Game User Settings
   static UGameUserSettings* GetGameUserSettings ();
public:

   //==========================================================================
   //               Screen  Resolution
   //==========================================================================

   // Get a list of supported resolution
   UFUNCTION(BlueprintCallable, Category = "Rade|Settings")
      static bool GetSupportedScreenResolutions (TArray<FRScreenResolution>& Resolutions);

   // Get a current resolution
   UFUNCTION(BlueprintCallable, Category = "Rade|Settings")
      static bool GetCurrentScreenResolution (FRScreenResolution &Resolutions, TEnumAsByte<EWindowMode::Type> &WindowMode);

   // Update Screen Resolution
   UFUNCTION(BlueprintCallable, Category = "Rade|Settings")
      static bool SetScreenResolution (const FRScreenResolution &Resolution, const EWindowMode::Type WindowMode);

   //==========================================================================
   //                Graphics Quality
   //==========================================================================

   // --- Values

   // Get Video Quality Settings
   UFUNCTION(BlueprintCallable, Category = "Rade|Settings")
      static bool GetCurrentVideoQualitySettings (FRVideoQualitySetting& QualitySettings);

   // Set Video Quality Settings
   UFUNCTION(BlueprintCallable, Category = "Rade|Settings")
      static bool SetVideoQualitySettings (FRVideoQualitySetting QualitySettings);

   //==========================================================================
   //               Input Settings
   //==========================================================================

   // Get All Action Inputs
   UFUNCTION(BlueprintCallable, Category = "Rade|Settings")
      static bool GetAllActionInput(TArray<FText>&InputActions, TArray<FText>&InputKeys);

   // Get Action Input with Name
   UFUNCTION(BlueprintCallable, Category = "Rade|Settings")
      static bool GetActionInput (const FName& ActionName, FText& ActionKey);

   // Set Action Input with Name
   UFUNCTION(BlueprintCallable, Category = "Rade|Settings")
      static bool SetActionInput (const FName& ActionName, const FText& ActionKey);

   //==========================================================================
   //               Audio Settings
   //==========================================================================

   // Get Global Sound Volume
   UFUNCTION(BlueprintCallable, Category = "Rade|Settings")
      static bool GetGlobalSoundVolume (UObject* WorldContextObject, float & Volume);

   // Set Global Sound Volume
   UFUNCTION(BlueprintCallable, Category = "Rade|Settings")
      static bool SetGlobalSoundVolume (UObject* WorldContextObject, const float NewVolume);
};

