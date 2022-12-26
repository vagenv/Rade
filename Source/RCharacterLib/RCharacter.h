// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "GameFramework/Character.h"
#include "RCharacter.generated.h"

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
   UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rade|Character")
      TObjectPtr<URAnimInstance> BodyAnimInstance;

   //  Inventory Component
   UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rade|Inventory")
      TObjectPtr<URInventoryComponent> Inventory;


   /*
   // Called When Weapon Changed
   UFUNCTION()
      virtual void CurrentWeaponUpdated();
   // Current Weapon
   UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Rade|Character ")
      class ARadeWeapon* TheWeapon;

   */
   //==========================================================================
   //                     Status and Basic Data
   //==========================================================================

   // Character Maximum Health
   UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Rade|Character")
      float HealthMax;

   // Character Current Health
   UPROPERTY(Replicated,EditAnywhere, BlueprintReadWrite, Category = "Rade|Character")
      float Health;

   // Is The Character Dead?
   UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Rade|Character")
      bool bDead;


   //==========================================================================
   //                        Take Damage
   //==========================================================================

   // Damage will be applied only if landing vertical velocity is more than this value
   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rade|Character")
      float FallDamageMinVelocity;

   // Fall Velocity To Damage Curve
   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rade|Character")
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

   // Can Character Revive after death
   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rade|Character")
      bool bAutoRevive = false;

   // Delay Before Revived. (Count started after death event)
   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rade|Character")
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
   UFUNCTION(BlueprintImplementableEvent, Category = "Rade")
      void BP_Revived ();

   UFUNCTION(Server, Reliable)
      void Revive_Server ();
   virtual void Revive_Server_Implementation ();
   UFUNCTION(NetMulticast, Reliable)
      void Revive_Client ();
   virtual void Revive_Client_Implementation ();

   /*

   // Can Character Revive after death
   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rade")
      bool bCanRevive = false;

   // Delay Before Revived. (Count started after death event)
   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rade")
      float ReviveTime = 5;

   // Death on Server
   virtual void ServerDie();

   // Called on all users when Character Died
   UFUNCTION(NetMulticast, Reliable)
      void GlobalDeath();
   virtual void GlobalDeath_Implementation();

   // Revive on Server
   virtual void ServerRevive();

   // Called on all users when Character revived
   UFUNCTION(NetMulticast, Reliable)
      void GlobalRevive();
   virtual void GlobalRevive_Implementation();

public:

   //==========================================================================
   //                        Animation
   //==========================================================================


   // Called from inventory when player wants to equip new weapon
   UFUNCTION(BlueprintCallable, Category = "Rade")
      virtual void EquipWeapon(class ARadeWeapon* NewWeaponClass);

   // Called on server to set animation
   UFUNCTION(Reliable, Server, WithValidation)
      void ServerSetAnimID(ERAnimState AnimID);
   virtual bool ServerSetAnimID_Validate(ERAnimState AnimID);
   virtual void ServerSetAnimID_Implementation(ERAnimState AnimID);

   // Called on all users to set animation
   UFUNCTION(NetMulticast, Reliable)
      void Global_SetAnimID(ERAnimState AnimID);
   virtual void Global_SetAnimID_Implementation(ERAnimState AnimID);

   // Is the Character in Specific Anim State
   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade")
      virtual bool IsAnimState(ERAnimState ThERAnimState);

   // Is Character in Air
   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade")
      bool IsAnimInAir();

   // Called on all users to set Animation Archetype
   UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category = "Rade")
      void Global_SetAnimArchtype(ERAnimArchetype newAnimArchetype);
   virtual void Global_SetAnimArchtype_Implementation(ERAnimArchetype newAnimArchetype);
   */
};

