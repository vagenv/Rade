// Copyright 2015-2017 Vagen Ayrapetyan

#pragma once

#include "Character/RadeCharacter.h"
#include "RadePlayer.generated.h"


// Main Player Class, Includes First Person and 2 Cameras.
UCLASS(config = Game)
class RADE_API ARadePlayer : public ARadeCharacter
{
	GENERATED_BODY()

public:



	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//				Base 

	ARadePlayer(const class FObjectInitializer& PCIP);

	
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime)override;


	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////						 Components and Important References

	// Rade Player Controller
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rade Player")
	class ARadePC* ThePC;

	// Player HUD
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rade Player")
	class ABaseHUD* TheHUD;
	
	// First Person Mesh Anim Instance
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category = "Rade Player")
	class URadeAnimInstance * ArmsAnimInstance;


	//  First Person Mesh
	UPROPERTY(Replicated, VisibleDefaultsOnly, Category = Mesh)
	class USkeletalMeshComponent* Mesh1P;




	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//							Camera

	// First Person Camera
	UPROPERTY(Replicated,VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	class UCameraComponent* FirstPersonCameraComponent;

	// Third Person Camera Boom
	UPROPERTY(Replicated,VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	class USpringArmComponent* ThirdPersonCameraBoom;

	// Third Person Camera
	UPROPERTY(Replicated,VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	class UCameraComponent* ThirdPersonCameraComponent;

	// Default Camera State
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
		ECameraState DefaultCameraState = ECameraState::FP_Camera;

	// Current Camera State
	UPROPERTY(ReplicatedUsing = UpdateComponentsVisibility, EditAnywhere, BlueprintReadWrite, Category = Camera)
	ECameraState CurrentCameraState = ECameraState::FP_Camera;


	// Camera Mouse Sensivity
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
		float CameraMouseSensivity = 1.0;

	// Player pressed ChangeCamera
	virtual void ChangeCamera();


	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//								Inventory and Item Control


	// Currently Selected Pickup Actor
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rade Player")
	class AItemPickup * currentPickup;

	// Save/Load Inventory during game Start/End ?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rade Player")
		bool bSaveInventory;

	// Updates Component Visibility when Camera State Changes
	UFUNCTION()
	void UpdateComponentsVisibility();



	// Current Selected item index in Inventory . Replicated.
	UPROPERTY(Replicated)
		int32 CurrentItemSelectIndex = 0;

	// Is Player inventory open? . Replicated.
	UPROPERTY(Replicated)
		bool bInventoryOpen = false;



	// Called From Client to set Inventory Visibility on server
	UFUNCTION(Reliable, Server, WithValidation, BlueprintCallable, Category = "Rade")
		void SetInventoryVisible(bool bVisible);
	bool SetInventoryVisible_Validate(bool bVisible);
	void SetInventoryVisible_Implementation(bool bVisible);

	// Called From Client to set Inventory Item Selected on server
	UFUNCTION(Reliable, Server, WithValidation, BlueprintCallable, Category = "Rade")
		void SetInventorySelectIndex(int32 index);
	bool SetInventorySelectIndex_Validate(int32 index);
	void SetInventorySelectIndex_Implementation(int32 index);



	// Item was used
	UFUNCTION(BlueprintImplementableEvent, Category = "Rade")
		void BP_ItemUsed(class AItem* TheItem);

	// Item Was Dropped
	UFUNCTION(BlueprintImplementableEvent, Category = "Rade")
		void BP_ItemDroped(class AItemPickup* ThePickup);



	////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//							 Weapon Control and Events

	// Called from inventory when player wants to equip new weapon
	void EquipWeapon(class AWeapon* NewWeaponClass)override;

	// Called when player unequips current weapon
	void UnEquipCurrentWeapon();


	// Time before new weapon is spawned if returning to empty weapon state
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rade Player")
		float DefaultWeaponEquipDelay = 0.4f;


	// Current Weapon Stats Was updated.
	UFUNCTION()
		virtual void CurrentWeaponUpdated()override;


	// Can Player Fire in Air
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rade Player")
		bool bCanFireInAir = false;



private:
	// Internal Events when weapon in changed
	void EquipStart();
	void EquipEnd();
	void UnEquipStart();
	void UnEquipEnd();


	// The next weapon class to equip from inventory (During weapon change)
	UPROPERTY()
		class AWeapon* PendingEquipWeapon;



public:


	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//						Jetpack	Props

	// Is jetpack enabled ?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rade Player")
		bool bJetPackEnabled=true;

	// Jetpack Struct properties
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rade Player")
		FJetPackData JumpJetPack;

	// Jetpack restore timer handle
	FTimerHandle JetPackHandle;

	// Is the Jetpack restoring
	bool bCanFillJetPack;

	// The Event Called by the jetpack timer handle
	void JetPackFillUp();


	////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//			Network Chat And Properties


	// Add Chat Message
	UFUNCTION(Reliable, Server, WithValidation, BlueprintCallable, Category = "Rade")
		void AddChatMessage(const FString & TheMessage);
	bool AddChatMessage_Validate(const FString & TheMessage);
	void AddChatMessage_Implementation(const FString & TheMessage)	;

	virtual void SetCharacterStats_Implementation(const FString & newName, FLinearColor newColor);


	////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//					Animation State


	// Set Animation ID
	virtual void Global_SetAnimID_Implementation(EAnimState AnimID)override;

	// Set Animation Archtype
	virtual void Global_SetAnimArchtype_Implementation(EAnimArchetype newAnimArchetype)override;

	// Is Player in Anim State
	virtual bool IsAnimState(EAnimState TheAnimState)override;


	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//			Death and Revive


	// Called in BP when player died
	UFUNCTION(BlueprintImplementableEvent, Category = "Rade")
		void BP_PlayerDied();

	// Called in BP when player revived
	UFUNCTION(BlueprintImplementableEvent, Category = "Rade")
		void BP_PlayerRevived();

protected:

	// Called When Player Revives
	virtual void ServerRevive()override;

	// Called On Server when player Died
	virtual void ServerDie()override;

	// Called on all users when player died
	virtual void GlobalDeath_Implementation()override;

	// Called on all users when player revived
	virtual void GlobalRevive_Implementation();

public:


	////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//			State Checking


	// Called to reset current move speed when player fired (Fire after sprint)
	UFUNCTION(BlueprintImplementableEvent, Category = "Rade")
		void ResetMoveSpeed();

	// Check player state if he can fire
	virtual bool CanShoot();

	// Can player Sprint
	UFUNCTION(BlueprintCallable, Category = "Rade")
		bool  CanSprint();



	// Player Landed
	virtual void Landed(const FHitResult& Hit)override;




	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//						Blueprint  Action Events

	// Player Pressed Action
	UFUNCTION(BlueprintImplementableEvent, Category = "Rade")
		void BP_Action();

	// Player Pressed MeleeAction
	UFUNCTION(BlueprintImplementableEvent, Category = "Rade")
		void BP_MeleeAction();

	// Player Pressed FAction
	UFUNCTION(BlueprintImplementableEvent, Category = "Rade")
		void BP_FAction();

	// Player Pressed Reload
	UFUNCTION(BlueprintImplementableEvent, Category = "Rade")
		void BP_Reload();

	// Player Pressed Toggle Inventory
	UFUNCTION(BlueprintImplementableEvent, Category = "Rade")
		void BP_ToggleInventory();

	// Player Pressed Jump
	UFUNCTION(BlueprintImplementableEvent, Category = "Rade")
		void BP_Jump();


protected:



	////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//			Network  :  Client Action Input -> Server Action Input



	// Player Pressed Fire, Called on Server
	UFUNCTION(Reliable, Server, WithValidation)
		void FireStart();
	virtual bool FireStart_Validate();
	virtual void FireStart_Implementation();

	// Player Released Fire, Called on Server
	UFUNCTION(Reliable, Server, WithValidation)
		void FireEnd();
	virtual bool FireEnd_Validate();
	virtual void FireEnd_Implementation();


	// Player Pressed AltFire, Called on Server
	UFUNCTION(Reliable, Server, WithValidation)
		void AltFireStart();
	virtual bool AltFireStart_Validate();
	virtual void AltFireStart_Implementation();

	// Player Released AltFire, Called on Server
	UFUNCTION(Reliable, Server, WithValidation)
		void AltFireEnd();
	virtual bool AltFireEnd_Validate();
	virtual void AltFireEnd_Implementation();


	////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//			Internal Client Input 

protected:

	// Binding Player Input to internal events
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;


	//  Player Pressed Action, Called on Server
	UFUNCTION(Reliable, Server, WithValidation)
		void Action();
	virtual bool Action_Validate();
	virtual void Action_Implementation();

	//  Player Pressed AltAction, Called on Server
	UFUNCTION(Reliable, Server, WithValidation)
		virtual void MeleeAction();
	virtual bool MeleeAction_Validate();
	virtual void MeleeAction_Implementation();

	//  Player Pressed FAction, Called on Server
	UFUNCTION(Reliable, Server, WithValidation)
	virtual void FAction();
	virtual bool FAction_Validate();
	virtual void FAction_Implementation();

	//  Player Pressed Reload, Called on Server
	UFUNCTION(Reliable, Server, WithValidation)
	virtual void Reload();
	virtual bool Reload_Validate();
	virtual void Reload_Implementation();

	//  Player Pressed Jump in Air, Called on server
	UFUNCTION(Reliable, Server, WithValidation)
		virtual void DoubleJump();
	virtual bool DoubleJump_Validate();
	virtual void DoubleJump_Implementation();






	// Player Pressed Jump, Called on client
	virtual void Jump()override;

	// Player Pressed Toggle Inventory, Called on client
	virtual void ToggleInventory();

	// Player Scrolled Mouse Wheel, Called on client
	virtual void MouseScroll(float Value);


	// Forward/Bacward movement Input, Called on client
	virtual void MoveForward(float Val);

	// Right/Left movement Input, Called on client
	virtual void MoveRight(float Val);

	// Vertical rotation Input, Called on client
	virtual void AddControllerPitchInput(float Rate) override;

	// Horizontal rotation Input, Called on client
	virtual void AddControllerYawInput(float Rate) override;

	// Mesh Rotation on player rotation input
	void FaceRotation(FRotator NewControlRotation, float DeltaTime)override;

};
