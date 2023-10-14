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
   bool IsEmpty () const noexcept;

   // All values are higher than
   bool MoreThan (const FRCoreStats &stat) const noexcept;

   bool         operator == (const FRCoreStats &CoreStat) const noexcept;
   bool         operator != (const FRCoreStats &CoreStat) const noexcept;
   FRCoreStats  operator +  (const FRCoreStats &CoreStat) const noexcept;
   FRCoreStats  operator -  (const FRCoreStats &CoreStat) const noexcept;
   FRCoreStats& operator += (const FRCoreStats &CoreStat)       noexcept;

   friend FArchive& operator << (FArchive& Ar, FRCoreStats &CoreStat) {
      Ar << CoreStat.STR;
      Ar << CoreStat.AGI;
      Ar << CoreStat.INT;
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

   // Are any values set
   bool IsEmpty () const noexcept;

   bool        operator == (const FRSubStats &SubStat) const noexcept;
   bool        operator != (const FRSubStats &SubStat) const noexcept;
   FRSubStats  operator +  (const FRSubStats &SubStat) const noexcept;
   FRSubStats  operator -  (const FRSubStats &SubStat) const noexcept;
   FRSubStats& operator += (const FRSubStats &SubStat)       noexcept;

   friend FArchive& operator << (FArchive& Ar, FRSubStats &SubStat) {
      Ar << SubStat.Evasion;
      Ar << SubStat.Critical;
      Ar << SubStat.MoveSpeed;
      Ar << SubStat.AttackSpeed;
      Ar << SubStat.AttackPower;
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
      static bool FRCoreStats_IsEmpty (const FRCoreStats &CoreStat);

   UFUNCTION(BlueprintPure, Category = "Rade|Status",
             meta=(DisplayName="Equal (FRCoreStats)", CompactNodeTitle="=="))
	   static bool FRCoreStats_EqualEqual (const FRCoreStats& A,
                                          const FRCoreStats& B);

   UFUNCTION(BlueprintPure, Category = "Rade|Status",
             meta=(DisplayName="NotEqual (FRCoreStats)", CompactNodeTitle="!="))
	   static bool FRCoreStats_NotEqual (const FRCoreStats& A,
                                        const FRCoreStats& B);


   UFUNCTION(BlueprintPure, Category = "Rade|Status",
             meta=(DisplayName="IsEmpty (FRSubStats)", CompactNodeTitle="IsEmpty"))
      static bool FRSubStats_IsEmpty (const FRSubStats &SubStat);

      UFUNCTION(BlueprintPure, Category = "Rade|Status",
             meta=(DisplayName="Equal (FRSubStats)", CompactNodeTitle="=="))
	   static bool FRSubStats_EqualEqual (const FRSubStats& A,
                                         const FRSubStats& B);

   UFUNCTION(BlueprintPure, Category = "Rade|Status",
             meta=(DisplayName="NotEqual (FRCoreStats)", CompactNodeTitle="!="))
	   static bool FRSubStats_NotEqual (const FRSubStats& A,
                                       const FRSubStats& B);


   UFUNCTION(BlueprintPure, Category = "Rade|Status",
             meta=(DisplayName="ToString (FRStatusValue)", CompactNodeTitle="ToString"))
      static FString FRStatusValue_ToString (const FRStatusValue &StatusValue);
};

