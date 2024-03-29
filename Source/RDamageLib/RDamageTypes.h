// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "GameFramework/DamageType.h"
#include "RUILib/RUIDescription.h"
#include "RDamageTypes.generated.h"

struct FDamageEvent;
class AController;
class AActor;

// ============================================================================
//                   Resistance
// ============================================================================

USTRUCT(Blueprintable, BlueprintType)
struct RDAMAGELIB_API FRDamageResistance
{
   GENERATED_BODY()

   UPROPERTY(EditAnywhere, BlueprintReadOnly)
      TSoftClassPtr<URDamageType> DamageType;

   UPROPERTY(EditAnywhere, BlueprintReadOnly)
      float Flat = 0;

   UPROPERTY(EditAnywhere, BlueprintReadOnly)
      float Percent = 0;
};

USTRUCT(Blueprintable, BlueprintType)
struct RDAMAGELIB_API FRDamageResistanceWithTag
{
   GENERATED_BODY()

   // Who or What is applying Resistance
   UPROPERTY(EditAnywhere, BlueprintReadOnly)
      FString Tag;

   // What value is added
   UPROPERTY(EditAnywhere, BlueprintReadOnly)
      FRDamageResistance Value;
};


// ============================================================================
//                   RDamageType
// ============================================================================

UCLASS(Abstract, Blueprintable, BlueprintType, ClassGroup=(_Rade))
class RDAMAGELIB_API URDamageType : public UDamageType,
                                    public IRGetDescriptionInterface
{
   GENERATED_BODY()
public:

   URDamageType ();

   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Status")
      FRUIDescription Description;

   virtual FRUIDescription GetDescription_Implementation () const override;

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

