// Copyright 2015-2023 Vagen Ayrapetyan

#include "RViewCapture.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Camera/CameraComponent.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/GameplayStatics.h"

#include "RLog.h"

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

TArray<uint8> ARViewCapture::GetScreenShot (UObject* WorldContextObject)
{
   TArray<uint8> Result;
   if (!ensure (IsValid (WorldContextObject))) return Result;

   UWorld* World = WorldContextObject->GetWorld ();
   if (!ensure (World)) return Result;

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
   Result.Reserve (BufferSize);
   Result.AddUninitialized (BufferSize);

   // Copy BufferSize number of bytes starting from the memory address where ColorArray's bulk data starts,
   // to a space in memory starting from the memory address where BinaryTexture's bulk data starts.
   FMemory::Memcpy (Result.GetData (), ColorArray.GetData (), BufferSize);

   return Result;
}

//=============================================================================
//                   Async task
//=============================================================================

UTexture2D* URCreateTextureAsync::CreateTextureSync (
   UObject*             Outer,
   const TArray<uint8>& PixelData,
   int32                InSizeX,
   int32                InSizeY,
   EPixelFormat         InFormat)
{
   // --- Ensure validity of input
   UTexture2D* NewTexture = nullptr;
   const int32 BufferSize = PixelData.Num ();

   if (!BufferSize) return NewTexture;
   int PixelFormatSize = GPixelFormats[InFormat].BlockBytes;
   if (InSizeX <= 0 || InSizeY <= 0) {
      const uint32 Resolution = FMath::Sqrt (BufferSize / (double) PixelFormatSize);
      InSizeX = Resolution;
      InSizeY = Resolution;
   }
   if (!ensure ((PixelFormatSize * InSizeX * InSizeY) == BufferSize)) return NewTexture;

   // --- Create transient texture with parent.
   FName TextureName = MakeUniqueObjectName (Outer, UTexture2D::StaticClass ());
   NewTexture = NewObject<UTexture2D> (Outer, TextureName, RF_Transient);

   // --- Meta data
   NewTexture->SetPlatformData (new FTexturePlatformData ());
   NewTexture->GetPlatformData ()->SizeX = InSizeX;
   NewTexture->GetPlatformData ()->SizeY = InSizeY;
   NewTexture->GetPlatformData ()->SetNumSlices (1);
   NewTexture->GetPlatformData ()->PixelFormat = InFormat;

   // --- Set mip map info
   FTexture2DMipMap* Mip = new FTexture2DMipMap ();
   NewTexture->GetPlatformData ()->Mips.Add (Mip);
   Mip->SizeX = InSizeX;
   Mip->SizeY = InSizeY;
   Mip->SizeZ = 1;

   // --- Allocate and set texture data
   Mip->BulkData.Lock (LOCK_READ_WRITE);
   void* MipBulkData = Mip->BulkData.Realloc (BufferSize);
   FMemory::Memcpy (MipBulkData, PixelData.GetData (), BufferSize);
   Mip->BulkData.Unlock ();

   // Register new object with engine. Must be main thread.
   if (IsInGameThread ()) NewTexture->UpdateResource ();

   return NewTexture;
}


URCreateTextureAsync* URCreateTextureAsync::CreateTextureAsync (
   UObject*             Outer,
   const TArray<uint8>& PixelData,
   int32                InSizeX,
   int32                InSizeY,
   EPixelFormat         InFormat)
{
   const int32 BufferSize = PixelData.Num ();

   if (!BufferSize) return nullptr;
   int PixelFormatSize = GPixelFormats[InFormat].BlockBytes;
   if (InSizeX <= 0 || InSizeY <= 0) {
      const uint32 Resolution = FMath::Sqrt (BufferSize / (double) PixelFormatSize);
      InSizeX = Resolution;
      InSizeY = Resolution;
   }
   if (!ensure ((PixelFormatSize * InSizeX * InSizeY) == BufferSize)) return nullptr;


	URCreateTextureAsync* BlueprintNode = NewObject<URCreateTextureAsync>();
   BlueprintNode->Outer     = Outer;
   BlueprintNode->PixelData = PixelData;
   BlueprintNode->SizeX     = InSizeX;
   BlueprintNode->SizeY     = InSizeY;
   BlueprintNode->Format    = InFormat;
	return BlueprintNode;
}

void URCreateTextureAsync::Activate ()
{
   // --- Copied from AsyncTaskDownloadImage
   if (UTexture2DDynamic* Texture = UTexture2DDynamic::Create (SizeX, SizeY)) {
      Texture->SRGB = true;
      Texture->UpdateResource ();

      FTexture2DDynamicResource* TextureResource = static_cast<FTexture2DDynamicResource*>(Texture->GetResource ());
      if (TextureResource) {
         ENQUEUE_RENDER_COMMAND (FWriteRawDataToTexture) (
            [TextureResource, PixelData = MoveTemp (PixelData)](FRHICommandListImmediate& RHICmdList)
            {
               TextureResource->WriteRawToTexture_RenderThread (PixelData);
            });
      }
      Loaded.Broadcast (Texture);
      return;
   }

   // const int32 NumMips = 1;

   // // Mip0Data
   // const int32 Mip0Size = SizeX * SizeY * GPixelFormats[Format].BlockBytes;

   // TArray<uint8> Mip0Data;
   // Mip0Data.SetNum(Mip0Size);

   // // Fill Mip0Data;
   // // below "green" mip0 is constructed in a very bruteforce way
   // for (int32 Index = 0; Index < Mip0Size; Index += 4)
   // {
   //    Mip0Data[Index] = 0;
   //    Mip0Data[Index + 1] = 255;
   //    Mip0Data[Index + 2] = 0;
   //    Mip0Data[Index + 3] = 255;
   // }

   // // make sure UTexture2D created on the game thread
   // // game thread task should be spawned in case this code is being executed on the separated thread
   // check (IsInGameThread());

   // // Create transient texture
   // UTexture2D* NewTexture = NewObject<UTexture2D>(
   //    GetTransientPackage(),
   //    MakeUniqueObjectName(GetTransientPackage(), UTexture2D::StaticClass()),
   //    RF_Transient
   // );
   // check(IsValid(NewTexture));

   // // never link the texture to Unreal streaming system
   // NewTexture->NeverStream = true;
   // {
   //    // --- Allocate dummy mipmap of 1x1 size
   //    NewTexture->SetPlatformData (new FTexturePlatformData ());
   //    NewTexture->GetPlatformData ()->SizeX = 1;
   //    NewTexture->GetPlatformData ()->SizeY = 1;
   //    NewTexture->GetPlatformData ()->SetNumSlices (1);
   //    NewTexture->GetPlatformData ()->PixelFormat = Format;

   //    // --- Set mip map info
   //    FTexture2DMipMap* Mip = new FTexture2DMipMap ();
   //    NewTexture->GetPlatformData ()->Mips.Add (Mip);
   //    Mip->SizeX = 1;
   //    Mip->SizeY = 1;
   //    Mip->SizeZ = 1;

   //    // GPixelFormats contains meta information for each pixel format
   //    const uint32 MipBytes = Mip->SizeX * Mip->SizeY * GPixelFormats[Format].BlockBytes;
   //    {
   //       Mip->BulkData.Lock(LOCK_READ_WRITE);
   //       void* TextureData = Mip->BulkData.Realloc (MipBytes);
   //       static TArray<uint8> DummyBytes;
   //       DummyBytes.SetNum(MipBytes);
   //       FMemory::Memcpy(TextureData, DummyBytes.GetData(), MipBytes);
   //       Mip->BulkData.Unlock();
   //    }

   //    // construct texture
   //    NewTexture->UpdateResource ();
   // }

   // // async create texture on the separate thread
   // FTexture2DRHIRef RHITexture2D;

   // Async(
   //    EAsyncExecution::Thread,
   //    [&RHITexture2D, this, NumMips, Mip0Data]
   //    {

   //       const uint32 DataSize = CalculateImageBytes (SizeX, SizeY, 1, Format);
   //       FDisplayClusterResourceBulkData BulkDataInterface (TextureData, DataSize);

   //       FRHITextureCreateDesc Info;
   //       Info.SetFormat (Format);
   //       Info.SetNumMips (NumMips);
   //       Info.SetExtent (SizeX, SizeY);
   //       Info.SetBulkData (&BulkDataInterface);

   //       RHITexture2D = RHICreateTexture (Info);

   //       // RHITexture2D = RHIAsyncCreateTexture2D (
   //       //    SizeX, SizeY,
   //       //    Format,
   //       //    NumMips,
   //       //    TexCreate_ShaderResource, Mip0Data.GetData(), NumMips
   //       // );
   //    }
   // ).Wait();

   // // link RHI texture to UTexture2D
   // ENQUEUE_RENDER_COMMAND(UpdateTextureReference)(
   //    [NewTexture, RHITexture2D](FRHICommandListImmediate& RHICmdList)
   //    {
   //       RHIUpdateTextureReference (NewTexture->TextureReference.TextureReferenceRHI, RHITexture2D);
   //       NewTexture->RefreshSamplerStates( );
   //    }
   // );
}

