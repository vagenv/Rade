// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "Components/ActorComponent.h"
#include "RMapTypes.h"
#include "RWorldMapMgr.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE (FRMapMgrEvent);

class URMapPointComponent;

UCLASS(Blueprintable, BlueprintType, ClassGroup=(_Rade), meta=(BlueprintSpawnableComponent))
class RMAPLIB_API URWorldMapMgr : public UActorComponent
{
   GENERATED_BODY()
public:

   URWorldMapMgr();

   // Read table before begin play
   virtual void InitializeComponent () override;

   //==========================================================================
   //             Map Point table
   //==========================================================================
private:
   UPROPERTY ()
      TMap<FString, FRMapPointInfo> MapOfMapPoints;
protected:
   // List of abilties
   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Map",
            meta=(RequiredAssetDataTags = "RowStructure=/Script/RMapLib.RMapPointInfo"))
      TObjectPtr<const UDataTable> MapPointTable = nullptr;
public:

   // Get map point info
   UFUNCTION (BlueprintCallable, BlueprintPure, Category = "Rade|Ability")
      FRMapPointInfo GetMapPointInfo_Actor (const AActor* TargetActor) const;

   // Get map point info
   UFUNCTION (BlueprintCallable, BlueprintPure, Category = "Rade|Map")
      FRMapPointInfo GetMapPointInfo_Class (const TSoftClassPtr<AActor> ActorClass) const;

   //==========================================================================
   //                  Map point list
   //==========================================================================
private:
   // Container all map actor
   UPROPERTY()
      TArray<TWeakObjectPtr<URMapPointComponent> > MapPointList;
protected:
   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Map")
      TArray<URMapPointComponent*> GetMapPointList () const;

   UFUNCTION()
      virtual void ReportListUpdateDelayed ();

   bool ReportListUpdatedDelayedTriggered = false;

public:
   // Called when map list has been modified
   UPROPERTY(BlueprintAssignable, Category = "Rade|Map")
      FRMapMgrEvent OnListUpdated;

   //==========================================================================
   //          Functions called by Map components
   //==========================================================================

   UFUNCTION(BlueprintCallable, Category = "Rade|Map")
      virtual void RegisterMapPoint  (URMapPointComponent* MapPoint);

   UFUNCTION(BlueprintCallable, Category = "Rade|Map")
      virtual void UnregisterMapPoint (URMapPointComponent* MapPoint);

   //==========================================================================
   //                  Get instance -> GameState component
   //==========================================================================

   UFUNCTION(BlueprintCallable, BlueprintPure,
             Category = "Rade|Map",
             meta = (HidePin          = "WorldContextObject",
                     DefaultToSelf    = "WorldContextObject",
                     DisplayName      = "World Map Mgr",
                     CompactNodeTitle = "World Map Mgr"))
      static URWorldMapMgr* GetInstance (const UObject* WorldContextObject);
};

