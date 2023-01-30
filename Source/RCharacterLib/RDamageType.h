// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "GameFramework/DamageType.h"
#include "RDamageType.generated.h"

UENUM(BlueprintType)
enum class ERDamageType : uint8
{
   BLUNT     UMETA (DisplayName = "BLUNT"    ),
   PIERCE    UMETA (DisplayName = "PIERCE"   ),
   SLASH     UMETA (DisplayName = "SLASH"    ),

   MAGIC     UMETA (DisplayName = "MAGIC"    ),
   FIRE      UMETA (DisplayName = "FIRE"     ),
   LIGHTNING UMETA (DisplayName = "LIGHTNING"),

   DARK      UMETA (DisplayName = "DARK"     ),
   LIGHT     UMETA (DisplayName = "LIGHT"    ),
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
