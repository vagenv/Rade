// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "RStatusMgrComponent.generated.h"


USTRUCT(BlueprintType)
struct RCHARACTERLIB_API FRStatusValue
{
   GENERATED_BODY()

   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      float Current = 50;

   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      float Max = 100;

   // Per second
   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      float Regen = 0;
};


// Status Manager Component. Player Health, Stats,
UCLASS(Blueprintable, BlueprintType)
class RCHARACTERLIB_API URStatusMgrComponent : public UActorComponent
{
   GENERATED_BODY()
public:

   // Base events
   URStatusMgrComponent ();
   virtual void GetLifetimeReplicatedProps (TArray<FLifetimeProperty> &OutLifetimeProps) const override;
   virtual void BeginPlay () override;
   virtual void EndPlay (const EEndPlayReason::Type EndPlayReason) override;

private:
      // Has authority to change status
      bool bIsServer = false;
public:

   // --- Core Status

   UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Rade|Character")
      bool bDead = false;

   UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Rade|Status")
      FRStatusValue Health;

   UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Rade|Status")
      FRStatusValue Mana;

   UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Rade|Status")
      FRStatusValue Stamina;


   // Calls from RCharacter
   float TakeDamage (float DamageAmount,
                     FDamageEvent const& DamageEvent,
                     AController* EventInstigator,
                     AActor* DamageCauser);


public:
   // Status Saved / Loaded between sessions.
   // Should be used only for Player.
   // Careful with collision of 'PlayerUniqueId'
   UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Rade|Status")
      bool bSaveLoadStatus = false;

protected:
   // Rade Save events
   UFUNCTION()
      void OnSave ();
   UFUNCTION()
      void OnLoad ();
};

