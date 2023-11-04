// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "Components/ActorComponent.h"
#include "RWorldTargetMgr.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE (FRTargetableMgrEvent);

class URTargetComponent;
class URPlayerTargetMgr;

UCLASS(Blueprintable, BlueprintType, ClassGroup=(_Rade), meta=(BlueprintSpawnableComponent))
class RTARGETLIB_API URWorldTargetMgr : public UActorComponent
{
   GENERATED_BODY()
public:

   URWorldTargetMgr ();

   //==========================================================================
   //                  Target list
   //==========================================================================
private:
   // Container for current targets
   UPROPERTY()
      TArray<TWeakObjectPtr<URTargetComponent> > TargetList;
protected:
   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Target")
      const TArray<URTargetComponent*> GetTargetList () const;
public:
   // Called when target list has been modified
   UPROPERTY(BlueprintAssignable, Category = "Rade|Target")
      FRTargetableMgrEvent OnListUpdated;

   UFUNCTION()
      virtual void ReportListUpdateDelayed ();

   bool ReportListUpdatedDelayedTriggered = false;

   //==========================================================================
   //          Functions called by target components
   //==========================================================================

   UFUNCTION(BlueprintCallable, Category = "Rade|Target")
      virtual void RegisterTarget (URTargetComponent* Target);

   UFUNCTION(BlueprintCallable, Category = "Rade|Target")
      virtual void UnregisterTarget  (URTargetComponent* Target);

   //==========================================================================
   //                  Minimum search requirnments
   //==========================================================================

   // Distance at which target can be searched
   UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Rade|Target")
      float SearchDistanceMax = 5000;

   // FOV angle to search targets
   UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Rade|Target")
      float SearchAngleMax = 70;

   // Check if target can be targeted
   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Target")
      bool IsValidTarget (
         const URTargetComponent*          Target,
         const FVector                    &SearchOrigin,
         const FVector                    &SearchDirection,
         const TArray<AActor*>            &ExcludeActors,
         const TArray<URTargetComponent*> &ExcludeTargets) const;


   //==========================================================================
   //          Target to point
   //==========================================================================

   // Angle in degrees to point
   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Target")
      FRuntimeFloatCurve SearchAnglePoint;

   // Distance to point
   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Target")
      FRuntimeFloatCurve SearchDistancePoint;

   // Calculate target point value
   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Target")
      float GetTargetPoint (
         const URTargetComponent*          Target,
         const FVector                    &SearchOrigin,
         const FVector                    &SearchDirection,
         const TArray<AActor*>            &ExcludeActors,
         const TArray<URTargetComponent*> &ExcludeTargets) const;


   //==========================================================================
   //          Find Target
   //==========================================================================

   // Find by world location and forward direction
   UFUNCTION(BlueprintCallable, Category = "Rade|Target")
      virtual URTargetComponent* Find_Direction (
         const FVector                    &SearchOrigin,
         const FVector                    &SearchDirection,
         const TArray<AActor*>            &ExcludeActors,
         const TArray<URTargetComponent*> &ExcludeTargets);

   // Find using params from targeting component
   UFUNCTION(BlueprintCallable, Category = "Rade|Target")
      virtual URTargetComponent* Find_Target (
         const URPlayerTargetMgr*          Targeter,
         const TArray<AActor*>            &ExcludeActors,
         const TArray<URTargetComponent*> &ExcludeTargets);

   //==========================================================================
   //         Adjust target
   //==========================================================================

   // Dot value to point. [0-1]
   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Target")
      FRuntimeFloatCurve SearchScreenDotPoint;

   // Screen distance to point. Normized to screen size [0-1]
   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Target")
      FRuntimeFloatCurve SearchScreenDistancePoint;

   // Find target in by input direction from center of screen
   UFUNCTION(BlueprintCallable, Category = "Rade|Target")
      virtual URTargetComponent* Find_Screen (
         const URPlayerTargetMgr*          Targeter,
         FVector2D                         InputVector,
         const TArray<AActor*>            &ExcludeActors,
         const TArray<URTargetComponent*> &ExcludeTargets);

   //==========================================================================
   //                  Get instance -> GameState component
   //==========================================================================

   UFUNCTION(BlueprintCallable, BlueprintPure,
             Category = "Rade|Target",
             meta = (HidePin          = "WorldContextObject",
                     DefaultToSelf    = "WorldContextObject",
                     DisplayName      = "World Target Mgr",
                     CompactNodeTitle = "World Target Mgr"))
      static URWorldTargetMgr* GetInstance (const UObject* WorldContextObject);
};

