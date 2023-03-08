// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "Components/ActorComponent.h"
#include "RTargetableMgr.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE (FRTargetableEvent);

class URTargetableComponent;

UCLASS(Blueprintable, BlueprintType, meta=(BlueprintSpawnableComponent) )
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
      FRTargetableEvent OnListUpdated;

   // Distance at which target can be searched
   UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Rade|Targetable")
      float SearchDistance = 2000;

   // FOV angle to search targets
   UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Rade|Targetable")
      float SearchAngle = 40;

   //==========================================================================
   //                  Functions
   //==========================================================================

   UFUNCTION(BlueprintCallable, Category = "Rade|Targetable")
      virtual void AddTarget (URTargetableComponent * Target);

   UFUNCTION(BlueprintCallable, Category = "Rade|Targetable")
      virtual void RmTarget  (URTargetableComponent * Target);

   UFUNCTION(BlueprintCallable, Category = "Rade|Targetable", meta = (AutoCreateRefTerm = "FilterOut"))
      virtual URTargetableComponent* Find (FVector Origin, FRotator Direction, TArray<AActor*> FilterOut);

   //==========================================================================
   //                  Get instamce -> GameMode component
   //==========================================================================

   UFUNCTION(BlueprintCallable, Category = "Rade|Targetable")
      static URTargetableMgr* GetInstance (UObject* WorldContextObject);
};

