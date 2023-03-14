// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "Components/ActorComponent.h"
#include "RTargetableMgr.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE (FRTargetableMgrEvent);

class URTargetableComponent;
class URTargetingComponent;

UCLASS(Blueprintable, BlueprintType, ClassGroup=(_Rade), meta=(BlueprintSpawnableComponent))
class RTARGETABLELIB_API URTargetableMgr : public UActorComponent
{
   GENERATED_BODY()
public:

   URTargetableMgr ();

   //==========================================================================
   //                  Params
   //==========================================================================
protected:
   // Container for current targets
   UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rade|Targetable")
      TArray<URTargetableComponent*> TargetableList;
public:
   // Called when target list has been modified
   UPROPERTY(BlueprintAssignable, Category = "Rade|Targetable")
      FRTargetableMgrEvent OnListUpdated;

   // Distance at which target can be searched
   UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Rade|Targetable")
      float SearchDistance = 2000;

   // FOV angle to search targets
   UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Rade|Targetable")
      float SearchAngle = 40;

   UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Rade|Targetable")
      float InputHWeight = 1;

   UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Rade|Targetable")
      float InputVWeight = 0.5f;

   //==========================================================================
   //          Functions called by targetable components
   //==========================================================================

   UFUNCTION(BlueprintCallable, Category = "Rade|Targetable")
      virtual void AddTarget (URTargetableComponent * Target);

   UFUNCTION(BlueprintCallable, Category = "Rade|Targetable")
      virtual void RmTarget  (URTargetableComponent * Target);

   //==========================================================================
   //          Functions called by targeting components
   //==========================================================================

   UFUNCTION(BlueprintCallable, Category = "Rade|Targetable")
      virtual URTargetableComponent* Find (URTargetingComponent*          Targeter,
                                           TArray<AActor*>                FilterOutActors,
                                           TArray<URTargetableComponent*> FilterOutTargets);

   UFUNCTION(BlueprintCallable, Category = "Rade|Targetable")
      virtual URTargetableComponent* FindNear (URTargetingComponent*          Targeter,
                                               URTargetableComponent*         CurrentTarget,
                                               float                          InputOffsetX,
                                               float                          InputOffsetY,
                                               TArray<AActor*>                FilterOutActors,
                                               TArray<URTargetableComponent*> FilterOutTargets);


   // Gets angle in degrees between two vectors
   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Targetable")
      static float GetAngle (FVector v1, FVector v2);

   //==========================================================================
   //                  Get instamce -> GameState component
   //==========================================================================

   UFUNCTION(BlueprintCallable, Category = "Rade|Targetable")
      static URTargetableMgr* GetInstance (UObject* WorldContextObject);
};

