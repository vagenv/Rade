// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "GameFramework/Character.h"
#include "RCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE (FRCharacterEvent);

class URAnimInstance;
class URInventoryComponent;

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

   // Third Person Anim Instance
   UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rade")
      TObjectPtr<URAnimInstance> BodyAnimInstance;

   //  Inventory Component
   UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rade")
      TObjectPtr<URInventoryComponent> Inventory;

   //==========================================================================
   //                     Status and Basic Data
   //==========================================================================

   // Character Maximum Health
   UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Rade")
      float HealthMax = 100;

   // Character Current Health
   UPROPERTY(Replicated,EditAnywhere, BlueprintReadWrite, Category = "Rade")
      float Health = 90;

   // Is The Character Dead?
   UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Rade")
      bool bDead = false;

   //==========================================================================
   //                        Take Damage
   //==========================================================================

   // Fall Velocity To Damage Curve
   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rade")
      FRuntimeFloatCurve FallDamageCurve;

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

   UPROPERTY(BlueprintAssignable, Category = "Rade")
      FRCharacterEvent OnDeath;

   UPROPERTY(BlueprintAssignable, Category = "Rade")
      FRCharacterEvent OnRevive;

   // Can Character Revive after death
   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rade")
      bool bAutoRevive = false;

   // Delay Before Revived. (Count started after death event)
   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rade")
      float ReviveTime = 5;

   // --- Death
   virtual void Die (class AActor* DeathCauser, class AController* EventInstigator);
   UFUNCTION(BlueprintImplementableEvent, Category = "Rade")
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

