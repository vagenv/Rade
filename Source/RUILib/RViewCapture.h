// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "GameFramework/Actor.h"
#include "RUtilLib/RUtilTypes.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "RViewCapture.generated.h"

class UTexture2DDynamic;

class USceneCaptureComponent2D;

UCLASS()
class RUILIB_API ARViewCapture : public AActor
{
   GENERATED_BODY()
public:

   ARViewCapture ();

   UFUNCTION(BlueprintCallable,
             Category = "Rade|UI",
             meta = (ExpandEnumAsExecs = "Outcome",
                     ToolTip = "Align the camera of this View Capture actor with the player's camera."))
      void SetCameraToPlayerView (ERActionResult &Outcome);
      bool SetCameraToPlayerView ();

   UFUNCTION(BlueprintCallable,
             Category = "Rade|UI",
             meta = (ExpandEnumAsExecs = "Outcome",
                     ToolTip = "Capture the player's view.\n\nResolution - a power of 2 resolution for the view capture, like 512"))
      void CapturePlayersView (ERActionResult &Outcome, int32 Resolution, TArray<FColor> &ColorData);
      bool CapturePlayersView (int32 Resolution, TArray<FColor> &ColorData);


   UPROPERTY(VisibleAnywhere, Transient)
      TObjectPtr<USceneCaptureComponent2D> Camera;

	UFUNCTION(BlueprintCallable,
             Category = "Rade|UI",
               meta = (HidePin          = "WorldContextObject",
                       DefaultToSelf    = "WorldContextObject"))
		static bool GetScreenShot (UObject* WorldContextObject, TArray<uint8> &TextureData);
};

