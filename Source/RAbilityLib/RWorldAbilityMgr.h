// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "RAbilityTypes.h"
#include "RWorldAbilityMgr.generated.h"

class URAbility;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam (FRAddAbilityEvent,
                                             URAbility*, Ability);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam (FRRmAbilityEvent,
                                             URAbility*, Ability);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam (FRUseAbilityEvent,
                                             URAbility*, Ability);

UCLASS(Blueprintable, BlueprintType, ClassGroup=(_Rade), meta=(BlueprintSpawnableComponent))
class RABILITYLIB_API URWorldAbilityMgr : public UActorComponent
{
   GENERATED_BODY()
public:

   URWorldAbilityMgr ();
   // Read table before begin play
   virtual void InitializeComponent () override;
   virtual void BeginPlay () override;

   //==========================================================================
   //             Ability table
   //==========================================================================
private:
   UPROPERTY ()
      TMap<FTopLevelAssetPath, FRAbilityInfo> MapAbility;
protected:
   // List of abilties
   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Ability",
            meta=(RequiredAssetDataTags = "RowStructure=/Script/RAbilityLib.RAbilityInfo"))
      UDataTable* AbilityTable = nullptr;
public:

   // Get ability info and scaling.
   UFUNCTION (BlueprintCallable, BlueprintPure, Category = "Rade|Ability")
      FRAbilityInfo GetAbilityInfo (const URAbility* Ability) const;

   UFUNCTION (BlueprintCallable, BlueprintPure, Category = "Rade|Ability")
      TArray<FRAbilityInfo> GetAllAbilities () const;

   //==========================================================================
   //                  Events
   //==========================================================================

   // --- Add
   UFUNCTION()
      void ReportAddAbility (URAbility* Ability);

   UPROPERTY(BlueprintAssignable, Category = "Rade|Ability")
      FRAddAbilityEvent OnAddAbility;

   // --- Remove
   UFUNCTION()
      void ReportRmAbility (URAbility* Ability);

   UPROPERTY(BlueprintAssignable, Category = "Rade|Ability")
      FRRmAbilityEvent OnRmAbility;

   // --- Use
   UFUNCTION()
      void ReportUseAbility (URAbility* Ability);

   UPROPERTY(BlueprintAssignable, Category = "Rade|Ability")
      FRUseAbilityEvent OnUseAbility;

   //==========================================================================
   //                  Get instance -> GameState component
   //==========================================================================

   UFUNCTION(BlueprintCallable, BlueprintPure,
             Category = "Rade|Ability",
             meta = (HidePin          = "WorldContextObject",
                     DefaultToSelf    = "WorldContextObject",
                     DisplayName      = "World Ability Mgr",
                     CompactNodeTitle = "World Ability Mgr"))
      static URWorldAbilityMgr* GetInstance (const UObject* WorldContextObject);
};

