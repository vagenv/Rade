// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "RUIDescription.generated.h"

USTRUCT(Blueprintable, BlueprintType)
struct RUILIB_API FRUIDescription
{
   GENERATED_BODY()

   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      FString Label;

   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      FString Tooltip;

   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      TSoftObjectPtr<UTexture2D> Icon;

   bool operator == (const FRUIDescription &Description) const noexcept;
   bool operator != (const FRUIDescription &Description) const noexcept;

   bool IsEmpty () const noexcept {
      return Icon.IsNull () && Label.IsEmpty () && Tooltip.IsEmpty ();
   };
};


// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class URGetDescriptionInterface : public UInterface
{
   GENERATED_BODY()
};

class RUILIB_API IRGetDescriptionInterface
{
   GENERATED_BODY()

public:

   UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Rade|UI")
      FRUIDescription GetDescription () const;
};

// ============================================================================
//                   Util Library
// ============================================================================

UCLASS()
class RUILIB_API URUIDescriptionUtilLibrary : public UBlueprintFunctionLibrary
{
   GENERATED_BODY()
public:

   UFUNCTION(BlueprintPure, Category = "Rade|Ability")
      static bool FRUIDescription_IsEmpty (const FRUIDescription& Description);

   UFUNCTION(BlueprintPure, Category = "Rade|UI", meta=(DisplayName="==", CompactNodeTitle="=="))
      static bool FRUIDescription_EqualEqual (const FRUIDescription& A, const FRUIDescription& B);

   UFUNCTION(BlueprintPure, Category = "Rade|UI", meta=(DisplayName="!=", CompactNodeTitle="!="))
      static bool FRUIDescription_NotEqual (const FRUIDescription& A, const FRUIDescription& B);
};

