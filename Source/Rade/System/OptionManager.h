// Copyright 2015 Vagen Ayrapetyan

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "OptionManager.generated.h"

// Manager for Audio/Video/Input
UCLASS()
class RADE_API UOptionManager : public UBlueprintFunctionLibrary
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

   //               Config  Settings  

   // Reload Config [Need to be done after every update of that config]
   UFUNCTION(BlueprintCallable, Category = "Rade|Settings")
      static void ReloadConfig();

   // Send FileName-> Recieve Full path in config folder
   FORCEINLINE static FString GetConfigFullAddress(const FString & FileName);


   //                         Strings

   // Save Config - String
   UFUNCTION(BlueprintCallable, Category = "Rade|Settings")
      static void SaveConfig_String(const FString& FileName, const FString& SettingGroup, const FString& SettingName,const FString& SettingValue);
   
   // Load Config - String
   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Settings")
      static void LoadConfig_String(const FString& FileName, const FString& SettingGroup, const FString& SettingName, FString& SettingValue);

   //                         Int

   // Save Config - Int
   UFUNCTION(BlueprintCallable, Category = "Rade|Settings")
      static void SaveConfig_Int(const FString& FileName, const FString& SettingGroup, const FString& SettingName, const int32& SettingValue);
   
   // Load Config - Int
   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Settings")
      static void LoadConfig_Int(const FString& FileName, const FString& SettingGroup, const FString& SettingName, int32& SettingValue);

   //                        Float

   // Save Config - Float
   UFUNCTION(BlueprintCallable, Category = "Rade|Settings")
      static void SaveConfig_Float(const FString& FileName, const FString& SettingGroup, const FString& SettingName, const float& SettingValue);
   
   // Load Config - Float
   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Settings")
      static void LoadConfig_Float(const FString& FileName, const FString& SettingGroup, const FString& SettingName, float& SettingValue);

   //                         Vector

   // Save Config - Vector
   UFUNCTION(BlueprintCallable, Category = "Rade|Settings")
      static void SaveConfig_Vector(const FString& FileName, const FString& SettingGroup, const FString& SettingName, const FVector& SettingValue);
   
   // Load Config - Vector
   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Settings")
      static void LoadConfig_Vector(const FString& FileName, const FString& SettingGroup, const FString& SettingName, FVector& SettingValue);

   //                         Rotator

   // Save Config - Rotator
   UFUNCTION(BlueprintCallable, Category = "Rade|Settings")
      static void SaveConfig_Rotator(const FString& FileName, const FString& SettingGroup, const FString& SettingName, const FRotator& SettingValue);
   
   // Load Config - Rotator
   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Settings")
      static void LoadConfig_Rotator(const FString& FileName, const FString& SettingGroup, const FString& SettingName, FRotator& SettingValue);
   
   //                         Color

   // Save Config - Color
   UFUNCTION(BlueprintCallable, Category = "Rade|Settings")
      static void SaveConfig_Color(const FString& FileName, const FString& SettingGroup, const FString& SettingName, const FColor& SettingValue);
   
   // Load Config - Color
   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Settings")
      static void LoadConfig_Color(const FString& FileName, const FString& SettingGroup, const FString& SettingName, FColor& SettingValue);



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
