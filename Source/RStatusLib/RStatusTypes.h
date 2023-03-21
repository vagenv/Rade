// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "RStatusTypes.generated.h"

// ============================================================================
//                   Core Stat
// ============================================================================

USTRUCT(BlueprintType)
struct RSTATUSLIB_API FRCoreStats
{
   GENERATED_BODY()

   FRCoreStats ();
   FRCoreStats (float Value);

   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
      float STR = 0;

   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
      float AGI = 0;

   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
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

   UPROPERTY (EditDefaultsOnly, BlueprintReadOnly)
      float Evasion = 0;

   UPROPERTY (EditDefaultsOnly, BlueprintReadOnly)
      float Critical = 0;

   UPROPERTY (EditDefaultsOnly, BlueprintReadOnly)
      float MoveSpeed = 0;

   UPROPERTY (EditDefaultsOnly, BlueprintReadOnly)
      float AttackSpeed = 0;

   UPROPERTY (EditDefaultsOnly, BlueprintReadOnly)
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

   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
      float Current = 50;

   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
      float Max = 100;

   // Per second
   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
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
//                   Util Library
// ============================================================================

UCLASS()
class RSTATUSLIB_API URStatusUtilLibrary : public UBlueprintFunctionLibrary
{
   GENERATED_BODY()
public:

};

