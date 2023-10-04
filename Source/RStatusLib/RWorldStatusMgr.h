// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "RStatusTypes.h"
#include "RActiveStatusEffect.h"
#include "RWorldStatusMgr.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams (FRStatusEffectEvent,
                                                URActiveStatusEffect*, Effect,
                                                AActor*,               Causer,
                                                AActor*,               Victim);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam (FRStatusEffectStartEvent,
                                             URActiveStatusEffect*, Effect);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam (FRStatusEffectRefreshEvent,
                                             URActiveStatusEffect*, Effect);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam (FRStatusEffectEndEvent,
                                             URActiveStatusEffect*, Effect);

UCLASS(Blueprintable, BlueprintType, ClassGroup=(_Rade), meta=(BlueprintSpawnableComponent))
class RSTATUSLIB_API URWorldStatusMgr : public UActorComponent
{
   GENERATED_BODY()
public:

   URWorldStatusMgr ();
   // Read table before begin play
   virtual void InitializeComponent () override;
   virtual void BeginPlay () override;

   //==========================================================================
   //             Effect table
   //==========================================================================
private:
   UPROPERTY ()
      TMap<FTopLevelAssetPath, FRActiveStatusEffectInfo> MapStatusEffect;
protected:
   // List of Enemies and experience for attacking / killing them
   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Status",
            meta=(RequiredAssetDataTags = "RowStructure=/Script/RStatusLib.RActiveStatusEffectInfo"))
      UDataTable* StatusEffectTable = nullptr;
public:
   // Gets Effect info and scaling
   UFUNCTION (BlueprintPure, Category = "Rade|Status")
      FRActiveStatusEffectInfo GetEffectInfo (const URActiveStatusEffect * StatusEffect) const;

   //==========================================================================
   //                Get stat growth functions
   //==========================================================================
public:
   // Gets delta stats growth per level
   UFUNCTION (BlueprintPure, Category = "Rade|Status")
      FRCoreStats GetLevelUpStatGain (int CurrentLevel) const;

   //==========================================================================
   //                Stat growth Curves
   //==========================================================================
protected:
   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Status")
      FRuntimeFloatCurve LevelUpStrGainCurve;

   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Status")
      FRuntimeFloatCurve LevelUpAgiGainCurve;

   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Status")
      FRuntimeFloatCurve LevelUpIntGainCurve;

   //==========================================================================
   //                User assignable stat functions
   //==========================================================================
public:
   // Gets delta stats growth per level
   UFUNCTION (BlueprintPure, Category = "Rade|Status")
      float GetLevelUpExtraGain (int CurrentLevel) const;

   //==========================================================================
   //                User assignable stat gain curve
   //==========================================================================
protected:
   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Status")
      FRuntimeFloatCurve LevelUpExtraGainCurve;

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

   // --- Start
   UFUNCTION()
      void ReportStatusEffectStart (URActiveStatusEffect* Effect_);

   UPROPERTY(BlueprintAssignable, Category = "Rade|Status")
      FRStatusEffectStartEvent OnStatusEffectStart;

   // --- Stopped
   UFUNCTION()
      void ReportStatusEffectStop (URActiveStatusEffect* Effect_);

   UPROPERTY(BlueprintAssignable, Category = "Rade|Status")
      FRStatusEffectRefreshEvent OnStatusEffectStop;

   // --- Refresh
   UFUNCTION()
      void ReportStatusEffectRefresh (URActiveStatusEffect* Effect_);

   UPROPERTY(BlueprintAssignable, Category = "Rade|Status")
      FRStatusEffectRefreshEvent OnStatusEffectRefresh;

   // --- End
   UFUNCTION()
      void ReportStatusEffectEnd (URActiveStatusEffect* Effect_);

   UPROPERTY(BlueprintAssignable, Category = "Rade|Status")
      FRStatusEffectEndEvent OnStatusEffectEnd;

   //==========================================================================
   //           Get instance -> GameState component
   //==========================================================================
public:
   UFUNCTION(BlueprintCallable, BlueprintPure,
             Category = "Rade|Status",
             meta = (HidePin          = "WorldContextObject",
                     DefaultToSelf    = "WorldContextObject",
                     DisplayName      = "World Status Mgr",
                     CompactNodeTitle = "World Status Mgr"))
      static URWorldStatusMgr* GetInstance (const UObject* WorldContextObject);
};

