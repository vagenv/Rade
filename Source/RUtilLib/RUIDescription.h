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
};

