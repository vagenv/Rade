// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

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

   // Base events
   URAbilityMgrComponent ();
   virtual void GetLifetimeReplicatedProps (TArray<FLifetimeProperty> &OutLifetimeProps) const override;
   virtual void BeginPlay () override;
   virtual void EndPlay (const EEndPlayReason::Type EndPlayReason) override;
   virtual void TickComponent (float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

   //==========================================================================
   //                 Core
   //==========================================================================

   // Item to be added upon game start
   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Ability")
      TArray<TSubclassOf<URAbility> > DefaultAbilities;

   UFUNCTION(BlueprintCallable, Category = "Rade|Ability")
      URAbility* GetAbility (const TSubclassOf<URAbility> Ability);

   UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Rade|Ability")
      bool AddAbility (const TSubclassOf<URAbility> Ability);

   UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Rade|Ability")
      bool RMAbility  (const TSubclassOf<URAbility> Ability);

   // When Ability list updated
   UPROPERTY(BlueprintAssignable, Category = "Rade|Ability")
      FRAbilityMgrEvent OnAbilityListUpdated;

   //==========================================================================
   //                 Save/Load
   //==========================================================================

   // Status Saved / Loaded between sessions.
   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Ability")
      bool bSaveLoad = false;

protected:
   virtual void OnSave (FBufferArchive &SaveData) override;
   virtual void OnLoad (FMemoryReader &LoadData) override;
};

