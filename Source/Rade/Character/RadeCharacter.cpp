// Copyright 2015 Vagen Ayrapetyan

#include "Rade.h"
#include "RadeCharacter.h"
#include "RadeGameMode.h"
#include "BaseHUD.h"
#include "RadePC.h"

#include "Weapon/Projectile.h"
#include "Weapon/Weapon.h"
#include "Character/RadeAnimInstance.h"
#include "Item/Inventory.h"
#include "Item/ItemPickup.h"
#include "Custom/SystemSaveGame.h"
#include "UnrealNetwork.h"


ARadeCharacter::ARadeCharacter(const class FObjectInitializer& PCIP) 
	: Super(PCIP)
{

	
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);


	// Create a CameraComponent	
	FirstPersonCameraComponent = PCIP.CreateDefaultSubobject<UCameraComponent>(this, TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->AttachParent = GetCapsuleComponent();
	FirstPersonCameraComponent->RelativeLocation = FVector(0, 0, 64.f); // Position the camera


	// Create a camera boom (pulls in towards the player if there is a collision)
	ThirdPersonCameraBoom = PCIP.CreateDefaultSubobject<USpringArmComponent>(this, TEXT("CameraBoom"));
	ThirdPersonCameraBoom->AttachTo(RootComponent);
	ThirdPersonCameraBoom->TargetArmLength = 150;	
	ThirdPersonCameraBoom->RelativeLocation = FVector(0,50,100);
	ThirdPersonCameraBoom->bUsePawnControlRotation = true; 

	// Create a follow camera
	ThirdPersonCameraComponent = PCIP.CreateDefaultSubobject<UCameraComponent>(this, TEXT("PlayerCamera"));
	ThirdPersonCameraComponent->AttachTo(ThirdPersonCameraBoom, USpringArmComponent::SocketName);


	// Set First Person Mesh
	Mesh1P = PCIP.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);		
	Mesh1P->AttachParent = FirstPersonCameraComponent;
	Mesh1P->RelativeLocation = FVector(0.f, 0.f, -150.f);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->bOnlyOwnerSee = true;
	Mesh1P->SetIsReplicated(true);

	// Set Third Person Mesh
	GetMesh()->SetOwnerNoSee(true);
	GetMesh()->AttachParent = RootComponent;
	GetMesh()->bCastDynamicShadow = true;
	GetMesh()->CastShadow = true;
	GetMesh()->bOwnerNoSee = true;
	GetMesh()->SetIsReplicated(true);

}


void ARadeCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Get Default Third Person Mesh Relative Location and Rotation
	if (GetMesh())
	{
		Mesh_InGameRelativeLoc = GetMesh()->RelativeLocation;
		Mesh_InGameRelativeRot = GetMesh()->RelativeRotation;
	}
	
	// Get Player Controller
	if (GetController() && Cast<ARadePC>(GetController()))
	{
		ThePC = Cast<ARadePC>(GetController());
	}

	// Get HUD
	if (ThePC && ThePC->GetHUD() && Cast<ABaseHUD>(ThePC->GetHUD()))TheHUD = Cast<ABaseHUD>(ThePC->GetHUD());

	// Manage Inventory
	if (TheInventory)
	{
		// Set player Ref in Inventory
		TheInventory->ThePlayer = this;

		// Load Inventory 
		if (bSaveInventory && Role >= ROLE_Authority)
		{
			TheInventory->LoadInventory();
		}
	}

	// Get Player Movement Component
	if (Cast<UCharacterMovementComponent>(GetMovementComponent()))
	{
		PlayerMovementComponent = Cast<UCharacterMovementComponent>(GetMovementComponent());
	}

	// Get First Person Anim Instance
	if (Mesh1P && Mesh1P->GetAnimInstance() && Cast<URadeAnimInstance>(Mesh1P->GetAnimInstance()))
		ArmsAnimInstance = Cast<URadeAnimInstance>(Mesh1P->GetAnimInstance());

	// Get Third Person Anim Instance
	if (GetMesh() && GetMesh()->GetAnimInstance() && Cast<URadeAnimInstance>(GetMesh()->GetAnimInstance()))
		BodyAnimInstance = Cast<URadeAnimInstance>(GetMesh()->GetAnimInstance());

	// Set Player Ref in Anim Instance
	if (ArmsAnimInstance)ArmsAnimInstance->ThePlayer = this;
	if (BodyAnimInstance)BodyAnimInstance->ThePlayer = this;


	// Set Default Anim State
	ServerSetAnimID(EAnimState::Idle_Run);

	// Set Current Camera to Default State
	CurrentCameraState = DefaultCameraState;
	UpdateComponentsVisibility();

	// Enable and start jetpack FillUp
	bCanFillJetPack = true;
	GetWorldTimerManager().SetTimer(JetPackHandle, this, &ARadeCharacter::JetPackFillUp, JumpJetPack.RestoreSpeed, true);

}


bool ARadeCharacter::Action_Validate()
{
	return true;
}
void ARadeCharacter::Action_Implementation()
{
	// Action Called in Blueprint
	BP_Action();
	
	// Check if player can use items
	if (bInventoryOpen && IsAnimState(EAnimState::Idle_Run)
		&& PlayerMovementComponent&& PlayerMovementComponent->IsMovingOnGround())
	{
		if (TheInventory)
			TheInventory->ActionIndex(CurrentItemSelectIndex);
	}

	// Checki if player is near any Pickup
	else if (currentPickup)
	{
		currentPickup->PickedUp(this);
		currentPickup = NULL;
	}


}

// Player Pressed Alt Action
bool ARadeCharacter::AltAction_Validate()
{
	return true;
}
void ARadeCharacter::AltAction_Implementation()
{
	// Call Alt action from Blueprint
	BP_AltAction();
	
}

// Player Pressed CameraChange
void ARadeCharacter::ChangeCamera()
{
	// Change Camera
	if (DefaultCameraState == ECameraState::FP_Camera)
	{
		DefaultCameraState = ECameraState::TP_Camera;
		CurrentCameraState = DefaultCameraState;
		UpdateComponentsVisibility();
	}
	else if (DefaultCameraState == ECameraState::TP_Camera)
	{
		DefaultCameraState = ECameraState::FP_Camera;
		CurrentCameraState = DefaultCameraState;
		UpdateComponentsVisibility();
	}

}

// Player Pressed FAction
bool ARadeCharacter::FAction_Validate()
{
	return true;
}
void ARadeCharacter::FAction_Implementation()
{
	// Call FAction on Blueprint
	BP_FAction();

	// Thow out Current item selected
	if (TheInventory && bInventoryOpen && IsAnimState(EAnimState::Idle_Run))
	{
		TheInventory->ThrowOutIndex(CurrentItemSelectIndex);
	}
}

// Player Pressed Reload
bool ARadeCharacter::Reload_Validate()
{
	return true;
}
void ARadeCharacter::Reload_Implementation()
{
	// Call Reload on Blueprint
	BP_Reload();

	// Check if Player and Weapon are in state to reload
	if (IsAnimState(EAnimState::Idle_Run)
		&& PlayerMovementComponent && PlayerMovementComponent->IsMovingOnGround() 
		&& TheWeapon &&  TheWeapon->CanReload())
	{
		TheWeapon->ReloadWeaponStart();
	}
}


// Toggle HUD Inventory visibility
void ARadeCharacter::ToggleInventory()
{
	BP_ToggleInventory();

	if (TheHUD)
	{
		// Call Toggle event on HUD
		TheHUD->ToggleInventory();
		if (CurrentItemSelectIndex != TheHUD->CurrentItemSelectIndex) 
			SetInventorySelectIndex(TheHUD->CurrentItemSelectIndex);

		// Tell serer that inventory was toggled
		SetInventoryVisible(TheHUD->bInventoryOpen);
	}	
}

// Server set Inventory visibility 
bool ARadeCharacter::SetInventoryVisible_Validate(bool bVisible)
{
	return true;
}
void ARadeCharacter::SetInventoryVisible_Implementation(bool bVisible)
{
	bInventoryOpen = bVisible;
}


// Player Scrolled Mouse Wheel
void ARadeCharacter::MouseScroll(float Value)
{
	// Call Scroll on HUd
	if (TheHUD)
	{
		if (Value>0)
		{
			TheHUD->BP_InventoryScrollUp();
		}
		else if (Value<0)
		{	
			TheHUD->BP_InventoryScrollDown();
		}

		// Tell Server that Current Selected Item was Updated
		if (CurrentItemSelectIndex != TheHUD->CurrentItemSelectIndex)
			SetInventorySelectIndex(TheHUD->CurrentItemSelectIndex);
	}
}

// Set Server Player Selected Item Index
bool ARadeCharacter::SetInventorySelectIndex_Validate(int32 index)
{
	return true;
}
void ARadeCharacter::SetInventorySelectIndex_Implementation(int32 index)
{
	CurrentItemSelectIndex = index;
}

// Called to equip new Weapon
void ARadeCharacter::EquipWeapon(AWeapon* NewWeaponClass)
{
	if (!NewWeaponClass)return;

	// If player has a weapon, Unequip it
	if (TheWeapon)
	{
		TheWeapon->UnEquipStart();
	}

	// Set next equip weapon
	PendingEquipWeapon = NewWeaponClass;

	// Start Player unequip anim even if there is no current weapon
	UnEquipStart();
}

// Current Weapon in being unequiped
void ARadeCharacter::UnEquipCurrentWeapon()
{
	if (TheWeapon)
	{
		TheWeapon->UnEquipStart();
	}
	UnEquipStart();
}

// Player Started equip of new weapon
void ARadeCharacter::EquipStart()
{
	if (PendingEquipWeapon && !TheWeapon)
	{
		// Set next weapon anim type
		ArmsAnimInstance->AnimArchetype = PendingEquipWeapon->AnimArchetype;

		// Set Animation state
		ServerSetAnimID(EAnimState::Equip);

		// Spawn new weapon
		TheWeapon = GetWorld()->SpawnActor<AWeapon>(PendingEquipWeapon->GetClass());
		TheWeapon->ThePlayer = this;
		TheWeapon->SetOwner(this);
		TheWeapon->EquipStart();

		FTimerHandle myHandle;
		GetWorldTimerManager().SetTimer(myHandle, this, &ARadeCharacter::EquipEnd, TheWeapon->EquipTime, false);
	}
	UpdateComponentsVisibility();
}

// Called when equip is finished
void ARadeCharacter::EquipEnd()
{
	ServerSetAnimID(EAnimState::Idle_Run);

	// Clear pending weapon
	if (PendingEquipWeapon!=NULL)PendingEquipWeapon = NULL;

	// Tell hud that weapon is updated
	if (TheHUD)
	{
		TheHUD->BP_WeaponUpdated();
	}

}

// Unequip curent weapon
void ARadeCharacter::UnEquipStart()
{
	// Set Animation State
	ServerSetAnimID(EAnimState::UnEquip);


	if (TheWeapon)
	{
		// Set Delay of current weapon
		FTimerHandle myHandle;
		GetWorldTimerManager().SetTimer(myHandle, this, &ARadeCharacter::UnEquipEnd, TheWeapon->EquipTime, false);
	}
	else 
	{
		// Set Default Delay
		FTimerHandle myHandle;
		GetWorldTimerManager().SetTimer(myHandle, this, &ARadeCharacter::UnEquipEnd, DefaultWeaponEquipDelay, false);
	}

	UpdateComponentsVisibility();

}

// Unequip Ended
void ARadeCharacter::UnEquipEnd()
{
	// Destroy Current weapon
	if (TheWeapon)
	{
		TheWeapon->Destroy();
		TheWeapon = NULL;
	}

	// Weapon Was Updated
	if (TheHUD)
	{
		TheHUD->BP_WeaponUpdated();
	}

	// if there is new weapon Start the equip
	if (PendingEquipWeapon)
	{
		EquipStart();
	}
	// Else return to empty hand state
	else
	{
		ArmsAnimInstance->AnimArchetype = EAnimArchetype::EmptyHand;
		ServerSetAnimID(EAnimState::Idle_Run);
	}
}

void ARadeCharacter::CurrentWeaponUpdated()
{
	Super::CurrentWeaponUpdated();

	if (ArmsAnimInstance)
	{
		if (TheWeapon)
			ArmsAnimInstance->AnimArchetype = TheWeapon->AnimArchetype;
		else 
			ArmsAnimInstance->AnimArchetype = EAnimArchetype::EmptyHand;
	}

	if (BodyAnimInstance)
	{
		if (TheWeapon)
			BodyAnimInstance->AnimArchetype = TheWeapon->AnimArchetype;
		else
			BodyAnimInstance->AnimArchetype = EAnimArchetype::EmptyHand;
	}



	if (TheHUD)
	{
		TheHUD->BP_WeaponUpdated();
	}

	UpdateComponentsVisibility();
}


// Player Pressed Fire Button
bool ARadeCharacter::FireStart_Validate()
{
	return true;
}

void ARadeCharacter::FireStart_Implementation()
{
	// Check if player has weapn and is in state to fire weapon
	if (TheWeapon && CanShoot())
		TheWeapon->FireStart();
}

bool ARadeCharacter::FireEnd_Validate(){
	return true;
}

// Player Released Fire button
void ARadeCharacter::FireEnd_Implementation()
{
	if (TheWeapon)
		TheWeapon->FireEnd();
}

// Player Pressed Alt Fire
bool ARadeCharacter::AltFireStart_Validate(){
	return true;
}
void ARadeCharacter::AltFireStart_Implementation()
{
	if (TheWeapon)
		TheWeapon->AltFireStart();
}

// Player Released AltFire
bool ARadeCharacter::AltFireEnd_Validate(){
	return true;
}
void ARadeCharacter::AltFireEnd_Implementation()
{
	if (TheWeapon)
		TheWeapon->AltFireEnd();
}



// Checking if player can shoot
bool ARadeCharacter::CanShoot()
{
	// Player and mesh is in the state where he can shoot
	if (ArmsAnimInstance && (IsAnimState(EAnimState::Idle_Run) || (IsAnimState(EAnimState::Jumploop) && bCanFireInAir)))
		return true;
	
	return false;
}

// Check if player can sprint 
bool ARadeCharacter::CanSprint()
{
	bool bReturnCanShoot = true;
	if (TheWeapon && TheWeapon->bShooting)bReturnCanShoot = false;
	if (!IsAnimState(EAnimState::Idle_Run)) bReturnCanShoot = false;
	if (PlayerMovementComponent && !PlayerMovementComponent->IsMovingOnGround())bReturnCanShoot = false;

	return bReturnCanShoot;
}

// Player landed on ground
void ARadeCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	// Start Jetpack FillUp
	if (!bCanFillJetPack)
		bCanFillJetPack = true;

	if (ArmsAnimInstance || BodyAnimInstance)
		ServerSetAnimID(EAnimState::Idle_Run);
}



//////////////// Network Anim

// Set Server Anim State
bool ARadeCharacter::ServerSetAnimID_Validate(EAnimState AnimID){
	return true;
}
void ARadeCharacter::ServerSetAnimID_Implementation(EAnimState AnimID)
{
	// Set new State on all users
	Global_SetAnimID(AnimID);
}

void ARadeCharacter::Global_SetAnimID_Implementation(EAnimState AnimID)
{
	// Set The Value in anim instances
	if (ArmsAnimInstance)
		ArmsAnimInstance->RecieveGlobalAnimID(AnimID);
	if (BodyAnimInstance)
		BodyAnimInstance->RecieveGlobalAnimID(AnimID);

}

// Check Anim State on body or arms
bool ARadeCharacter::IsAnimState(EAnimState TheAnimState)
{
	if (ArmsAnimInstance)
	{
		if (ArmsAnimInstance->IsAnimState(TheAnimState))return true;
		else return false;

	}
	else if (BodyAnimInstance)
	{
		if (BodyAnimInstance->IsAnimState(TheAnimState))return true;
		else return false;
	}

	return true;
}
// Is Player In Air
bool ARadeCharacter::IsAnimInAir()
{
	// Check each air state separetly.
	if (IsAnimState(EAnimState::JumpEnd) || IsAnimState(EAnimState::Jumploop) || IsAnimState(EAnimState::JumpStart))return true;
	else return false;
}

// Player Pressed Jump
void ARadeCharacter::Jump()
{
	if (!PlayerMovementComponent) return;

	// If Player on ground -> Simple Jump
	if (!IsAnimInAir() && PlayerMovementComponent->IsMovingOnGround())
	{
		ACharacter::Jump();
		ServerSetAnimID(EAnimState::JumpStart);
	}
	// else If Player is in air Double Jump
	else if (bJetPackEnabled && bCanFillJetPack && JumpJetPack.CurrentChargePercent>JumpJetPack.MinUseablePercent)
	{
		DoubleJump();
		JumpJetPack.CurrentChargePercent = 0;
		bCanFillJetPack = false;
	}
}

bool ARadeCharacter::DoubleJump_Validate(){
	return true;
}
void ARadeCharacter::DoubleJump_Implementation()
{	
	// Set player Velocity on server
	GetCharacterMovement()->Velocity.Z += JumpJetPack.CurrentChargePercent*JumpJetPack.PushPower;
	JumpJetPack.CurrentChargePercent = 0;
	bCanFillJetPack = false;
}

// Jetpack Fill Up
void ARadeCharacter::JetPackFillUp()
{
	// Fill Up jetpack if can
	if (bCanFillJetPack && JumpJetPack.CurrentChargePercent<100)
	{
		JumpJetPack.CurrentChargePercent += JumpJetPack.RestorePower;
		if (JumpJetPack.CurrentChargePercent>100)
		{
			JumpJetPack.CurrentChargePercent = 100;
		}
	}
}

// Update First Person and Third Person Components visibility
void ARadeCharacter::UpdateComponentsVisibility()
{

	if (CurrentCameraState==ECameraState::FP_Camera)
	{
		if (ThirdPersonCameraComponent)ThirdPersonCameraComponent->Deactivate();

		if (FirstPersonCameraComponent)FirstPersonCameraComponent->Activate();

		if (GetMesh())
			GetMesh()->SetOwnerNoSee(true);
	
		if (Mesh1P)
			Mesh1P->SetVisibility(true);
	
		if (TheWeapon)
		{
			if (TheWeapon->Mesh1P)
				TheWeapon->Mesh1P->SetVisibility(true);
				
			if (TheWeapon->Mesh3P)
				TheWeapon->Mesh3P->SetOwnerNoSee(true);
		}
	}

	// Currently Third Person Camera
	else if (CurrentCameraState == ECameraState::TP_Camera)
	{
		if (ThirdPersonCameraComponent)ThirdPersonCameraComponent->Activate();

		if (FirstPersonCameraComponent)FirstPersonCameraComponent->Deactivate();

		if (GetMesh())
			GetMesh()->SetOwnerNoSee(false);

		if (Mesh1P)
			Mesh1P->SetVisibility(false);

		if (TheWeapon)
		{
			if (TheWeapon->Mesh1P)
				TheWeapon->Mesh1P->SetVisibility(false);

			if (TheWeapon->Mesh3P)
				TheWeapon->Mesh3P->SetOwnerNoSee(false);
		}
	}
}

// Player Died , called on Server
void ARadeCharacter::ServerDie()
{
	// Stop any Fire
	FireEnd();
	bDead = true;
	// Switch to third Person View on death, to look at body
	CurrentCameraState = ECameraState::TP_Camera;

	Super::ServerDie();
}
// Player Died, Called on all users
void ARadeCharacter::GlobalDeath_Implementation()
{
	// save third person mesh Relative Location and rotation before ragdoll
	if (GetMesh())
	{
		Mesh_InGameRelativeLoc = GetMesh()->RelativeLocation;
		Mesh_InGameRelativeRot = GetMesh()->RelativeRotation;
	}

	// Disable player input
	DisableInput(Cast<APlayerController>(Controller));

	// If Player can revive, revive hit after a delay
	if (bCanRevive)
	{
		FTimerHandle MyHandle;
		GetWorldTimerManager().SetTimer(MyHandle, this, &ARadeCharacter::Revive, ReviveTime, false);
	}

	// Event and Ragdoll
	Super::GlobalDeath_Implementation();

	// Update Visibility
	UpdateComponentsVisibility();

	// Call on Blueprint
	BP_PlayerDied();
}


// Revive Player
void ARadeCharacter::Revive()
{
	// Enable Input
	EnableInput(Cast<APlayerController>(Controller));

	// Set Camer to Default Camera state
	CurrentCameraState = DefaultCameraState;

	// Resoter Half of player health
	Health = MaxHealth/2;
	bDead = false;

	
	// Restore Third Person Mesh to default State
	if (GetMesh())
	{
		GetMesh()->SetSimulatePhysics(false);
		GetMesh()->AttachTo(RootComponent);
		GetMesh()->RelativeLocation = Mesh_InGameRelativeLoc;
		GetMesh()->RelativeRotation = Mesh_InGameRelativeRot;
		GetMesh()->BodyInstance.SetCollisionProfileName("Pawn");
	}



	// Find The Closest Revive Point
	TActorIterator<APlayerStart> p(GetWorld());
	APlayerStart* revivePoint = *p;

	for (TActorIterator<APlayerStart> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		if (revivePoint && FVector::Dist(revivePoint->GetActorLocation(), GetActorLocation())> FVector::Dist(ActorItr->GetActorLocation(), GetActorLocation()))
		{
			revivePoint = *ActorItr;
		}
	}

	UpdateComponentsVisibility();

	// Create a small offset from the spawn point
	if (revivePoint)
		GetRootComponent()->SetWorldLocation(revivePoint->GetActorLocation() + FVector(FMath::RandRange(-400, 400), FMath::RandRange(-400, 400), 60));

	//GetMovementComponent()->Activate();

	// Call Revive on Blueprint
	BP_PlayerRevived();
}



// Bind input to events
void ARadeCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	// set up gameplay key bindings
	check(InputComponent);

	InputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);

	InputComponent->BindAction("Reload", IE_Pressed, this, &ARadeCharacter::Reload);

	InputComponent->BindAction("Action", IE_Pressed, this, &ARadeCharacter::Action);
	InputComponent->BindAction("AltAction", IE_Pressed, this, &ARadeCharacter::AltAction);
	InputComponent->BindAction("Inventory", IE_Pressed, this, &ARadeCharacter::ToggleInventory);
	InputComponent->BindAction("FAction", IE_Pressed, this, &ARadeCharacter::FAction);
	InputComponent->BindAction("ChangeCamera", IE_Pressed, this, &ARadeCharacter::ChangeCamera);

	InputComponent->BindAction("Fire", IE_Pressed, this, &ARadeCharacter::FireStart);
	InputComponent->BindAction("Fire", IE_Released, this, &ARadeCharacter::FireEnd);

	InputComponent->BindAction("AltFire", IE_Pressed, this, &ARadeCharacter::AltFireStart);
	InputComponent->BindAction("AltFire", IE_Released, this, &ARadeCharacter::AltFireEnd);


	InputComponent->BindAxis("MouseScroll", this, &ARadeCharacter::MouseScroll);

	InputComponent->BindAxis("MoveForward", this, &ARadeCharacter::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &ARadeCharacter::MoveRight);
	InputComponent->BindAxis("Turn", this, &ARadeCharacter::AddControllerYawInput);
	InputComponent->BindAxis("LookUp", this, &ARadeCharacter::AddControllerPitchInput);
}

//////////////////////////////////////////////////////////////////////////
// Input Handling


// Player Mesh Rotation after after the input
void ARadeCharacter::FaceRotation(FRotator NewControlRotation, float DeltaTime)
{
	Super::FaceRotation(NewControlRotation,DeltaTime);


	if (FirstPersonCameraComponent)
	{
		FRotator rot = FirstPersonCameraComponent->GetComponentRotation();
		rot.Pitch = NewControlRotation.Pitch;
		FirstPersonCameraComponent->SetWorldRotation(rot);
	}
}

// Vertical Rotation Input
void ARadeCharacter::AddControllerPitchInput(float Rate){
	Super::AddControllerPitchInput(Rate*CameraMouseSensivity);
}

// Horizontal Rotation Input
void ARadeCharacter::AddControllerYawInput(float Rate){
	Super::AddControllerYawInput(Rate*CameraMouseSensivity);
}

// Forward/Backward Movement Input
void ARadeCharacter::MoveForward(float Value){
	if (Value != 0.0f)AddMovementInput(GetActorForwardVector(), Value);
}

// Right/Left Movement Input
void ARadeCharacter::MoveRight(float Value){
	if (Value != 0.0f)AddMovementInput(GetActorRightVector(), Value);
}

// Replication of data
void ARadeCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARadeCharacter, Mesh1P);
	DOREPLIFETIME(ARadeCharacter, bInventoryOpen);
	DOREPLIFETIME(ARadeCharacter, CurrentItemSelectIndex);
	DOREPLIFETIME(ARadeCharacter, CurrentCameraState);


}