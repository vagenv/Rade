// Copyright 2015-2023 Vagen Ayrapetyan

#include "RViewCapture.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Camera/CameraComponent.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/GameplayStatics.h"

#include "RUtilLib/RUtil.h"
#include "RUtilLib/RLog.h"

// Sets default values
ARViewCapture::ARViewCapture ()
{
   PrimaryActorTick.bCanEverTick = false;

   Camera = CreateDefaultSubobject<USceneCaptureComponent2D> (TEXT ("Camera"));
   SetRootComponent (Camera);

   // Make sure we don't capture every frame for performance, and because our render target will be made to be GC'd.
   Camera->bCaptureEveryFrame = false;
   // Set the right format to not deal with alpha issues
   Camera->CaptureSource = ESceneCaptureSource::SCS_FinalColorHDR;
}

bool ARViewCapture::SetCameraToPlayerView ()
{
   APlayerCameraManager* PlayerCamera = UGameplayStatics::GetPlayerCameraManager (GetWorld (), 0);
   if (!ensure (PlayerCamera)) return false;

   const FVector  CameraLocation = PlayerCamera->GetCameraLocation ();
   const FRotator CameraRotation = PlayerCamera->GetCameraRotation ();

   SetActorLocationAndRotation (CameraLocation, CameraRotation);
   Camera->SetWorldLocationAndRotation (CameraLocation, CameraRotation);
   Camera->FOVAngle = PlayerCamera->GetFOVAngle ();

   return true;
}

bool ARViewCapture::CapturePlayersView (int32 Resolution, TArray<FColor>& ColorData)
{
   // Make the resolution a power of two.
   Resolution = FMath::Pow (2., FMath::FloorLog2 (FMath::Max (Resolution, 1.) * 2. - 1.));

   // Move our actor and its camera component to player's camera.
   if (!SetCameraToPlayerView ()) return false;

   // Create a temporary object that we will let die in GC in a moment after this scope ends.
   UTextureRenderTarget2D* TextureRenderTarget = NewObject<UTextureRenderTarget2D> ();
   if (!ensure (TextureRenderTarget)) return false;
   TextureRenderTarget->InitCustomFormat (Resolution, Resolution, PF_B8G8R8A8,false);

   // Take the capture.
   Camera->TextureTarget = TextureRenderTarget;
   Camera->CaptureScene ();

   // Output the capture to a pixel array.
   ColorData.Empty ();
   ColorData.Reserve (Resolution * Resolution);
   const auto RenderResources = TextureRenderTarget->GameThread_GetRenderTargetResource ();
   if (!ensure (RenderResources)) return false;
   RenderResources->ReadPixels (ColorData);
   ColorData.Shrink ();

   return true;
}

// Function wrappers for Blueprint functions.
void ARViewCapture::SetCameraToPlayerView (ERActionResult &Outcome)
{
   Outcome = SetCameraToPlayerView () ? ERActionResult::Success : ERActionResult::Failure;
}

void ARViewCapture::CapturePlayersView (ERActionResult &Outcome, const int32 Resolution, TArray<FColor>& ColorData)
{
   Outcome = CapturePlayersView (Resolution, ColorData) ? ERActionResult::Success : ERActionResult::Failure;
}

bool ARViewCapture::GetScreenShot (UObject* WorldContextObject, TArray<uint8> &TextureData)
{
   if (!ensure (WorldContextObject)) return false;

   UWorld* World = URUtil::GetWorld (WorldContextObject);
   if (!World) return false;

   // 1. Create a temporary actor to capture the player's view.
   ARViewCapture* ViewCaptureActor = World->SpawnActor<ARViewCapture>();

   // 2. Get the player's view as a BGRA8 array of colors.
   static int32 Resolution = 256;
   TArray<FColor> ColorArray;
   ViewCaptureActor->CapturePlayersView (Resolution, ColorArray);

   // 3. Actors do not get garbage collected automatically since the level is keeping them in GC graph.
   ViewCaptureActor->Destroy ();

   // 4. Memcpy data of our color array to our BinaryTexture byte array.
   // Calculate the total number of bytes we will copy. Every color is represented by 4 bytes: R, G, B, A.
   static int32 BufferSize = ColorArray.Num () * 4;

   // Pre-allocate enough memory to fit our data. We reserve space before adding uninitialized elements to avoid array
   // growth operations and we add uninitialized elements to increase array element count properly.
   TextureData.Reserve (BufferSize);
   TextureData.AddUninitialized (BufferSize);

   // Copy BufferSize number of bytes starting from the memory address where ColorArray's bulk data starts,
   // to a space in memory starting from the memory address where BinaryTexture's bulk data starts.
   FMemory::Memcpy (TextureData.GetData (), ColorArray.GetData (), BufferSize);

   return true;
}

