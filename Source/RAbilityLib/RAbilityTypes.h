// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "RUILib/RUIDescription.h"
#include "Engine/DataTable.h"
#include "RAbilityTypes.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE (FRAbilityEvent);

class URAbility;

// ============================================================================
//                   AbilityInfo
// ============================================================================

USTRUCT(Blueprintable, BlueprintType)
struct RABILITYLIB_API FRAbilityInfo : public FTableRowBase
{
   GENERATED_BODY()

   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      TSoftClassPtr<URAbility> AbilityClass;

   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      FRUIDescription Description;

   // Level -> scaling info?

   bool IsValid () const {
      return !AbilityClass.IsNull () && !Description.Label.IsEmpty ();
   };
};

UCLASS(ClassGroup=(_Rade))
class RABILITYLIB_API URAbilityInfolLibrary : public UBlueprintFunctionLibrary
{
   GENERATED_BODY()
public:

   UFUNCTION(BlueprintPure, Category = "Rade|Ability")
	   static bool AbilityInfo_IsValid (const FRAbilityInfo& AbilityInfo);
};

// ============================================================================
//                   Ability Component
// ============================================================================

UCLASS(Abstract, Blueprintable, BlueprintType, ClassGroup=(_Rade))
class RABILITYLIB_API URAbility : public UActorComponent,
                                  public IRGetDescriptionInterface
{
   GENERATED_BODY()
public:

   URAbility ();

   virtual void BeginPlay () override;
   virtual void EndPlay (const EEndPlayReason::Type EndPlayReason) override;

   //==========================================================================
   //                 Core Params
   //==========================================================================

   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Ability")
      FRAbilityInfo GetAbilityInfo () const;

   // Wrapper around GetAbilityInfo ().Description
   virtual FRUIDescription GetDescription_Implementation () override;

   //==========================================================================
   //                 Core Functions
   //==========================================================================

   // Should the effect of ability be applied
   UFUNCTION(BlueprintCallable, Category = "Rade|Ability")
      virtual void SetIsEnabled (bool IsEnabled_);

   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Ability")
      virtual bool GetIsEnabled () const;

private:

   UPROPERTY()
      bool IsEnabled = true;

   // Value must be defined in table
   UPROPERTY()
      FRAbilityInfo AbilityInfo;

   UFUNCTION()
      void PullAbilityInfo ();
};


//=============================================================================
//                 Passive Ability
//=============================================================================

UCLASS(Abstract, Blueprintable, BlueprintType, ClassGroup=(_Rade))
class RABILITYLIB_API URAbility_Passive : public URAbility
{
   GENERATED_BODY()
public:

   URAbility_Passive ();
};

//=============================================================================
//                 Aura Ability
//=============================================================================

UCLASS(Abstract, Blueprintable, BlueprintType, ClassGroup=(_Rade))
class RABILITYLIB_API URAbility_Aura : public URAbility_Passive
{
   GENERATED_BODY()
public:

   URAbility_Aura ();

   virtual void BeginPlay () override;
   virtual void EndPlay (const EEndPlayReason::Type EndPlayReason) override;

   // Called in interval to create AffectedActirs list
   virtual void CheckRange ();

   //==========================================================================
   //                 Params
   //==========================================================================

   // How often to check if target is within range
   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Ability")
      float CheckRangeInterval = 1;

   // Range for actor search
   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Ability")
      float Range = 1000;

   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Ability")
      TSoftClassPtr<AActor> AffectedType;

   // Actors currently within range.
   UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rade|Ability")
      TArray<AActor*> AffectedActors;

   //==========================================================================
   //                 Events
   //==========================================================================

   // Called after AffectedActors list is updated
   UPROPERTY(BlueprintAssignable, Category = "Rade|Ability")
      FRAbilityEvent OnUpdated;

protected:
   UPROPERTY()
      FTimerHandle TimerCheckRange;
};

//=============================================================================
//                 Active Ability
//=============================================================================

UCLASS(Abstract, Blueprintable, BlueprintType, ClassGroup=(_Rade))
class RABILITYLIB_API URAbility_Active : public URAbility
{
   GENERATED_BODY()
public:

   URAbility_Active ();
   virtual void TickComponent (float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

   //==========================================================================
   //                 Params
   //==========================================================================

   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Ability")
      float Cooldown = 3;

   //==========================================================================
   //                 Functions
   //==========================================================================

   // Called by user
   UFUNCTION(BlueprintCallable, Category = "Rade|Ability")
      virtual void Use ();

   // Can the ability be used
   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Ability")
      virtual bool CanUse () const;

   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Ability")
      double GetCooldownLeft () const;

   //==========================================================================
   //                 Events
   //==========================================================================

   // When Ability was used
   UPROPERTY(BlueprintAssignable, Category = "Rade|Ability")
      FRAbilityEvent OnAbilityUsed;

   // When Ability status updated
   UPROPERTY(BlueprintAssignable, Category = "Rade|Ability")
      FRAbilityEvent OnAbilityStatusUpdated;

protected:

   // --- Server version
   UFUNCTION(Server, Reliable, Category = "Rade|Ability")
              void Use_Server ();
      virtual void Use_Server_Implementation ();

   // Effect has been re-applied
   UFUNCTION(NetMulticast, Unreliable, Category = "Rade|Ability")
              void Use_Global ();
      virtual void Use_Global_Implementation ();


   UPROPERTY()
      double UseLastTime = 0;

   UPROPERTY()
      double UseCooldownLeft = 0;

   UPROPERTY()
      bool IsUseable = false;
};

