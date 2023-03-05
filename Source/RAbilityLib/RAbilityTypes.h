// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "RAbilityTypes.generated.h"

// ============================================================================
//                   Core Stat
// ============================================================================


UCLASS(Abstract, Blueprintable, BlueprintType)
class RABILITYLIB_API URAbility : public UActorComponent
{
   GENERATED_BODY()
public:

   URAbility();
   virtual void BeginPlay () override;
   virtual void EndPlay (const EEndPlayReason::Type EndPlayReason) override;
   virtual void TickComponent (float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

   // --- Values
   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
      FString UIName = "";

   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
      float Cooldown = 3;

   // --- Events

   UFUNCTION(BlueprintCallable, Category = "Rade|Ability")
      virtual void Use ();

   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Ability")
      virtual bool CanUse () const;

   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Ability")
      double GetCooldownLeft () const;

protected:


   double UsedLast = 0;
};



UCLASS()
class RABILITYLIB_API URAbilityUtilLibrary : public UBlueprintFunctionLibrary
{
   GENERATED_BODY()
public:

   // UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Status",
   //    meta = (DisplayName = "<", CompactNodeTitle = "<"))
   //    static bool FRCoreStats_Less (const FRCoreStats &a, const FRCoreStats &b) {
   //       return (a < b);
   //    }
};

