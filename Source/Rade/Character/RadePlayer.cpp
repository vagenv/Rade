// Copyright 2015-2017 Vagen Ayrapetyan

#include "RadePlayer.h"
//#include "Engine.h"
#include "Rade.h"
#include "UnrealNetwork.h"

#include "RadeGameMode.h"
#include "BaseHUD.h"
#include "RadePC.h"


#include "Character/RadeAnimInstance.h"

#include "Weapon/Projectile.h"
#include "Weapon/Weapon.h"

#include "Item/Inventory.h"
#include "Item/ItemPickup.h"

#include "System/SystemSaveGame.h"
#include "System/RadeGameState.h"
#include "System/RadePlayerState.h"


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//										 Base 

ARadePlayer::ARadePlayer(const class FObjectInitializer& PCIP) 
	: Super(PCIP), ThePC(NULL), TheHUD(NULL)
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);


	// Create a CameraComponent	
	FirstPersonCameraComponent = PCIP.CreateDefaultSubobject<UCameraComponent>(this, TEXT("FirstPersonCamera"));
   FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent(), NAME_None);
	FirstPersonCameraComponent->RelativeLocation = FVector(0, 0, 64.f); // Position the camera


	// Create a camera boom (pulls in towards the player if there is a collision)
	ThirdPersonCameraBoom = PCIP.CreateDefaultSubobject<USpringArmComponent>(this, TEXT("CameraBoom"));
	ThirdPersonCameraBoom->SetupAttachment(RootComponent, NAME_None);
	ThirdPersonCameraBoom->TargetArmLength = 150;	
	ThirdPersonCameraBoom->RelativeLocation = FVector(0,50,100);
	ThirdPersonCameraBoom->bUsePawnControlRotation = true; 

	// Create a follow camera
	ThirdPersonCameraComponent = PCIP.CreateDefaultSubobject<UCameraComponent>(this, TEXT("PlayerCamera"));
	ThirdPersonCameraComponent->SetupAttachment(ThirdPersonCameraBoom, USpringArmComponent::SocketName);


	// Set First Person Mesh
	Mesh1P = PCIP.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);		
   Mesh1P->SetupAttachment(FirstPersonCameraComponent, NAME_None);
	Mesh1P->RelativeLocation = FVector(0.f, 0.f, -150.f);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->bOnlyOwnerSee = true;
	Mesh1P->SetIsReplicated(true);

	// Set Third Person Mesh
	GetMesh()->SetOwnerNoSee(true);
   GetMesh()->SetupAttachment(RootComponent, NAME_None);
	GetMesh()->bCastDynamicShadow = true;
	GetMesh()->CastShadow = true;
	GetMesh()->bOwnerNoSee = true;
	GetMesh()->SetIsReplicated(true);

	CharacterName = "Rade Player";
	bCanRevive = true;
}


void ARadePlayer::BeginPlay()
{
	Super::BeginPlay();

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

		// Load Inventory 
		if (bSaveInventory && Role >= ROLE_Authority)
		{
			TheInventory->LoadInventory();
		}
	}


	// Get First Person Anim Instance
	if (Mesh1P && Mesh1P->GetAnimInstance() && Cast<URadeAnimInstance>(Mesh1P->GetAnimInstance()))
		ArmsAnimInstance = Cast<URadeAnimInstance>(Mesh1P->GetAnimInstance());

	// Set Player Ref in Anim Instance
	if (ArmsAnimInstance)ArmsAnimInstance->TheCharacter = this;


	// Set Default Anim State
	Global_SetAnimID_Implementation(EAnimState::Idle_Run);

	// Set Current Camera to Default State
	CurrentCameraState = DefaultCameraState;
	UpdateComponentsVisibility();

	// Enable and start jetpack FillUp
	bCanFillJetPack = true;
	GetWorldTimerManager().SetTimer(JetPackHandle, this, &ARadePlayer::JetPackFillUp, JumpJetPack.RestoreSpeed, true);


	// Seed out Spawn Location a bit
	FVector RandomLoc = FVector(FMath::RandRange(-100, 100), FMath::RandRange(-100, 100), 0);
	SetActorLocation(GetActorLocation() + RandomLoc);

}

void ARadePlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//							Action Input


bool ARadePlayer::Action_Validate()
{
	return true;
}
void ARadePlayer::Action_Implementation()
{
	// Action Called in Blueprint
	BP_Action();
	
	// Check if player can use items
	if (bInventoryOpen && IsAnimState(EAnimState::Idle_Run)
		&& CharacterMovementComponent && CharacterMovementComponent->IsMovingOnGround())
	{
		if (TheInventory)
			TheInventory->ActionIndex(CurrentItemSelectIndex);
	}

	// Check if player is near any Pickup
	else if (currentPickup)
	{
		currentPickup->PickedUp(this);
		currentPickup = NULL;
	}

}

// Player Pressed Alt Action
bool ARadePlayer::MeleeAction_Validate(){
	return true;
}
void ARadePlayer::MeleeAction_Implementation()
{
	// Call Alt action from Blueprint
	BP_MeleeAction();

	if (TheWeapon && IsAnimState(EAnimState::Idle_Run)
		&& CharacterMovementComponent && CharacterMovementComponent->IsMovingOnGround())
	{
		TheWeapon->PreMeleeAttack();
	}
	
}

// Player Pressed FAction
bool ARadePlayer::FAction_Validate(){
	return true;
}
void ARadePlayer::FAction_Implementation()
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
bool ARadePlayer::Reload_Validate(){
	return true;
}
void ARadePlayer::Reload_Implementation()
{
	// Call Reload on Blueprint
	BP_Reload();

	// Check if Player and Weapon are in state to reload
	if (IsAnimState(EAnimState::Idle_Run)
		&& CharacterMovementComponent && CharacterMovementComponent->IsMovingOnGround()
		&& TheWeapon &&  TheWeapon->CanReload())
	{
		TheWeapon->ReloadWeaponStart();
	}
}


// Toggle HUD Inventory visibility
void ARadePlayer::ToggleInventory()
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



// Player Pressed Fire Button
bool ARadePlayer::FireStart_Validate()
{
	return true;
}

void ARadePlayer::FireStart_Implementation()
{
	// Check if player has weapn and is in state to fire weapon
	if (TheWeapon && CanShoot())
		TheWeapon->FireStart();
}

bool ARadePlayer::FireEnd_Validate(){
	return true;
}

// Player Released Fire button
void ARadePlayer::FireEnd_Implementation()
{
	if (TheWeapon)
		TheWeapon->FireEnd();
}

// Player Pressed Alt Fire
bool ARadePlayer::AltFireStart_Validate(){
	return true;
}
void ARadePlayer::AltFireStart_Implementation()
{
	if (TheWeapon)
		TheWeapon->AltFireStart();
}

// Player Released AltFire
bool ARadePlayer::AltFireEnd_Validate(){
	return true;
}
void ARadePlayer::AltFireEnd_Implementation()
{
	if (TheWeapon)
		TheWeapon->AltFireEnd();
}




// Player Pressed Jump
void ARadePlayer::Jump()
{
	if (!CharacterMovementComponent) return;

	// If Player on ground -> Simple Jump
	if (!IsAnimInAir() && CharacterMovementComponent->IsMovingOnGround())
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

bool ARadePlayer::DoubleJump_Validate(){
	return true;
}
void ARadePlayer::DoubleJump_Implementation()
{
	// Set player Velocity on server
	GetCharacterMovement()->Velocity.Z += JumpJetPack.CurrentChargePercent*JumpJetPack.PushPower;
	JumpJetPack.CurrentChargePercent = 0;
	bCanFillJetPack = false;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//								Camera		 


// Player Pressed CameraChange
void ARadePlayer::ChangeCamera()
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




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//								Inventory and Item		 



// Server set Inventory visibility 
bool ARadePlayer::SetInventoryVisible_Validate(bool bVisible)
{
	return true;
}
void ARadePlayer::SetInventoryVisible_Implementation(bool bVisible)
{
	bInventoryOpen = bVisible;
}


// Player Scrolled Mouse Wheel
void ARadePlayer::MouseScroll(float Value)
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
bool ARadePlayer::SetInventorySelectIndex_Validate(int32 index)
{
	return true;
}
void ARadePlayer::SetInventorySelectIndex_Implementation(int32 index)
{
	CurrentItemSelectIndex = index;
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//									Weapon		 


// Called to equip new Weapon
void ARadePlayer::EquipWeapon(AWeapon* NewWeaponClass)
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
void ARadePlayer::UnEquipCurrentWeapon()
{
	if (TheWeapon)
	{
		TheWeapon->UnEquipStart();
	}
	UnEquipStart();
}

// Player Started equip of new weapon
void ARadePlayer::EquipStart()
{
	if (PendingEquipWeapon && !TheWeapon)
	{
		// Set next weapon anim type
		if (ArmsAnimInstance)ArmsAnimInstance->AnimArchetype = PendingEquipWeapon->AnimArchetype;
		if (BodyAnimInstance)BodyAnimInstance->AnimArchetype = PendingEquipWeapon->AnimArchetype;

		// Set Animation state
		ServerSetAnimID(EAnimState::Equip);

		// Spawn new weapon
		TheWeapon = GetWorld()->SpawnActor<AWeapon>(PendingEquipWeapon->GetClass());
		TheWeapon->ThePlayer = this;
		TheWeapon->SetOwner(this);
		TheWeapon->EquipStart();

		FTimerHandle myHandle;
		GetWorldTimerManager().SetTimer(myHandle, this, &ARadePlayer::EquipEnd, TheWeapon->EquipTime, false);
	}
	UpdateComponentsVisibility();
}

// Called when equip is finished
void ARadePlayer::EquipEnd()
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
void ARadePlayer::UnEquipStart()
{
	// Set Animation State
	ServerSetAnimID(EAnimState::UnEquip);


	if (TheWeapon)
	{
		// Set Delay of current weapon
		FTimerHandle myHandle;
		GetWorldTimerManager().SetTimer(myHandle, this, &ARadePlayer::UnEquipEnd, TheWeapon->EquipTime, false);
	}
	else 
	{
		// Set Default Delay
		FTimerHandle myHandle;
		GetWorldTimerManager().SetTimer(myHandle, this, &ARadePlayer::UnEquipEnd, DefaultWeaponEquipDelay, false);
	}

	UpdateComponentsVisibility();

}

// Unequip Ended
void ARadePlayer::UnEquipEnd()
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
		if(ArmsAnimInstance)ArmsAnimInstance->AnimArchetype = EAnimArchetype::EmptyHand;
		if(BodyAnimInstance)BodyAnimInstance->AnimArchetype = EAnimArchetype::EmptyHand;
		ServerSetAnimID(EAnimState::Idle_Run);
	}
}

void ARadePlayer::CurrentWeaponUpdated()
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




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//									State Checking		 



// Checking if player can shoot
bool ARadePlayer::CanShoot()
{
	// Player and mesh is in the state where he can shoot
	if (IsAnimState(EAnimState::Idle_Run) || (IsAnimState(EAnimState::Jumploop) && bCanFireInAir))
		return true;
	
	return false;
}

// Check if player can sprint 
bool ARadePlayer::CanSprint()
{
	bool bReturnCanShoot = true;
	if (TheWeapon && TheWeapon->bShooting)bReturnCanShoot = false;
	if (!IsAnimState(EAnimState::Idle_Run)) bReturnCanShoot = false;
	if (CharacterMovementComponent && !CharacterMovementComponent->IsMovingOnGround())bReturnCanShoot = false;

	return bReturnCanShoot;
}

// Player landed on ground
void ARadePlayer::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	// Start Jetpack FillUp
	if (!bCanFillJetPack)
		bCanFillJetPack = true;

	if (ArmsAnimInstance || BodyAnimInstance)
		ServerSetAnimID(EAnimState::Idle_Run);
}



// Jetpack Fill Up
void ARadePlayer::JetPackFillUp()
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
void ARadePlayer::UpdateComponentsVisibility()
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


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//							Death / Revive

// Player Died , called on Server
void ARadePlayer::ServerDie()
{
	// Stop any Fire
	FireEnd();

	// Switch to third Person View on death, to look at body
	CurrentCameraState = ECameraState::TP_Camera;

	Super::ServerDie();
}
// Player Died, Called on all users
void ARadePlayer::GlobalDeath_Implementation()
{
	// Disable player input
	DisableInput(Cast<APlayerController>(Controller));

	// Event and Ragdoll
	Super::GlobalDeath_Implementation();

	// Update Visibility
	UpdateComponentsVisibility();

	// Call on Blueprint
	BP_PlayerDied();
}


// Revive Player
void ARadePlayer::ServerRevive()
{
	Super::ServerRevive();



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
	// Create a small offset from the spawn point
	if (revivePoint)
		GetRootComponent()->SetWorldLocation(revivePoint->GetActorLocation() + FVector(FMath::RandRange(-400, 400), FMath::RandRange(-400, 400), 60));

}

void ARadePlayer::GlobalRevive_Implementation()
{
	Super::GlobalRevive_Implementation();

	// Enable Input
	EnableInput(Cast<APlayerController>(Controller));
	// Set Camer to Default Camera state
	CurrentCameraState = DefaultCameraState;
	UpdateComponentsVisibility();
	Global_SetAnimArchtype_Implementation(EAnimArchetype::EmptyHand);

	BP_PlayerRevived();
}




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//									Animation 		 


void ARadePlayer::Global_SetAnimID_Implementation(EAnimState AnimID)
{
	Super::Global_SetAnimID_Implementation(AnimID);

	// Set The Value in anim instances
	if (ArmsAnimInstance)
		ArmsAnimInstance->RecieveGlobalAnimID(AnimID);

}
// Check Anim State on body or arms
bool ARadePlayer::IsAnimState(EAnimState TheAnimState)
{
	if (ArmsAnimInstance)
	{
		if (ArmsAnimInstance->IsAnimState(TheAnimState))return true;
		else return false;

	}
	else return Super::IsAnimState(TheAnimState);
}

void ARadePlayer::Global_SetAnimArchtype_Implementation(EAnimArchetype newAnimArchetype)
{
	Super::Global_SetAnimArchtype_Implementation(newAnimArchetype);

	if (ArmsAnimInstance)ArmsAnimInstance->AnimArchetype = newAnimArchetype;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//									Local Input Handling


// Bind input to events
void ARadePlayer::SetupPlayerInputComponent(UInputComponent* inputComponent)
{
	// set up gameplay key bindings
	check(inputComponent);

	inputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);

	inputComponent->BindAction("Reload", IE_Pressed, this, &ARadePlayer::Reload);

	inputComponent->BindAction("Action", IE_Pressed, this, &ARadePlayer::Action);
	inputComponent->BindAction("MeleeAction", IE_Pressed, this, &ARadePlayer::MeleeAction);
	inputComponent->BindAction("Inventory", IE_Pressed, this, &ARadePlayer::ToggleInventory);
	inputComponent->BindAction("FAction", IE_Pressed, this, &ARadePlayer::FAction);
	inputComponent->BindAction("ChangeCamera", IE_Pressed, this, &ARadePlayer::ChangeCamera);

	inputComponent->BindAction("Fire", IE_Pressed, this, &ARadePlayer::FireStart);
	inputComponent->BindAction("Fire", IE_Released, this, &ARadePlayer::FireEnd);

	inputComponent->BindAction("AltFire", IE_Pressed, this, &ARadePlayer::AltFireStart);
	inputComponent->BindAction("AltFire", IE_Released, this, &ARadePlayer::AltFireEnd);


	inputComponent->BindAxis("MouseScroll", this, &ARadePlayer::MouseScroll);

	inputComponent->BindAxis("MoveForward", this, &ARadePlayer::MoveForward);
	inputComponent->BindAxis("MoveRight", this, &ARadePlayer::MoveRight);
	inputComponent->BindAxis("Turn", this, &ARadePlayer::AddControllerYawInput);
	inputComponent->BindAxis("LookUp", this, &ARadePlayer::AddControllerPitchInput);
}



// Player Mesh Rotation after after the input
void ARadePlayer::FaceRotation(FRotator NewControlRotation, float DeltaTime)
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
void ARadePlayer::AddControllerPitchInput(float Rate){
	Super::AddControllerPitchInput(Rate*CameraMouseSensivity);
}

// Horizontal Rotation Input
void ARadePlayer::AddControllerYawInput(float Rate){
	Super::AddControllerYawInput(Rate*CameraMouseSensivity);
}

// Forward/Backward Movement Input
void ARadePlayer::MoveForward(float Value){
	if (Value != 0.0f)AddMovementInput(GetActorForwardVector(), Value);
}

// Right/Left Movement Input
void ARadePlayer::MoveRight(float Value){
	if (Value != 0.0f)AddMovementInput(GetActorRightVector(), Value);
}








////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//									Network Chat, Props and Replication





bool ARadePlayer::AddChatMessage_Validate(const FString & TheMessage)
{
	return true;
}
void ARadePlayer::AddChatMessage_Implementation(const FString & TheMessage)
{
	if (GetWorld() && GetWorld()->GetGameState<ARadeGameState>())
	{
		GetWorld()->GetGameState<ARadeGameState>()->AddNewChatMessage(TheMessage, this);
	}
}

void ARadePlayer::SetCharacterStats_Implementation(const FString & newName, FLinearColor newColor)
{
	Super::SetCharacterStats_Implementation(newName,newColor);
	if (PlayerState)
	{
		PlayerState->PlayerName = newName;
		if (Cast<ARadePlayerState>(PlayerState))Cast<ARadePlayerState>(PlayerState)->PlayerColor = newColor;
	}
}


// Replication of data
void ARadePlayer::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARadePlayer, Mesh1P);
	DOREPLIFETIME(ARadePlayer, bInventoryOpen);
	DOREPLIFETIME(ARadePlayer, CurrentItemSelectIndex);
	DOREPLIFETIME(ARadePlayer, CurrentCameraState);

	//DOREPLIFETIME(ARadePlayer, currentPickup);
	
}