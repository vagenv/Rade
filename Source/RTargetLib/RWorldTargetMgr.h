// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "Components/ActorComponent.h"
#include "RWorldTargetMgr.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE (FRTargetableMgrEvent);

class URTargetComponent;
class URTargetingComponent;

UCLASS(Blueprintable, BlueprintType, ClassGroup=(_Rade), meta=(BlueprintSpawnableComponent))
class RTARGETLIB_API URWorldTargetMgr : public UActorComponent
{
   GENERATED_BODY()
public:

   URWorldTargetMgr ();

   //==========================================================================
   //                  Params
   //==========================================================================
protected:
   // Container for current targets
   UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rade|Targetable")
      TArray<URTargetComponent*> TargetableList;
public:
   // Called when target list has been modified
   UPROPERTY(BlueprintAssignable, Category = "Rade|Targetable")
      FRTargetableMgrEvent OnListUpdated;

   // Distance at which target can be searched
   UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Rade|Targetable")
      float SearchDistance = 3000;

   // FOV angle to search targets
   UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Rade|Targetable")
      float SearchAngle = 50;

   UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Rade|Targetable")
      float InputHWeight = 1;

   UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Rade|Targetable")
      float InputVWeight = 0.5f;

   //==========================================================================
   //          Functions called by targetable components
   //==========================================================================

   UFUNCTION(BlueprintCallable, Category = "Rade|Targetable")
      virtual void AddTarget (URTargetComponent* Target);

   UFUNCTION(BlueprintCallable, Category = "Rade|Targetable")
      virtual void RmTarget  (URTargetComponent* Target);

   //==========================================================================
   //          Functions called by targeting components
   //==========================================================================

   UFUNCTION(BlueprintCallable, Category = "Rade|Targetable")
      virtual URTargetComponent* Find (URTargetingComponent*      Targeter,
                                       TArray<AActor*>            FilterOutActors,
                                       TArray<URTargetComponent*> FilterOutTargets);

   UFUNCTION(BlueprintCallable, Category = "Rade|Targetable")
      virtual URTargetComponent* FindNear (URTargetingComponent*      Targeter,
                                           URTargetComponent*         CurrentTarget,
                                           float                      InputOffsetX,
                                           float                      InputOffsetY,
                                           TArray<AActor*>            FilterOutActors,
                                           TArray<URTargetComponent*> FilterOutTargets);

   //==========================================================================
   //                  Get instance -> GameState component
   //==========================================================================

   UFUNCTION(BlueprintCallable, BlueprintPure,
             Category = "Rade|Targetable",
             meta = (HidePin          = "WorldContextObject",
                     DefaultToSelf    = "WorldContextObject",
                     DisplayName      = "World Target Mgr",
                     CompactNodeTitle = "World Target Mgr"))
      static URWorldTargetMgr* GetInstance (const UObject* WorldContextObject);
};

