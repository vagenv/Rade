// Copyright 2015-2021 Vagen Ayrapetyan

#pragma once

#include "GameFramework/Character.h"
#include "../RadeData.h"
#include "RadeCharacter.generated.h"

UCLASS()
class RADE_API ARadeCharacter : public ACharacter
{
   GENERATED_BODY()

public:

   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

   //                     Base

   ARadeCharacter(const class FObjectInitializer& PCIP);

   virtual void BeginPlay() override;
   virtual void PostBeginPlay();


   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

   //                           Components and Important References


   // Third Person Anim Instance
   UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rade Character")
      class URadeAnimInstance * BodyAnimInstance;

   // Movement Component
   UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rade Character")
      UCharacterMovementComponent* CharacterMovementComponent;

   //  Inventory Component
   UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Rade Character")
      class UInventory* TheInventory;


   // Called When Weapon Changed
   UFUNCTION()
      virtual void CurrentWeaponUpdated();
   // Current Weapon
   UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Rade Character ")
      class AWeapon* TheWeapon;


   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

   //                     Status and Basic Data

   // Character Maximum Health
   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rade Character")
      float MaxHealth=100;

   // Character Current Health
   UPROPERTY(Replicated,EditAnywhere, BlueprintReadWrite, Category = "Rade Character")
      float Health=50;

   // Is The Character Dead?
   UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Rade Character")
      bool bDead;


   // Character Name
   UPROPERTY(ReplicatedUsing = OnRep_CharacterStatsUpdated, EditAnywhere, BlueprintReadWrite, Category = "Rade Character")
      FString CharacterName;

   // Character Color
   UPROPERTY(ReplicatedUsing = OnRep_CharacterStatsUpdated , EditAnywhere, BlueprintReadWrite, Category = "Rade Character")
      FLinearColor CharacterColor = FLinearColor::White;

   // Character Stats Updated
   UFUNCTION()
      virtual void OnRep_CharacterStatsUpdated();
   // BP Server Event - Character Died
   UFUNCTION(BlueprintImplementableEvent, Category = "Rade")
      void BP_CharacterStatsUpdated();

   // Set Character Stats
   UFUNCTION(Reliable, Server, WithValidation, BlueprintCallable, Category = "Rade")
      void SetCharacterStats(const FString & newName, FLinearColor newColor);
   bool SetCharacterStats_Validate(const FString & newName, FLinearColor newColor);
   void SetCharacterStats_Implementation(const FString & newName, FLinearColor newColor);


   // Default Inventory Items
   UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Rade Character")
      TArray<TSubclassOf<class AItem> >  DefaultInventoryItems;


   // Called from inventory when player wants to equip new weapon
   UFUNCTION(BlueprintCallable, Category = "Rade")
      virtual void EquipWeapon(class AWeapon* NewWeaponClass);


   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

   //                        Take Damage, Death and Revive

   //  Fall Velocity Acceptable Value, Damage will be applied if fall velocity is more than value
   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rade Character")
      float FallAcceptableValue = 1000;

   // Fall Velocity To Damage Curve
   UPROPERTY(EditAnywhere, Category = "Rade Character")
      FRuntimeFloatCurve FallDamageCurve;


   // Take Damage override
   virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser)override;

   // Override Land Event
   virtual void Landed(const FHitResult& Hit)override;

   // Can Character Revive after death
   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rade Character")
      bool bCanRevive = false;

   // Delay Before Revived. (Count started after death event)
   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rade Character")
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

   // Start Ragdoll Mode
   virtual void ForceRagdoll();

   // BP Server Event - Character Died
   UFUNCTION(BlueprintImplementableEvent, Category = "Rade")
      void BP_CharacterDeath();

   UFUNCTION(BlueprintImplementableEvent, Category = "Rade")
      void BP_CharacterRevive();

private:
   // Default Mesh Offset before ragdoll
   FVector  Mesh_DefaultRelativeLoc;
   // Default Mesh Rotation before ragdoll
   FRotator Mesh_DefaultRelativeRot;

public:

   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////

   //                        Animation

   // Called on server to set animation
   UFUNCTION(Reliable, Server, WithValidation)
      void ServerSetAnimID(EAnimState AnimID);
   virtual bool ServerSetAnimID_Validate(EAnimState AnimID);
   virtual void ServerSetAnimID_Implementation(EAnimState AnimID);

   // Called on all users to set animation
   UFUNCTION(NetMulticast, Reliable)
      void Global_SetAnimID(EAnimState AnimID);
   virtual void Global_SetAnimID_Implementation(EAnimState AnimID);

   // Is the Character in Specific Anim State
   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade")
      virtual bool IsAnimState(EAnimState TheAnimState);

   // Is Character in Air
   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade")
      bool IsAnimInAir();


   // Called on all users to set Animation Archetype
   UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category = "Rade")
      void Global_SetAnimArchtype(EAnimArchetype newAnimArchetype);
   virtual void Global_SetAnimArchtype_Implementation(EAnimArchetype newAnimArchetype);
};
