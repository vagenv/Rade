// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "Components/ActorComponent.h"
#include "RSaveLib/RSaveInterface.h"
#include "RAbilityMgrComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE (FRAbilityMgrEvent);

class URAbility;

// Ability Manager Component.
UCLASS(Blueprintable, BlueprintType, ClassGroup=(_Rade), meta=(BlueprintSpawnableComponent))
class RABILITYLIB_API URAbilityMgrComponent : public UActorComponent, public IRSaveInterface
{
   GENERATED_BODY()
public:


   URAbilityMgrComponent ();
   virtual void GetLifetimeReplicatedProps (TArray<FLifetimeProperty> &OutLifetimeProps) const override;
   virtual void BeginPlay () override;
   virtual void EndPlay (const EEndPlayReason::Type EndPlayReason) override;

   //==========================================================================
   //                 Level Up
   //==========================================================================
protected:
   UFUNCTION()
      void LeveledUp ();

   //==========================================================================
   //                 Ability Points
   //==========================================================================
protected:
   // --- Core Stats
   UPROPERTY(ReplicatedUsing = "OnRep_Points", Replicated)
      int AbilityPoints = 0;

   UFUNCTION()
      void OnRep_Points ();
public:
   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Status")
      int GetAbilityPoint () const;

   //==========================================================================
   //                 Ability Functions
   //==========================================================================

   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Ability")
      bool CanAddAbility (const FRAbilityInfo& AbilityInfo) const;

   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Ability")
      URAbility* GetAbility (const FRAbilityInfo& AbilityInfo) const;


   UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Rade|Ability")
      bool AddAbility (const FRAbilityInfo& AbilityInfo);

   UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Rade|Ability")
      bool RmAbility  (const FRAbilityInfo& AbilityInfo);

   //==========================================================================
   //                 Server versions of the functions
   //==========================================================================

   UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Rade|Ability")
      void AddAbility_Server                (const FRAbilityInfo& AbilityInfo);
      void AddAbility_Server_Implementation (const FRAbilityInfo& AbilityInfo);

   UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Rade|Ability")
      void RmAbility_Server                (const FRAbilityInfo& AbilityInfo);
      void RmAbility_Server_Implementation (const FRAbilityInfo& AbilityInfo);

   //==========================================================================
   //                 Events
   //==========================================================================

   UFUNCTION()
      void ReportAbilityListUpdated ();

   // When Ability list updated
   UPROPERTY(BlueprintAssignable, Category = "Rade|Ability")
      FRAbilityMgrEvent OnAbilityListUpdated;

   UFUNCTION()
      void ReportAbilityPointUpdated ();

   UPROPERTY(BlueprintAssignable, Category = "Rade|Ability")
      FRAbilityMgrEvent OnAbilityPointUpdated;

   //==========================================================================
   //                 Save / Load
   //==========================================================================
public:
   // Status Saved / Loaded between sessions.
   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Save")
      bool bSaveLoad = false;

   // Should be called during BeginPlay
   UFUNCTION()
      void ConnectToSaveMgr ();

protected:
   virtual void OnSave (FBufferArchive &SaveData) override;
   virtual void OnLoad (FMemoryReader  &LoadData) override;
};

