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
private:
   UPROPERTY ()
      TMap<UClass *, FREnemyExp> MapEnemyExp;
public:
   // List of Enemies and experience for attacking / killing them
   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Experience",
            meta=(RequiredAssetDataTags = "RowStructure=/Script/RExperienceLib.REnemyExp"))
      UDataTable* EnemyExpTable = nullptr;

public:
   // Provides experience required for level
   UFUNCTION(BlueprintPure, Category = "Rade|Experience")
      virtual int64 LevelToExperience (int Level) const;

   // Provides level at a experience point number
   UFUNCTION(BlueprintPure, Category = "Rade|Experience")
      virtual int ExperienceToLevel (int64 ExpPoints) const;

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
   //           Get instance -> GameState component
   //==========================================================================
public:
   UFUNCTION(BlueprintCallable, BlueprintPure,
             Category = "Rade|Experience",
             meta = (HidePin          = "WorldContextObject",
                     DefaultToSelf    = "WorldContextObject",
                     DisplayName      = "Get EXP Mgr",
                     CompactNodeTitle = "EXP Mgr"))
      static URWorldExperienceMgr* GetInstance (UObject* WorldContextObject);
};

