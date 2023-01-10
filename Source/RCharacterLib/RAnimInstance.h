// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "Animation/AnimInstance.h"
#include "RAnimInstance.generated.h"

//=============================================================================
//                     Custom Anim Type
//=============================================================================

// Weapon Anim Archetype Enum
UENUM(BlueprintType)
enum class ERAnimArchetype : uint8
{
   EmptyHand        UMETA(DisplayName = "Empty Hands"),
   Melee_Weapon     UMETA(DisplayName = "Melee Weapon"),
   Handgun_Weapon   UMETA(DisplayName = "Hand Gun"),
   Hip_Weapon       UMETA(DisplayName = "Hip Fired Weapon"),
   Shoulder_Weapon  UMETA(DisplayName = "Shoulder Fired Weapon")
};

// Anim State Type
UENUM(BlueprintType)
enum class ERAnimState : uint8
{
   Empty        UMETA(DisplayName = "Empty"),

   JumpStart    UMETA(DisplayName = "Jump Start"),
   JumpEnd      UMETA(DisplayName = "Jump End"),
   Jumploop     UMETA(DisplayName = "Jump Loop"),

   Idle_Run     UMETA(DisplayName = "Idle/Run"),
   Melee_Attack UMETA(DisplayName = "Melee Attack"),

   Fire         UMETA(DisplayName = "Fire"),
   Equip        UMETA(DisplayName = "Equip"),
   UnEquip      UMETA(DisplayName = "Unequip"),
   Reload       UMETA(DisplayName = "Reload"),
};


UCLASS()
class RCHARACTERLIB_API URAnimInstance : public UAnimInstance
{
   GENERATED_BODY()

public:

   URAnimInstance();

   void BeginPlay();


   // Find/Reset Reference to The Character Class
   void ResetRadeCharacterRef();

   //==========================================================================
   //                  Main References and Properties
   //==========================================================================

   // Current Weapon Archetype
   UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Rade|Anim")
      ERAnimArchetype AnimArchetype;

   // Current Animation playing
   UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Rade|Anim" )
      ERAnimState PlayerCurrentAnimState;


   // // Reference to the player
   // UPROPERTY(EditAnywhere, BlueprintReadWrite)
   //    class ARCharacter* TheCharacter;

   //==========================================================================
   //                   Animation Control and Events
   //==========================================================================

   // Notification in blueprint that animation started
   UFUNCTION(BlueprintImplementableEvent, Category = "Rade|Anim")
      void BP_AnimStarted(ERAnimState currentAnimCheck);

   // Set Animation Archetype
   UFUNCTION(BlueprintCallable, Category = "Rade|Anim")
      void SetAnimArchetype(ERAnimArchetype newAnimArchtype);

   // Receive the animation from player Class
   void RecieveGlobalAnimID(ERAnimState currentAnimCheck);

private:
   // Set new Animation
   void PlayLocalAnim(ERAnimState currentAnimCheck);

public:

   //==========================================================================
   //                           State Checking
   //==========================================================================

   // Global Anim state Check
   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Anim")
      bool IsAnimState(ERAnimState currentAnimCheck);
   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Anim")
      bool IsAnimArchetype(ERAnimArchetype AnimArchtypeCheck);

   // Check value from player class
   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Anim")
      bool CanFireInAir();

   // Event called in blueprint when Jump_Start -> Jump_Idle , To enable fire in Air
   UFUNCTION(BlueprintCallable, Category = "Rade|Anim")
      void InAirIdleStateEntered();


   // In air check from current animation state
   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Anim")
      bool IsInAir();


protected:

   //==========================================================================
   //                      Custom Melee Sword
   //==========================================================================

   // Start Trace of Sword Weapon
   UFUNCTION(BlueprintCallable, Category = "Rade|Anim")
      void StartSwordWeaponTrace();

   // End Trace of Sword Weapon
   UFUNCTION(BlueprintCallable, Category = "Rade|Anim")
      void EndSwordWeaponTrace();
};

