// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "RWorldExperienceMgr.generated.h"

class URDamageType;
class ACharacter;

USTRUCT(BlueprintType)
struct REXPERIENCELIB_API FREnemyExp : public FTableRowBase
{
   GENERATED_BODY()

   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
      TSubclassOf<ACharacter> Target;

   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
      float PerDamage = 1;

   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
      float PerDeath = 500;
};


UCLASS(Blueprintable, BlueprintType, ClassGroup=(_Rade), meta=(BlueprintSpawnableComponent))
class REXPERIENCELIB_API URWorldExperienceMgr : public UActorComponent
{
   GENERATED_BODY()
public:

   URWorldExperienceMgr ();

   virtual void BeginPlay () override;


   //==========================================================================
   //          Experience table
   //==========================================================================

   // How stacks are scaling passive effect
   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Experience",
            meta=(RequiredAssetDataTags = "RowStructure=/Script/RExperienceLib.REnemyExp"))
      UDataTable* EnemyExpTable = nullptr;

   //==========================================================================
   //          Subscribe to DamageMgr
   //==========================================================================
protected:
   UFUNCTION()
      void OnDamage (AActor*             Victim,
                     float               Amount,
                     const URDamageType* Type,
                     AActor*             Causer);
   UFUNCTION()
      void OnDeath (AActor*             Victim,
                    AActor*             Causer,
                    const URDamageType* Type);

   //==========================================================================
   //                  Get instance -> GameState component
   //==========================================================================
public:

   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Experience", meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", DisplayName = "Get Target Mgr", CompactNodeTitle = "Target Mgr"))
      static URWorldExperienceMgr* GetInstance (UObject* WorldContextObject);
};

