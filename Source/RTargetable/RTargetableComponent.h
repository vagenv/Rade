// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "Components/SceneComponent.h"
#include "RTargetableComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE (FRTargetableEvent);

UCLASS(Blueprintable, BlueprintType, meta=(BlueprintSpawnableComponent))
class RTARGETABLELIB_API URTargetableComponent : public USceneComponent
{
   GENERATED_BODY()
public:

   virtual void BeginPlay () override;
   virtual void EndPlay (const EEndPlayReason::Type EndPlayReason) override;

   //==========================================================================
   //         Can this target be selected
   //==========================================================================

   UPROPERTY(BlueprintAssignable, Category = "Rade|Targetable")
      FRTargetableEvent OnIsTargetable;

   UFUNCTION(BlueprintCallable, Category = "Rade|Targetable")
      void SetIsTargetable (bool CanFind);

   UFUNCTION(BlueprintCallable, Category = "Rade|Targetable")
      bool GetIsTargetable () const;

   //==========================================================================
   //         Is this target be selected
   //==========================================================================

   UPROPERTY(BlueprintAssignable, Category = "Rade|Targetable")
      FRTargetableEvent OnIsTargeted;

   UFUNCTION(BlueprintCallable, Category = "Rade|Targetable")
      void SetIsTargeted (bool CanTarget);

   UFUNCTION(BlueprintCallable, Category = "Rade|Targetable")
      bool GetIsTargeted () const;

protected:

   // Is this target selected
   UPROPERTY()
      bool IsTargeted = false;

   // Can this Target be selected
   UPROPERTY()
      bool IsTargetable = true;
};

