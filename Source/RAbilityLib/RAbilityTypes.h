// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "RAbilityTypes.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE (FRAbilityEvent);


// ============================================================================
//                   Ability Component
// ============================================================================

UCLASS(Abstract, Blueprintable, BlueprintType)
class RABILITYLIB_API URAbility : public UActorComponent
{
   GENERATED_BODY()
public:

   URAbility ();
   virtual void BeginPlay () override;
   virtual void EndPlay (const EEndPlayReason::Type EndPlayReason) override;
   virtual void TickComponent (float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

   // --- Values
   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
      FString UIName = "";

   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
      float Cooldown = 3;

   // Item Icon
   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      TSoftObjectPtr<UTexture2D> Icon;

   // --- Runtime status
   UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
      bool UseBlocked = true;

   UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
      double CooldownLeft = 0;

   // --- Core Function Calls
   UFUNCTION(BlueprintCallable, Category = "Rade|Ability")
      virtual void Use ();

   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Ability")
      virtual bool CanUse () const;

   // --- Events

   // When Ability list updated
   UPROPERTY(BlueprintAssignable, Category = "Rade|Ability")
      FRAbilityEvent OnAbilityUsed;

   // When Ability list updated
   UPROPERTY(BlueprintAssignable, Category = "Rade|Ability")
      FRAbilityEvent OnAbilityStatusUpdated;

protected:
};

// ============================================================================
//                   Ability Util Library
// ============================================================================


UCLASS()
class RABILITYLIB_API URAbilityUtilLibrary : public UBlueprintFunctionLibrary
{
   GENERATED_BODY()
public:


};

