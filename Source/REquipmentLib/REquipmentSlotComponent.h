// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "Components/ActorComponent.h"
#include "REquipmentTypes.h"
#include "RUtilLib/RUIDescription.h"
#include "REquipmentSlotComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE (FREquipmentSlotEvent);

// Status Manager Component.
UCLASS(Abstract, Blueprintable, BlueprintType, ClassGroup=(_Rade))
class REQUIPMENTLIB_API UREquipmentSlotComponent : public USceneComponent
{
   GENERATED_BODY()
public:

   // Base events
   UREquipmentSlotComponent ();
   virtual void GetLifetimeReplicatedProps (TArray<FLifetimeProperty> &OutLifetimeProps) const override;
   virtual void BeginPlay () override;
   virtual void EndPlay (const EEndPlayReason::Type EndPlayReason) override;

   // Called when variables replicated
   UFUNCTION()
      void OnRep_Slot ();

   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Equipment")
      FRUIDescription Description;

   UPROPERTY(ReplicatedUsing = "OnRep_Slot", Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Rade|Equipment")
      bool Busy = false;

   UPROPERTY(ReplicatedUsing = "OnRep_Slot", Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Rade|Equipment")
      FREquipmentData EquipmentData;

   //==========================================================================
   //                 Events
   //==========================================================================

   UFUNCTION()
      void ReportOnSlotUpdated ();

   UPROPERTY(BlueprintAssignable, Category = "Rade|Equipment")
      FREquipmentSlotEvent OnSlotUpdated;
};

