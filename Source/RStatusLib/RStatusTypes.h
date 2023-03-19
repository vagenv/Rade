// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "RStatusTypes.generated.h"

class URDamageType;

// ============================================================================
//                   Core Stat
// ============================================================================

USTRUCT(BlueprintType)
struct RSTATUSLIB_API FRCoreStats
{
   GENERATED_BODY()

   FRCoreStats ();
   FRCoreStats (float Value);

   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      float STR = 0;

   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      float AGI = 0;

   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      float INT = 0;

   // Are any values set
   bool Empty () const;

   // All values are higher than
   bool MoreThan (const FRCoreStats &stat) const;

   FRCoreStats operator + (const FRCoreStats &stat) const;
   FRCoreStats operator - (const FRCoreStats &stat) const;

   friend FArchive& operator << (FArchive& Ar, FRCoreStats &Data) {
      Ar << Data.STR;
      Ar << Data.AGI;
      Ar << Data.INT;
      return Ar;
   }
};

// ============================================================================
//                   Extra Stat
// ============================================================================

USTRUCT(BlueprintType)
struct RSTATUSLIB_API FRSubStats
{
   GENERATED_BODY()

   UPROPERTY (EditAnywhere, BlueprintReadWrite)
      float Evasion = 0;

   UPROPERTY (EditAnywhere, BlueprintReadWrite)
      float Critical = 0;

   UPROPERTY (EditAnywhere, BlueprintReadWrite)
      float MoveSpeed = 0;

   UPROPERTY (EditAnywhere, BlueprintReadWrite)
      float AttackSpeed = 0;

   UPROPERTY (EditAnywhere, BlueprintReadWrite)
      float AttackPower = 0;

   FRSubStats operator + (const FRSubStats &stat) const;
   FRSubStats operator - (const FRSubStats &stat) const;

   friend FArchive& operator << (FArchive& Ar, FRSubStats &Value) {
      Ar << Value.Evasion;
      Ar << Value.Critical;
      Ar << Value.MoveSpeed;
      Ar << Value.AttackSpeed;
      Ar << Value.AttackPower;
      return Ar;
   }
};

// ============================================================================
//                   Status Value (Health, Stamina, Mana)
// ============================================================================

USTRUCT(BlueprintType)
struct RSTATUSLIB_API FRStatusValue
{
   GENERATED_BODY()

   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      float Current = 50;

   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      float Max = 100;

   // Per second
   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      float Regen = 0;

   void Tick (float DeltaTime);

   friend FArchive& operator << (FArchive& Ar, FRStatusValue &Value) {
      Ar << Value.Current;
      Ar << Value.Max;
      Ar << Value.Regen;
      return Ar;
   }
};

// ============================================================================
//                   Resistance
// ============================================================================

USTRUCT(BlueprintType)
struct RSTATUSLIB_API FRResistanceStat
{
   GENERATED_BODY()

   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      TSubclassOf<URDamageType> DamageType;

   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      float Value = 0;
};

USTRUCT(BlueprintType)
struct RSTATUSLIB_API FRResistanceStatWithTag
{
   GENERATED_BODY()

   // Who or What is applying Resistance
   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      FString Tag;

   // What value is added
   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      FRResistanceStat Value;
};

// ============================================================================
//                   Util Library
// ============================================================================

UCLASS()
class RSTATUSLIB_API URStatusUtilLibrary : public UBlueprintFunctionLibrary
{
   GENERATED_BODY()
public:

};

