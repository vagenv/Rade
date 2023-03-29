// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "RUIDescription.generated.h"

USTRUCT(BlueprintType)
struct RUTILLIB_API FRUIDescription
{
   GENERATED_BODY()

   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
      FString Label = FString ("?");

   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
      FString Tooltip = FString ("???");

   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
      TSoftObjectPtr<UTexture2D> Icon;

   bool operator == (const FRUIDescription &res) const noexcept;
};

// ============================================================================
//                   Util Library
// ============================================================================

UCLASS()
class RUTILLIB_API URUIDescriptionUtilLibrary : public UBlueprintFunctionLibrary
{
   GENERATED_BODY()
public:

   UFUNCTION(BlueprintPure, Category = "Rade|Util", meta=(DisplayName="Equal (FRUIDescription)", CompactNodeTitle="=="))
	   static bool FRUIDescription_EqualEqual (const FRUIDescription& A, const FRUIDescription& B);
};
