// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "RStatusTypes.h"
#include "RWorldStatusMgr.generated.h"

class URActiveStatusEffect;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams (FRStatusEffectEvent,
                                                URActiveStatusEffect*, Effect,
                                                AActor*,               Causer,
                                                AActor*,               Victim);

UCLASS(Blueprintable, BlueprintType, ClassGroup=(_Rade), meta=(BlueprintSpawnableComponent))
class RSTATUSLIB_API URWorldStatusMgr : public UActorComponent
{
   GENERATED_BODY()
public:

   URWorldStatusMgr ();
   virtual void BeginPlay () override;

public:
   //==========================================================================
   //                Get stat growth functions
   //==========================================================================

   // Gets delta stats growth per level
   UFUNCTION (BlueprintPure, Category = "Rade|Status")
      FRCoreStats GetLevelUpStatGain (int CurrentLevel) const;

   //==========================================================================
   //                Stat growth Curves
   //==========================================================================
protected:
   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Status")
      FRuntimeFloatCurve LvlStrGainCurve;

   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Status")
      FRuntimeFloatCurve LvlAgiGainCurve;

   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Status")
      FRuntimeFloatCurve LvlIntGainCurve;

   //==========================================================================
   //                Get stat effect functions
   //==========================================================================
public:
   // --- STR
   UFUNCTION (BlueprintPure, Category = "Rade|Status")
      float GetStrToHealthMax    (float STR) const;
   UFUNCTION (BlueprintPure, Category = "Rade|Status")
      float GetStrToHealthRegen  (float STR) const;

   // --- AGI
   UFUNCTION (BlueprintPure, Category = "Rade|Status")
      float GetAgiToStaminaMax   (float AGI) const;
   UFUNCTION (BlueprintPure, Category = "Rade|Status")
      float GetAgiToStaminaRegen (float AGI) const;

   UFUNCTION (BlueprintPure, Category = "Rade|Status")
      float GetAgiToEvasion      (float AGI) const;
   UFUNCTION (BlueprintPure, Category = "Rade|Status")
      float GetAgiToCritical     (float AGI) const;
   UFUNCTION (BlueprintPure, Category = "Rade|Status")
      float GetAgiToAttackSpeed  (float AGI) const;

   // --- INT
   UFUNCTION (BlueprintPure, Category = "Rade|Status")
      float GetIntToManaMax      (float INT) const;
   UFUNCTION (BlueprintPure, Category = "Rade|Status")
      float GetIntToManaRegen    (float INT) const;

   //==========================================================================
   //                 Status Value Curves
   //==========================================================================
protected:
   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Status")
      FRuntimeFloatCurve StrToHealthMaxCurve;

   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Status")
      FRuntimeFloatCurve StrToHealthRegenCurve;

   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Status")
      FRuntimeFloatCurve AgiToStaminaMaxCurve;

   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Status")
      FRuntimeFloatCurve AgiToStaminaRegenCurve;

   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Status")
      FRuntimeFloatCurve IntToManaMaxCurve;

   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Status")
      FRuntimeFloatCurve IntToManaRegenCurve;

   //==========================================================================
   //                 Extra Stat Curves
   //==========================================================================
protected:
   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Status")
      FRuntimeFloatCurve AgiToCriticalCurve;

   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Status")
      FRuntimeFloatCurve AgiToEvasionCurve;

   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Status")
      FRuntimeFloatCurve AgiToAttackSpeedCurve;

   //==========================================================================
   //          Subscribe to DamageMgr
   //==========================================================================
public:

   // --- When status Effect was applied
   UFUNCTION()
      void ReportStatusEffect (URActiveStatusEffect* Effect,
                               AActor*               Causer,
                               AActor*               Victim);

   UPROPERTY(BlueprintAssignable, Category = "Rade|Status")
      FRStatusEffectEvent OnStatusEffectApplied;

   //==========================================================================
   //           Get instance -> GameState component
   //==========================================================================
public:
   UFUNCTION(BlueprintCallable, BlueprintPure,
             Category = "Rade|Status",
             meta = (HidePin          = "WorldContextObject",
                     DefaultToSelf    = "WorldContextObject",
                     DisplayName      = "Get Status Mgr",
                     CompactNodeTitle = "Status Mgr"))
      static URWorldStatusMgr* GetInstance (UObject* WorldContextObject);
};

