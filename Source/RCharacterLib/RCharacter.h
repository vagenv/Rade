// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "GameFramework/Character.h"
#include "RCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE (FRCharacterEvent);

// class URAnimInstance;
class URInventoryComponent;
class URStatusMgrComponent;

UCLASS(Blueprintable)
class RCHARACTERLIB_API ARCharacter : public ACharacter
{
   GENERATED_BODY()

public:

   //==========================================================================
   //                     Core
   //==========================================================================

   ARCharacter ();
   virtual void GetLifetimeReplicatedProps (TArray<FLifetimeProperty> &OutLifetimeProps) const override;
   virtual void BeginPlay () override;

   //==========================================================================
   //              Components and Important References
   //==========================================================================

   //  Inventory Component
   UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Rade|Character")
      TObjectPtr<URInventoryComponent> Inventory;

   //  Inventory Component
   UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Rade|Character")
      TObjectPtr<URStatusMgrComponent> StatusMgr;

   //==========================================================================
   //                        Take Damage
   //==========================================================================

   // Something hit
   virtual float TakeDamage (float DamageAmount,
                             struct FDamageEvent const& DamageEvent,
                             class AController* EventInstigator,
                             class AActor* DamageCauser) override;

   // Override Land Event
   virtual void Landed (const FHitResult& Hit) override;

   //==========================================================================
   //                        Death and Revive
   //==========================================================================

private:
   // Default Mesh Offset before ragdoll
   FVector  Mesh_DefaultRelativeLoc;
   // Default Mesh Rotation before ragdoll
   FRotator Mesh_DefaultRelativeRot;

public:

   UPROPERTY(BlueprintAssignable, Category = "Rade|Character")
      FRCharacterEvent OnDeath;

   UPROPERTY(BlueprintAssignable, Category = "Rade|Character")
      FRCharacterEvent OnRevive;

   // Can Character Revive after death
   UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Rade|Character")
      bool bAutoRevive = false;

   // Delay Before Revived. (Count started after death event)
   UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Rade|Character")
      float ReviveTime = 5;

   // --- Death
   virtual void Die (class AActor* DeathCauser, class AController* EventInstigator);
   UFUNCTION(BlueprintImplementableEvent, Category = "Rade|Character")
      void BP_Died (class AActor* DeathCauser, class AController* EventInstigator);
   virtual void ForceRagdoll ();

   // Server event
   UFUNCTION(Server, Reliable)
      void Die_Server(class AActor* DeathCauser, class AController* EventInstigator);
   virtual void Die_Server_Implementation (class AActor* DeathCauser, class AController* EventInstigator);
   UFUNCTION(NetMulticast, Reliable)
      void Die_Client (class AActor* DeathCauser, class AController* EventInstigator);
   virtual void Die_Client_Implementation (class AActor* DeathCauser, class AController* EventInstigator);

   // --- Revive
   virtual void Revive ();

   UFUNCTION(Server, Reliable)
      void Revive_Server ();
   virtual void Revive_Server_Implementation ();
   UFUNCTION(NetMulticast, Reliable)
      void Revive_Client ();
   virtual void Revive_Client_Implementation ();
};

