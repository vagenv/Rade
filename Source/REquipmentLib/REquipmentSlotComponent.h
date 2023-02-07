// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "Components/ActorComponent.h"
#include "REquipmentTypes.h"
#include "REquipmentSlotComponent.generated.h"


// Status Manager Component.
UCLASS(Abstract, Blueprintable, BlueprintType)
class REQUIPMENTLIB_API UREquipmentSlotComponent : public UActorComponent
{
   GENERATED_BODY()
public:

   // Base events
   UREquipmentSlotComponent ();
   virtual void GetLifetimeReplicatedProps (TArray<FLifetimeProperty> &OutLifetimeProps) const override;
   virtual void BeginPlay () override;
   virtual void EndPlay (const EEndPlayReason::Type EndPlayReason) override;

   UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
      bool Busy = false;

   UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
      FREquipmentData EquipmentData;

   UFUNCTION(BlueprintImplementableEvent, Category = "Rade|Status")
      void Updated ();
};

