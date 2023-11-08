// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "Components/ActorComponent.h"
#include "RMapTypes.h"
#include "RMapPointComponent.generated.h"

UCLASS(Blueprintable, BlueprintType, ClassGroup=(_Rade), meta=(BlueprintSpawnableComponent))
class RMAPLIB_API URMapPointComponent : public UActorComponent
{
   GENERATED_BODY()
public:

   virtual void BeginPlay () override;
   virtual void EndPlay (const EEndPlayReason::Type EndPlayReason) override;

   // Wrapper around URWorldMapMgr::GetMapPointInfo_Actor
   UFUNCTION (BlueprintCallable, BlueprintPure, Category = "Rade|Map")
      FRMapPointInfo GetMapPointInfo () const;

protected:

   // Add to the List in WorldMapMgr
   UFUNCTION()
      void RegisterMapPoint ();

   // remove from the List in WorldMapMgr
   UFUNCTION()
      void UnregisterMapPoint ();

   //==========================================================================
   //         Can this Point be found
   //==========================================================================
public:

   UFUNCTION(BlueprintCallable, Category = "Rade|Map")
      void SetIsVisible (bool IsVisible_);

   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Map")
      bool GetIsVisible () const;

protected:

   // Can this map point be found
   UPROPERTY()
      bool IsVisible = true;
};

