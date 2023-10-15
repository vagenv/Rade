// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "RAbility_Aura.h"
#include "RStatusLib/RPassiveStatusEffect.h"
#include "RAbility_Aura_StatusEffect.generated.h"

class URActiveStatusEffect;
class URStatusMgrComponent;

//=============================================================================
//                 Passive Ability
//=============================================================================

UCLASS(Abstract, Blueprintable, BlueprintType, ClassGroup=(_Rade))
class RABILITYLIB_API URAbility_Aura_StatusEffect : public URAbility_Aura
{
   GENERATED_BODY()
public:

   virtual void BeginPlay () override;
   virtual void SetIsEnabled (bool Enabled) override;
   virtual void CheckRange () override;


   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Ability")
      TArray<FRPassiveStatusEffect> PassiveEffects;

   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Ability")
      TArray<TSoftClassPtr<URActiveStatusEffect> > ActiveEffects;

protected:

   UFUNCTION ()
      virtual void ApplyEffects ();
   UFUNCTION ()
      virtual void RemoveEffects ();

   UPROPERTY()
      FString UniqueEffectTag;

private:
   UPROPERTY()
      TArray<TWeakObjectPtr<URStatusMgrComponent> > LastAffectedMgr;
};

