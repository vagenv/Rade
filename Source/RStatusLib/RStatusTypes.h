// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "RStatusTypes.generated.h"

// ============================================================================
//                   Core Stat
// ============================================================================

USTRUCT(Blueprintable, BlueprintType)
struct RSTATUSLIB_API FRCoreStats
{
   GENERATED_BODY()

   FRCoreStats ();
   FRCoreStats (float Value);

   UPROPERTY(EditAnywhere, BlueprintReadOnly)
      float STR = 0;

   UPROPERTY(EditAnywhere, BlueprintReadOnly)
      float AGI = 0;

   UPROPERTY(EditAnywhere, BlueprintReadOnly)
      float INT = 0;

   // Are any values set
   bool Empty () const;

   // All values are higher than
   bool MoreThan (const FRCoreStats &stat) const;

   FRCoreStats  operator +  (const FRCoreStats &stat) const;
   FRCoreStats  operator -  (const FRCoreStats &stat) const;
   FRCoreStats& operator += (const FRCoreStats &stat);

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

USTRUCT(Blueprintable, BlueprintType)
struct RSTATUSLIB_API FRSubStats
{
   GENERATED_BODY()

   UPROPERTY (EditAnywhere, BlueprintReadOnly)
      float Evasion = 0;

   UPROPERTY (EditAnywhere, BlueprintReadOnly)
      float Critical = 0;

   UPROPERTY (EditAnywhere, BlueprintReadOnly)
      float MoveSpeed = 0;

   UPROPERTY (EditAnywhere, BlueprintReadOnly)
      float AttackSpeed = 0;

   UPROPERTY (EditAnywhere, BlueprintReadOnly)
      float AttackPower = 0;

   FRSubStats  operator +  (const FRSubStats &stat) const;
   FRSubStats  operator -  (const FRSubStats &stat) const;
   FRSubStats& operator += (const FRSubStats &stat);

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

USTRUCT(Blueprintable, BlueprintType)
struct RSTATUSLIB_API FRStatusValue
{
   GENERATED_BODY()

   UPROPERTY(EditAnywhere, BlueprintReadOnly)
      float Current = 50;

   UPROPERTY(EditAnywhere, BlueprintReadOnly)
      float Max = 100;

   // Per second
   UPROPERTY(EditAnywhere, BlueprintReadOnly)
      float Regen = 1;

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

   UFUNCTION(BlueprintPure, Category = "Rade|Status",
             meta=(DisplayName="IsEmpty (FRCoreStats)", CompactNodeTitle="IsEmpty"))
      static bool FRCoreStats_IsEmpty (const FRCoreStats &Value);

   UFUNCTION(BlueprintPure, Category = "Rade|Status",
             meta=(DisplayName="IsEmpty (FRSubStats)", CompactNodeTitle="IsEmpty"))
      static bool FRSubStats_IsEmpty (const FRSubStats &Value);

   UFUNCTION(BlueprintPure, Category = "Rade|Status",
             meta=(DisplayName="ToString (FRStatusValue)", CompactNodeTitle="ToString"))
      static FString FRStatusValue_ToString (const FRStatusValue &Value);
};

