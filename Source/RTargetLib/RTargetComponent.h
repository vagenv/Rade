// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "Components/SceneComponent.h"
#include "RTargetComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE (FRTargetEvent);

UCLASS(Blueprintable, BlueprintType, ClassGroup=(_Rade), meta=(BlueprintSpawnableComponent))
class RTARGETLIB_API URTargetComponent : public USceneComponent
{
   GENERATED_BODY()
public:

   virtual void BeginPlay () override;
   virtual void EndPlay (const EEndPlayReason::Type EndPlayReason) override;

protected:

   void RegisterTarget ();
   void UnregisterTarget ();

   //==========================================================================
   //         Can this target be selected
   //==========================================================================
public:

   UPROPERTY(BlueprintAssignable, Category = "Rade|Target")
      FRTargetEvent OnIsTargetable;

   UFUNCTION(BlueprintCallable, Category = "Rade|Target")
      void SetIsTargetable (bool CanFind);

   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Target")
      bool GetIsTargetable () const;

   //==========================================================================
   //         Is this target be selected
   //==========================================================================

   UPROPERTY(BlueprintAssignable, Category = "Rade|Target")
      FRTargetEvent OnIsTargeted;

   UFUNCTION(BlueprintCallable, Category = "Rade|Target")
      void SetIsTargeted (bool CanTarget);

   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Target")
      bool GetIsTargeted () const;

protected:

   // Is this target selected
   UPROPERTY()
      bool IsTargeted = false;

   // Can this Target be selected
   UPROPERTY()
      bool IsTargetable = true;
};

