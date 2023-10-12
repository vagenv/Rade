// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "Components/ActorComponent.h"
#include "RSaveLib/RSaveInterface.h"
#include "RExperienceMgrComponent.generated.h"

class URWorldExperienceMgr;

DECLARE_DYNAMIC_MULTICAST_DELEGATE (FRExperienceEvent);

UCLASS(Blueprintable, BlueprintType, ClassGroup=(_Rade), meta=(BlueprintSpawnableComponent))
class REXPERIENCELIB_API URExperienceMgrComponent : public UActorComponent, public IRSaveInterface
{
   GENERATED_BODY()
public:

   URExperienceMgrComponent ();
   virtual void GetLifetimeReplicatedProps (TArray<FLifetimeProperty> &OutLifetimeProps) const override;
   virtual void BeginPlay () override;
   virtual void EndPlay (const EEndPlayReason::Type EndPlayReason) override;

   //==========================================================================
   //          Experience
   //==========================================================================
protected:
   // Network call when Item list has been updated
   UFUNCTION()
      virtual void OnRep_Exp ();

   UPROPERTY(ReplicatedUsing = "OnRep_Exp", Replicated)
      int64 ExperiencePoints = 0;
public:

   UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Rade|Experience")
      virtual void AddExperiencePoints (int64 ExpPoint);

   UFUNCTION(BlueprintPure, Category = "Rade|Experience")
      virtual int64 GetExperiencePoints () const;

   //==========================================================================
   //          Level
   //==========================================================================
protected:
   UPROPERTY()
      int CurrentLevel = 0;
public:
   UFUNCTION(BlueprintCallable, Category = "Rade|Experience")
      virtual void CheckLevel ();

   UFUNCTION(BlueprintPure, Category = "Rade|Experience")
      virtual int GetCurrentLevel () const;

   //==========================================================================
   //          Events
   //==========================================================================

   UFUNCTION()
      void ReportExperienceChange ();

   UPROPERTY(BlueprintAssignable, Category = "Rade|Experience")
      FRExperienceEvent OnExperienceChange;

   UFUNCTION()
      void ReportLevelUp ();

   UPROPERTY(BlueprintAssignable, Category = "Rade|Experience")
      FRExperienceEvent OnLevelUp;

protected:
   UFUNCTION()
      void LeveledUp ();

   UPROPERTY()
      TObjectPtr<URWorldExperienceMgr> WorldExpMgr = nullptr;


   //==========================================================================
   //         Save / Load
   //==========================================================================

   // Status Saved / Loaded between sessions.
   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Experience")
      bool bSaveLoad = false;

protected:
   virtual void OnSave (FBufferArchive &SaveData) override;
   virtual void OnLoad (FMemoryReader &LoadData) override;
};

