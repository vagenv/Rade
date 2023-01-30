// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "GameFramework/DamageType.h"
#include "RDamageType.generated.h"

USTRUCT(BlueprintType)
struct RCHARACTERLIB_API FRResistanceStat
{
   GENERATED_BODY()

   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      TSubclassOf<URDamageType> DamageType;

   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      float Resistance = 0;
};

UCLASS(Blueprintable, BlueprintType)
class RCHARACTERLIB_API URDamageType : public UDamageType
{
   GENERATED_BODY()
public:

   URDamageType();

   UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Rade|Character")
   FRuntimeFloatCurve ResistanceToDamage;

   UFUNCTION(BlueprintCallable, Category = "Rade|Character")
   virtual float CalcDamage (float Damage, float Resistance);
};

UCLASS(Blueprintable, BlueprintType)
class RCHARACTERLIB_API URDamageType_Fall : public URDamageType
{
   GENERATED_BODY()
public:
   URDamageType_Fall();

   UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Rade|Character")
      FRuntimeFloatCurve FallDamageCurve;

   virtual float CalcDamage (float Velocity, float Resistance) override;
};

