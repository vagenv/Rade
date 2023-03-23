// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "Components/ActorComponent.h"
#include "RExperienceMgrComponent.generated.h"

class URWorldExperienceMgr;

DECLARE_DYNAMIC_MULTICAST_DELEGATE (FRExperienceEvent);

UCLASS(Blueprintable, BlueprintType, ClassGroup=(_Rade), meta=(BlueprintSpawnableComponent))
class REXPERIENCELIB_API URExperienceMgrComponent : public UActorComponent
{
   GENERATED_BODY()
public:

   virtual void BeginPlay () override;
   virtual void EndPlay (const EEndPlayReason::Type EndPlayReason) override;

   //==========================================================================
   //          Experience
   //==========================================================================
protected:
   UPROPERTY()
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
   UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Rade|Experience")
      virtual void CheckLevel ();

   UFUNCTION(BlueprintPure, Category = "Rade|Experience")
      virtual int GetCurrentLevel () const;

   //==========================================================================
   //          Events
   //==========================================================================

   UPROPERTY(BlueprintAssignable, Category = "Rade|Experience")
      FRExperienceEvent OnExperienceChange;

   UPROPERTY(BlueprintAssignable, Category = "Rade|Experience")
      FRExperienceEvent OnLevelUp;

protected:
   UFUNCTION()
      void LeveledUp ();

   URWorldExperienceMgr* GlobalMgr = nullptr;
};

