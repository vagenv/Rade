// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "CoreMinimal.h"

#include "Components/SceneComponent.h"
#include "RTargetableComponent.generated.h"

UCLASS(Blueprintable, BlueprintType, meta=(BlueprintSpawnableComponent))
class RTARGETABLELIB_API URTargetableComponent : public USceneComponent
{
   GENERATED_BODY()
public:

   virtual void BeginPlay () override;
   virtual void EndPlay (const EEndPlayReason::Type EndPlayReason) override;


   UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rade|Targetable")
      bool IsTargetable = false;

   UFUNCTION(BlueprintCallable, Category = "Rade|Targetable")
      void SetTargetable (bool Enabled);
};

