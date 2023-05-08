// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "RUIDescription.generated.h"

USTRUCT(BlueprintType)
struct RUILIB_API FRUIDescription
{
   GENERATED_BODY()

   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      FString Label = FString ("?");

   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      FString Tooltip = FString ("???");

   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      TSoftObjectPtr<UTexture2D> Icon;

   bool operator == (const FRUIDescription &res) const noexcept;
};

// ============================================================================
//                   Util Library
// ============================================================================

UCLASS()
class RUILIB_API URUIDescriptionUtilLibrary : public UBlueprintFunctionLibrary
{
   GENERATED_BODY()
public:

   UFUNCTION(BlueprintPure, Category = "Rade|Util", meta=(DisplayName="==", CompactNodeTitle="=="))
	   static bool FRUIDescription_EqualEqual (const FRUIDescription& A, const FRUIDescription& B);
};