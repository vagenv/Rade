// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "GameFramework/DamageType.h"
#include "RDamageType.generated.h"

struct FDamageEvent;
class AController;
class AActor;

USTRUCT(BlueprintType)
struct RSTATUSLIB_API FRResistanceStat
{
   GENERATED_BODY()

   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      TSubclassOf<URDamageType> DamageType;

   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      float Resistance = 0;
};

UCLASS(Blueprintable, BlueprintType)
class RSTATUSLIB_API URDamageType : public UDamageType
{
   GENERATED_BODY()
public:

   URDamageType();

   UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Rade|Character")
      FRuntimeFloatCurve ResistanceToDamage;

   UFUNCTION(BlueprintCallable, Category = "Rade|Character")
      virtual float CalcDamage (float Damage, float Resistance) const;

   UFUNCTION(BlueprintImplementableEvent, Category = "Rade|Character")
      void TakeDamage (AActor* DamageVictim,
                       float Resistance,
                       float DamageAmount,
                       FDamageEvent const& DamageEvent,
                       AController* EventInstigator,
                       AActor* DamageCauser) const;
};

UCLASS(Blueprintable, BlueprintType)
class RSTATUSLIB_API URDamageType_Fall : public URDamageType
{
   GENERATED_BODY()
public:
   URDamageType_Fall();

   UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Rade|Character")
      FRuntimeFloatCurve FallDamageCurve;

   virtual float CalcDamage (float Velocity, float Resistance) const override;
};
