// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "RUILib/RUIDescription.h"
#include "Engine/DataTable.h"
#include "RAbilityTypes.generated.h"

class URAbility;

// ============================================================================
//                   AbilityInfo
// ============================================================================

USTRUCT(Blueprintable, BlueprintType)
struct RABILITYLIB_API FRAbilityInfo : public FTableRowBase
{
   GENERATED_BODY()

   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      TSoftClassPtr<URAbility> AbilityClass;

   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      FRUIDescription Description;

   // Level -> scaling info?

   bool operator == (const FRAbilityInfo &AbilityInfo) const noexcept;
   bool operator != (const FRAbilityInfo &AbilityInfo) const noexcept;

   bool IsEmpty () const noexcept {
      return AbilityClass.IsNull () && Description.IsEmpty ();
   };
};

UCLASS(ClassGroup=(_Rade))
class RABILITYLIB_API URAbilityInfolLibrary : public UBlueprintFunctionLibrary
{
   GENERATED_BODY()
public:

   UFUNCTION(BlueprintPure, Category = "Rade|Ability")
	   static bool AbilityInfo_IsEmpty (const FRAbilityInfo& AbilityInfo);

   UFUNCTION(BlueprintPure, Category = "Rade|Ability", meta=(DisplayName="==", CompactNodeTitle="=="))
	   static bool AbilityInfo_EqualEqual (const FRAbilityInfo& A, const FRAbilityInfo& B);

   UFUNCTION(BlueprintPure, Category = "Rade|Ability", meta=(DisplayName="!=", CompactNodeTitle="!="))
	   static bool AbilityInfo_NotEqual (const FRAbilityInfo& A, const FRAbilityInfo& B);
};


