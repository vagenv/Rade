// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "RItemTypes.h"
#include "Components/ActorComponent.h"
#include "RItemPickupMgrComponent.generated.h"

class ARItemPickup;

DECLARE_DYNAMIC_MULTICAST_DELEGATE (FRItemPickupMgrEvent);

UENUM(BlueprintType)
enum class ERClosestPickupSelectionMethod : uint8
{
   Direction UMETA (DisplayName = "Direction"),
   Distance  UMETA (DisplayName = "Distance")
};

UCLASS(Blueprintable, BlueprintType, ClassGroup=(_Rade), meta=(BlueprintSpawnableComponent))
class RINVENTORYLIB_API URItemPickupMgrComponent : public USceneComponent
{
   GENERATED_BODY()
public:

   // Base events
   URItemPickupMgrComponent ();
   virtual void BeginPlay () override;
   virtual void EndPlay (const EEndPlayReason::Type EndPlayReason) override;

   //==========================================================================
   //                 Pickups
   //==========================================================================

private:
   // List of currently available pickup
   UPROPERTY()
      TArray<TWeakObjectPtr<const ARItemPickup> > CurrentPickups;
protected:
   UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rade|Inventory")
      TWeakObjectPtr<const ARItemPickup> ClosestPickup = nullptr;


   UFUNCTION()
      void ReportPickupListUpdated ();

   UPROPERTY(BlueprintAssignable, Category = "Rade|Inventory")
      FRItemPickupMgrEvent OnPickupListUpdated;

   UFUNCTION()
      void ReportClosestPickupUpdated ();

   UPROPERTY(BlueprintAssignable, Category = "Rade|Inventory")
      FRItemPickupMgrEvent OnClosestPickupUpdated;

public:

   // Owner overlaps pickup
   bool Pickup_Register (const ARItemPickup* Pickup);

   // Owner no longer overlaps
   bool Pickup_Unregister (const ARItemPickup* Pickup);

protected:

   UPROPERTY()
      FTimerHandle CheckClosestPickupHandle;

   UFUNCTION()
      void SetCheckClosestPickupEnabled (bool Enabled);

   UFUNCTION()
      void CheckClosestPickup ();

   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Inventory")
      ERClosestPickupSelectionMethod SelectionMethod;

   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Inventory")
      float CheckClosestDelay = 0.5f;
};

