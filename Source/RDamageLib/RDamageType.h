// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "GameFramework/DamageType.h"
#include "RUtilLib/RUIDescription.h"
#include "RDamageType.generated.h"

struct FDamageEvent;
class AController;
class AActor;

// ============================================================================
//                   Resistance
// ============================================================================

USTRUCT(BlueprintType)
struct RDAMAGELIB_API FRDamageResistance
{
   GENERATED_BODY()

   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
      TSubclassOf<URDamageType> DamageType;

   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
      float Flat = 0;

   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
      float Percent = 0;
};

USTRUCT(BlueprintType)
struct RDAMAGELIB_API FRDamageResistanceWithTag
{
   GENERATED_BODY()

   // Who or What is applying Resistance
   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
      FString Tag;

   // What value is added
   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
      FRDamageResistance Value;
};


// ============================================================================
//                   RDamageType
// ============================================================================

UCLASS(Abstract, Blueprintable, BlueprintType, ClassGroup=(_Rade))
class RDAMAGELIB_API URDamageType : public UDamageType
{
   GENERATED_BODY()
public:

   URDamageType ();

   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Status")
      FRUIDescription Description;

   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Status")
      bool Evadeable = true;

   UFUNCTION(BlueprintCallable, Category = "Rade|Status")
      virtual float CalcDamage (float Damage, const FRDamageResistance& Resistance) const;

   UFUNCTION(BlueprintImplementableEvent, Category = "Rade|Status")
      void BP_AnyDamage (AActor*            DamageVictim,
                         FRDamageResistance Resistance,
                         float              DamageAmount,
                         AActor*            DamageCauser) const;
};

UCLASS(ClassGroup=(_Rade))
class RDAMAGELIB_API URDamageUtilLibrary : public UBlueprintFunctionLibrary
{
   GENERATED_BODY()
public:

   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Status")
      static TArray<FRDamageResistance> MergeResistance (const TArray<FRDamageResistanceWithTag>& Resistance);

};

