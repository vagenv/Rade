// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "Components/ActorComponent.h"
#include "RMapPointComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE (FRMapPointEvent);

UCLASS(Blueprintable, BlueprintType, ClassGroup=(_Rade), meta=(BlueprintSpawnableComponent))
class RMAPLIB_API URMapPointComponent : public UActorComponent
{
   GENERATED_BODY()
public:

   virtual void BeginPlay () override;
   virtual void EndPlay (const EEndPlayReason::Type EndPlayReason) override;

protected:

   void RegisterMapPoint ();
   void UnregisterMapPoint ();

   //==========================================================================
   //         Can this Point be found
   //==========================================================================
public:

   UFUNCTION(BlueprintCallable, Category = "Rade|Interact")
      void SetCanFind (bool CanFind);

   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Interact")
      bool GetCanFind () const;

   // Called when map list has been modified
   UPROPERTY(BlueprintAssignable, Category = "Rade|Interact")
      FRMapPointEvent OnCanFindChanged;

protected:

   // Is this Interact selected
   UPROPERTY()
      bool CanFind = true;
};

