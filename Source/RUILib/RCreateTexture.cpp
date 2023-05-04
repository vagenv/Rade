// Copyright 2015-2023 Vagen Ayrapetyan

#include "RCreateTexture.h"
#include "RUtilLib/RLog.h"

// ============================================================================
//                   Util Library
// ============================================================================

UTexture2D* URCreateTextureLibrary::CreateTextureSync (
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

//=============================================================================
//                   Async task
//=============================================================================

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

