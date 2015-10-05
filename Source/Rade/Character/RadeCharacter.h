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
	UPROPERTY()
	class ARadePC* ThePC;
	UPROPERTY()
	class AItemPickup * currentPickup;
	UPROPERTY()
	class ABaseHUD* TheHUD;

	UPROPERTY()
	class URadeAnimInstance * ArmsAnimInstance;
	UPROPERTY()
	class URadeAnimInstance * BodyAnimInstance;

	UPROPERTY()
	UCharacterMovementComponent* PlayerMovementComponent;

	UPROPERTY(Replicated,VisibleDefaultsOnly, Category = Mesh)
	class USkeletalMeshComponent* Mesh1P;

	UPROPERTY(Replicated,VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	class UCameraComponent* FirstPersonCameraComponent;


	UPROPERTY(Replicated,VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	class USpringArmComponent* ThirdPersonCameraBoom;

	UPROPERTY(Replicated,VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	class UCameraComponent* ThirdPersonCameraComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
		ECameraState DefaultCameraState = ECameraState::FP_Camera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
		FVector2D CameraMaxVerticalAngle = FVector2D(300,85);

	UPROPERTY(ReplicatedUsing = UpdateComponentsVisibility)
	ECameraState CurrentCameraState = ECameraState::FP_Camera;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
		bool bSaveInventory;

	// Checkes all components to hide/unhide them
	UFUNCTION()
	void UpdateComponentsVisibility();

	/*
	UFUNCTION(NetMulticast, Reliable)
		void UpdateComponentsVisibility();
	virtual void UpdateComponentsVisibility_Implementation();
	*/

	// Weapon Events

	void EquipWeapon(class AWeapon* NewWeaponClass);

	void UnEquipCurrentWeapon();

private:
	void EquipStart();
	void EquipEnd();
	void UnEquipStart();
	void UnEquipEnd();

	UPROPERTY()
		class AWeapon* PendingEquipWeapon;

public:
	// System Events





	// Action Events

	UFUNCTION(BlueprintImplementableEvent, Category = "Events")
		void BP_ItemUsed(class AItem* TheItem);

	UFUNCTION(BlueprintImplementableEvent, Category = "Events")
		void BP_ItemDroped(class AItemPickup* thePickup);

	UFUNCTION(BlueprintImplementableEvent, Category = "Events")
		void BP_Action();

	UFUNCTION(BlueprintImplementableEvent, Category = "Events")
		void BP_AltAction();

	UFUNCTION(BlueprintImplementableEvent, Category = "Events")
		void BP_FAction();

	UFUNCTION(BlueprintImplementableEvent, Category = "Events")
		void BP_Reload();

	UFUNCTION(BlueprintImplementableEvent, Category = "Events")
		void BP_ToggleInventory();
	UFUNCTION(BlueprintImplementableEvent, Category = "Events")
		void BP_Jump();









	// Internal Value

	UPROPERTY(Replicated)
		int32 CurrentItemSelectIndex = 0;

	UPROPERTY(Replicated)
		bool bInventoryOpen = false;

	UFUNCTION(Reliable, Server, WithValidation, BlueprintCallable, Category = "Item")
		void SetInventoryVisible(bool bVisible);
	bool SetInventoryVisible_Validate(bool bVisible);
	void SetInventoryVisible_Implementation(bool bVisible);


	UFUNCTION(Reliable, Server, WithValidation, BlueprintCallable, Category = "Item")
		void SetInventorySelectIndex(int32 index);
	bool SetInventorySelectIndex_Validate(int32 index);
	void SetInventorySelectIndex_Implementation(int32 index);



	UFUNCTION()
	virtual void CurrentWeaponUpdated()override;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
		float CameraMouseSensivity = 1.0;



	//						Jetpack	Props

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player")
		bool bCanFireInAir = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player")
		bool bJetPackEnabled=true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player")
		FJetPackProp JumpJetPack;

	FTimerHandle JetPackHandle;

	bool bCanFillJetPack;
	void JetPackFillUp();



	//			Death Revive

protected:
	virtual void Revive();
	virtual void ServerDie()override;
	virtual void GlobalDeath_Implementation()override;

	FVector Mesh_InGameRelativeLoc;
	FRotator Mesh_InGameRelativeRot;


public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player")
		bool bCanRevive = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player")
		float ReviveTime=5;







	//			Anim Replication

	UFUNCTION(Reliable, Server, WithValidation)
		void ServerSetAnimID(EAnimState AnimID);
	virtual bool ServerSetAnimID_Validate(EAnimState AnimID);
	virtual void ServerSetAnimID_Implementation(EAnimState AnimID);

	UFUNCTION(NetMulticast, Reliable)
		void Global_SetAnimID(EAnimState AnimID);
		virtual void Global_SetAnimID_Implementation(EAnimState AnimID);

	UFUNCTION(BlueprintImplementableEvent, Category = "Player State")
		void BP_PlayerDied();
	UFUNCTION(BlueprintImplementableEvent, Category = "Player State")
		void BP_PlayerRevived();



	// Called When start firing
	UFUNCTION(BlueprintImplementableEvent, Category = "Movement Reset")
		void ResetMoveSpeed();


	/// Reseting Part
	void ResetAnimInstances();




	// State Checking
	virtual bool CanShoot();

	UFUNCTION(BlueprintCallable, Category = "Player|Movement")
		bool  CanSprint();




protected:



	// Weapon Fire system
	UFUNCTION(Reliable, Server, WithValidation)
		void FireStart();
	virtual bool FireStart_Validate();
	virtual void FireStart_Implementation();


	UFUNCTION(Reliable, Server, WithValidation)
		void FireEnd();
	virtual bool FireEnd_Validate();
	virtual void FireEnd_Implementation();

	UFUNCTION(Reliable, Server, WithValidation)
		void AltFireStart();
	virtual bool AltFireStart_Validate();
	virtual void AltFireStart_Implementation();

	UFUNCTION(Reliable, Server, WithValidation)
		void AltFireEnd();
	virtual bool AltFireEnd_Validate();
	virtual void AltFireEnd_Implementation();


	virtual void ChangeCamera();

	//Events
	virtual void Landed(const FHitResult& Hit)override;






protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	// End of APawn interface



	//virtual void Action();

	// Action Input

	// Called To Server
	UFUNCTION(Reliable, Server, WithValidation)
		void Action();
	virtual bool Action_Validate();
	virtual void Action_Implementation();

	UFUNCTION(Reliable, Server, WithValidation)
	virtual void AltAction();
	virtual bool AltAction_Validate();
	virtual void AltAction_Implementation();

	UFUNCTION(Reliable, Server, WithValidation)
	virtual void FAction();
	virtual bool FAction_Validate();
	virtual void FAction_Implementation();

	UFUNCTION(Reliable, Server, WithValidation)
	virtual void Reload();
	virtual bool Reload_Validate();
	virtual void Reload_Implementation();


	UFUNCTION(Reliable, Server, WithValidation)
		virtual void DoubleJump();
	virtual bool DoubleJump_Validate();
	virtual void DoubleJump_Implementation();

	// Called To Client

	virtual void Jump()override;
	virtual void ToggleInventory();
	virtual void MouseScroll(float Value);


	// Axis Input
	virtual void MoveForward(float Val);
	virtual void MoveRight(float Val);
	virtual void AddControllerPitchInput(float Rate) override;
	virtual void AddControllerYawInput(float Rate) override;



	void FaceRotation(FRotator NewControlRotation, float DeltaTime)override;

};
