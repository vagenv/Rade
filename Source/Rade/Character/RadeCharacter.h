// Copyright 2015 Vagen Ayrapetyan

#pragma once

#include "Character/BaseCharacter.h"
#include "RadeData.h"
#include "RadeCharacter.generated.h"



UCLASS(config = Game)
class RADE_API ARadeCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:

	ARadeCharacter(const class FObjectInitializer& PCIP);

	// Main Events
	virtual void BeginPlay() override;


	// Basic References

	// Player Controller
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rade")
	class ARadePC* ThePC;

	// HUD
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rade")
	class ABaseHUD* TheHUD;
	
	// First Person Anim Instance
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rade")
	class URadeAnimInstance * ArmsAnimInstance;

	// Third Person Anim Instance
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rade")
	class URadeAnimInstance * BodyAnimInstance;

	// Movement Component
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rade")
	UCharacterMovementComponent* PlayerMovementComponent;

	// Current Pickup
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rade")
	class AItemPickup * currentPickup;

	//  First Person Mesh
	UPROPERTY(Replicated,VisibleDefaultsOnly, Category = Mesh)
	class USkeletalMeshComponent* Mesh1P;

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
	UPROPERTY(ReplicatedUsing = UpdateComponentsVisibility)
	ECameraState CurrentCameraState = ECameraState::FP_Camera;

	// Save/Load Inventory Enabled
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
		bool bSaveInventory;



	// Updates Component Visibility on Camera State Change
	UFUNCTION()
	void UpdateComponentsVisibility();



	// Weapon Events

	// Called from inventory when player wants to equip new weapon
	void EquipWeapon(class AWeapon* NewWeaponClass);

	// Called when player unequips current weapon
	void UnEquipCurrentWeapon();

private:
	// Internal Events when weapon in changed
	void EquipStart();
	void EquipEnd();
	void UnEquipStart();
	void UnEquipEnd();


	// The next weapon to equip from inventory
	UPROPERTY()
		class AWeapon* PendingEquipWeapon;

public:


	// Action Events

	// When Item Is Used
	UFUNCTION(BlueprintImplementableEvent, Category = "Events")
		void BP_ItemUsed(class AItem* TheItem);

	// When Item is dropped
	UFUNCTION(BlueprintImplementableEvent, Category = "Events")
		void BP_ItemDroped(class AItemPickup* thePickup);

	// Player Pressed Action
	UFUNCTION(BlueprintImplementableEvent, Category = "Events")
		void BP_Action();

	// Player Pressed AltAction
	UFUNCTION(BlueprintImplementableEvent, Category = "Events")
		void BP_AltAction();

	// Player Pressed FAction
	UFUNCTION(BlueprintImplementableEvent, Category = "Events")
		void BP_FAction();

	// Player Pressed Reload
	UFUNCTION(BlueprintImplementableEvent, Category = "Events")
		void BP_Reload();

	// Player Pressed Toggle Inventory
	UFUNCTION(BlueprintImplementableEvent, Category = "Events")
		void BP_ToggleInventory();

	// Player Pressed Jump
	UFUNCTION(BlueprintImplementableEvent, Category = "Events")
		void BP_Jump();



	// Internal Value

	// Current Selected item index replicated between server and client
	UPROPERTY(Replicated)
		int32 CurrentItemSelectIndex = 0;

	// If The player Inventory is open
	UPROPERTY(Replicated)
		bool bInventoryOpen = false;


	// Time before new weapon is spawned if no weapon is currently equiped
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		float DefaultWeaponEquipDelay=0.4f;


	// Called From Client to set Inventory Visibility on server
	UFUNCTION(Reliable, Server, WithValidation, BlueprintCallable, Category = "Item")
		void SetInventoryVisible(bool bVisible);
	bool SetInventoryVisible_Validate(bool bVisible);
	void SetInventoryVisible_Implementation(bool bVisible);

	// Called From Client to set Inventory Item Selected on server
	UFUNCTION(Reliable, Server, WithValidation, BlueprintCallable, Category = "Item")
		void SetInventorySelectIndex(int32 index);
	bool SetInventorySelectIndex_Validate(int32 index);
	void SetInventorySelectIndex_Implementation(int32 index);


	// If Current weapon Stats were updated.
	UFUNCTION()
	virtual void CurrentWeaponUpdated()override;


	// Mouse  Sensivity
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
		float CameraMouseSensivity = 1.0;

	// Can Player Fire in Air
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player")
		bool bCanFireInAir = false;

	//						Jetpack	Props

	// Can Player 

	// Is jetpack enabled
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player")
		bool bJetPackEnabled=true;

	// Jetpack Struct properties
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player")
		FJetPackData JumpJetPack;

	// Jetpack restore timer handle
	FTimerHandle JetPackHandle;

	// Is the Jetpack restoring
	bool bCanFillJetPack;

	// The Event Called by the jetpack timer handle
	void JetPackFillUp();



	//			Death Revive

	// Can Player Revive after death
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player")
		bool bCanRevive = false;

	// Revive delay
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player")
		float ReviveTime = 5;
protected:

	// Called When Player Revives
	virtual void Revive();

	// Called On Server when player Died
	virtual void ServerDie()override;

	// Called on all users when player died
	virtual void GlobalDeath_Implementation()override;

	// Default Mesh Offset before ragdoll
	FVector Mesh_InGameRelativeLoc;
	// Default Mesh Rotation before ragdoll
	FRotator Mesh_InGameRelativeRot;


public:








	//			Anim Replication

	// Called on server to set current animation
	UFUNCTION(Reliable, Server, WithValidation)
		void ServerSetAnimID(EAnimState AnimID);
	virtual bool ServerSetAnimID_Validate(EAnimState AnimID);
	virtual void ServerSetAnimID_Implementation(EAnimState AnimID);
	
	// Called on all users to set server current animation
	UFUNCTION(NetMulticast, Reliable)
		void Global_SetAnimID(EAnimState AnimID);
		virtual void Global_SetAnimID_Implementation(EAnimState AnimID);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Player Anim State")
		bool IsAnimState(EAnimState TheAnimState);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Player Anim State")
		bool IsAnimInAir();

	// Called in BP when player died
	UFUNCTION(BlueprintImplementableEvent, Category = "Player State")
		void BP_PlayerDied();

	// Called in BP when player revived
	UFUNCTION(BlueprintImplementableEvent, Category = "Player State")
		void BP_PlayerRevived();


	// Called to reset current move speed when player fired (Fire after sprint)
	UFUNCTION(BlueprintImplementableEvent, Category = "Movement")
		void ResetMoveSpeed();



	// Check player state if he can fire
	virtual bool CanShoot();

	// Can player Sprint
	UFUNCTION(BlueprintCallable, Category = "Movement")
		bool  CanSprint();




protected:



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

	// Player pressed ChangeCamera
	virtual void ChangeCamera();

	// Player Landed
	virtual void Landed(const FHitResult& Hit)override;



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
	virtual void AltAction();
	virtual bool AltAction_Validate();
	virtual void AltAction_Implementation();

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
