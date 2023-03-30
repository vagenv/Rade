// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "RStatusMgrComponent.h"
#include "RSaveLib/RSaveInterface.h"
#include "RPlayerStatusMgrComponent.generated.h"

class URExperienceMgrComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE (FRPlayerStatusMgrEvent);

// Status Manager Component.
UCLASS(Blueprintable, BlueprintType, ClassGroup=(_Rade), meta=(BlueprintSpawnableComponent))
class RSTATUSLIB_API URPlayerStatusMgrComponent : public URStatusMgrComponent, public IRSaveInterface
{
   GENERATED_BODY()
public:

   URPlayerStatusMgrComponent ();
   virtual void GetLifetimeReplicatedProps (TArray<FLifetimeProperty> &OutLifetimeProps) const override;
   virtual void BeginPlay () override;

   //==========================================================================
   //                 Stored pointers
   //==========================================================================
protected:

   // Experience
   UPROPERTY()
      URExperienceMgrComponent* ExperienceMgr = nullptr;

   //==========================================================================
   //                 Level Up
   //==========================================================================
protected:
   UFUNCTION()
      void LeveledUp ();

   //==========================================================================
   //                 Recalc status
   //==========================================================================
protected:
   virtual void RecalcStatus       () override;
   virtual void RecalcStatusValues () override;
   virtual void RecalcCoreStats    ();
   virtual void RecalcSubStats     ();

   //==========================================================================
   //                 Extra stat Points
   //==========================================================================
protected:
   // To be assigned to core stats
   UPROPERTY(ReplicatedUsing = "OnRep_Stats", Replicated)
      float CoreStats_Extra = 0;
public:

   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Status")
      float GetCoreStats_Extra () const;

   UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Rade|Status")
      bool AddExtraStat (FRCoreStats ExtraStat);

   UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Rade|Status")
      void AddExtraStat_Server                (FRCoreStats ExtraStat);
      void AddExtraStat_Server_Implementation (FRCoreStats ExtraStat);

   //==========================================================================
   //                 Core and Sub Stats
   //==========================================================================
protected:

   // --- Core Stats
   UPROPERTY(ReplicatedUsing = "OnRep_Stats", Replicated)
      FRCoreStats CoreStats_Base;

   UPROPERTY(ReplicatedUsing = "OnRep_Stats", Replicated)
      FRCoreStats CoreStats_Added;

   // --- Extra Stats
   UPROPERTY(ReplicatedUsing = "OnRep_Stats", Replicated)
      FRSubStats SubStats_Base;

   UPROPERTY(ReplicatedUsing = "OnRep_Stats", Replicated)
      FRSubStats SubStats_Added;

   UFUNCTION()
      void OnRep_Stats ();
public:
   // --- Get Core Stats
   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Status")
      FRCoreStats GetCoreStats_Base () const;

   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Status")
      FRCoreStats GetCoreStats_Added () const;

   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Status")
      FRCoreStats GetCoreStats_Total () const;

   // --- Get Extra Stats
   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Status")
      FRSubStats GetSubStats_Base () const;

   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Status")
      FRSubStats GetSubStats_Added () const;

   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Status")
      FRSubStats GetSubStats_Total () const;

   // --- Stats functions
   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Status")
      bool HasStats (const FRCoreStats &RequiredStats) const;

   //==========================================================================
   //                 Events
   //==========================================================================

   // Delegate when status updated
   UPROPERTY(BlueprintAssignable, Category = "Rade|Status")
      FRPlayerStatusMgrEvent OnStatsUpdated;

   //==========================================================================
   //                 Save/Load
   //==========================================================================
public:
   // Status Saved / Loaded between sessions.
   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Status")
      bool bSaveLoad = false;

protected:
   virtual void OnSave (FBufferArchive &SaveData) override;
   virtual void OnLoad (FMemoryReader &LoadData) override;
};

