// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "GameFramework/Actor.h"
#include "RUtilLib/RUtilTypes.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "RViewCapture.generated.h"

class UTexture2DDynamic;

class USceneCaptureComponent2D;

UCLASS()
class RUTILLIB_API ARViewCapture : public AActor
{
   GENERATED_BODY()
public:

   ARViewCapture ();

   UFUNCTION(BlueprintCallable,
             Category = "Rade|Util",
             meta = (ExpandEnumAsExecs = "Outcome",
                     ToolTip = "Align the camera of this View Capture actor with the player's camera."))
      void SetCameraToPlayerView (ERActionResult &Outcome);
      bool SetCameraToPlayerView ();

   UFUNCTION(BlueprintCallable,
             Category = "Rade|Util",
             meta = (ExpandEnumAsExecs = "Outcome",
                     ToolTip = "Capture the player's view.\n\nResolution - a power of 2 resolution for the view capture, like 512"))
      void CapturePlayersView (ERActionResult &Outcome, int32 Resolution, TArray<FColor> &ColorData);
      bool CapturePlayersView (int32 Resolution, TArray<FColor> &ColorData);


   UPROPERTY(EditAnywhere, Transient)
      USceneCaptureComponent2D* Camera;

	UFUNCTION(BlueprintCallable,
             Category = "Rade|Util",
               meta = (HidePin          = "WorldContextObject",
                       DefaultToSelf    = "WorldContextObject"))
		static TArray<uint8> GetScreenShot (UObject* WorldContextObject);
};



UCLASS()
class RUTILLIB_API URCreateTextureAsync : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:

   UFUNCTION(BlueprintCallable,
             Category = "Rade|Util",
             meta = (BlueprintInternalUseOnly = "true",
                     HidePin      = "WorldContextObject",
                     WorldContext = "WorldContextObject")
                     )
      static UTexture2D* CreateTextureSync (UObject* WorldContextObject,
                                            const TArray<uint8>& PixelData,
                                            int32 InSizeX = 0, int32 InSizeY = 0,
                                            EPixelFormat InPixelFormat = EPixelFormat::PF_B8G8R8A8);

	UFUNCTION(BlueprintCallable,
             Category = "Rade|Util",
            //  meta = (HidePin = "Outer", DefaultToSelf = "Outer")
             meta = (BlueprintInternalUseOnly = "true",
                     HidePin      = "WorldContextObject",
                     WorldContext = "WorldContextObject")
                     )
	   static URCreateTextureAsync* CreateTextureAsync (UObject* WorldContextObject,
                                                       const TArray<uint8>& PixelData,
                                                       int32 InSizeX = 0, int32 InSizeY = 0,
                                                       EPixelFormat InPixelFormat = EPixelFormat::PF_B8G8R8A8);



   // DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRCreateTextureEvent, UTexture2D*, Texture);
   DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRCreateTextureEvent, UTexture2DDynamic*, Texture);



   // Called when all save game slots have been read
	UPROPERTY(BlueprintAssignable)
	   FRCreateTextureEvent Loaded;

   // Execution point
	virtual void Activate () override;
private:

   UObject*      Outer = nullptr;
   TArray<uint8> PixelData;
   int32         SizeX = 0;
   int32         SizeY = 0;
   EPixelFormat  Format;
};