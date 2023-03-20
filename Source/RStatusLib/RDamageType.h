// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "GameFramework/DamageType.h"
#include "RUtilLib/RUIDescription.h"
#include "RDamageType.generated.h"

struct FDamageEvent;
class AController;
class AActor;

// ============================================================================
//                   RDamageType
// ============================================================================

UCLASS(Blueprintable, BlueprintType, ClassGroup=(_Rade))
class RSTATUSLIB_API URDamageType : public UDamageType
{
   GENERATED_BODY()
public:

   URDamageType ();

   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Status")
      FRUIDescription Description;

   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Status")
      bool Evadeable = true;

   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Status")
      FRuntimeFloatCurve ResistanceToDamage;

   UFUNCTION(BlueprintCallable, Category = "Rade|Status")
      virtual float CalcDamage (float Damage, float Resistance) const;

   UFUNCTION(BlueprintImplementableEvent, Category = "Rade|Status")
      void BP_AnyDamage (AActor* DamageVictim,
                         float Resistance,
                         float DamageAmount,
                         AActor* DamageCauser) const;
};

