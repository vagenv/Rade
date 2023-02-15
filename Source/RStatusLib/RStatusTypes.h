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

   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      float STR = 0;

   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      float AGI = 0;

   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      float INT = 0;

   // Are any values set
   bool Empty () const;

   // All values are higher than
   inline bool MoreThan (const FRCoreStats &stat) const;

   inline FRCoreStats operator + (const FRCoreStats &stat) const;
   inline FRCoreStats operator - (const FRCoreStats &stat) const;
};

// ============================================================================
//                   Extra Stat
// ============================================================================

USTRUCT(BlueprintType)
struct RSTATUSLIB_API FRExtraStats
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

   inline FRExtraStats operator + (const FRExtraStats &stat) const;
   inline FRExtraStats operator - (const FRExtraStats &stat) const;
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

   inline void Tick (float DeltaTime);

   friend FArchive& operator<< (FArchive& Ar, FRStatusValue &Value);
};

UCLASS()
class RSTATUSLIB_API URStatusUtilLibrary : public UBlueprintFunctionLibrary
{
   GENERATED_BODY()
public:

   // UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Status",
   //    meta = (DisplayName = "<", CompactNodeTitle = "<"))
   //    static bool FRCoreStats_Less (const FRCoreStats &a, const FRCoreStats &b) {
   //       return (a < b);
   //    }
};

