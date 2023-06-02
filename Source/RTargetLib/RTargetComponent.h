// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "Components/SceneComponent.h"
#include "RTargetComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE (FRTargetEvent);

UCLASS(Blueprintable, BlueprintType, ClassGroup=(_Rade), meta=(BlueprintSpawnableComponent))
class RTARGETABLELIB_API URTargetComponent : public USceneComponent
{
   GENERATED_BODY()
public:

   virtual void BeginPlay () override;
   virtual void EndPlay (const EEndPlayReason::Type EndPlayReason) override;

   //==========================================================================
   //         Can this target be selected
   //==========================================================================

   UPROPERTY(BlueprintAssignable, Category = "Rade|Targetable")
      FRTargetEvent OnIsTargetable;

   UFUNCTION(BlueprintCallable, Category = "Rade|Targetable")
      void SetIsTargetable (bool CanFind);

   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Targetable")
      bool GetIsTargetable () const;

   //==========================================================================
   //         Is this target be selected
   //==========================================================================

   UPROPERTY(BlueprintAssignable, Category = "Rade|Targetable")
      FRTargetEvent OnIsTargeted;

   UFUNCTION(BlueprintCallable, Category = "Rade|Targetable")
      void SetIsTargeted (bool CanTarget);

   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Targetable")
      bool GetIsTargeted () const;

protected:

   // Is this target selected
   UPROPERTY()
      bool IsTargeted = false;

   // Can this Target be selected
   UPROPERTY()
      bool IsTargetable = true;
};

