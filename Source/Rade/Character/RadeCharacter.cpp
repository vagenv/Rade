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


	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = PCIP.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	Mesh1P->AttachParent = FirstPersonCameraComponent;
	Mesh1P->RelativeLocation = FVector(0.f, 0.f, -150.f);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->bOnlyOwnerSee = true;
	Mesh1P->SetIsReplicated(true);


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

	if (Role>=ROLE_Authority)
	{
		SetActorLocation(GetActorLocation() + FVector(FMath::RandRange(-400, 400), FMath::RandRange(-400, -400), 0));
	}


	if (GetMesh())
	{
		Mesh_InGameRelativeLoc = GetMesh()->RelativeLocation;
		Mesh_InGameRelativeRot = GetMesh()->RelativeRotation;
	}
	
	if (GetController() && Cast<ARadePC>(GetController()))
	{
		ThePC = Cast<ARadePC>(GetController());
	}
	if (ThePC && ThePC->GetHUD() && Cast<ABaseHUD>(ThePC->GetHUD()))TheHUD = Cast<ABaseHUD>(ThePC->GetHUD());

	if (TheInventory)TheInventory->ThePlayer = this;
	if (bSaveInventory && Role >= ROLE_Authority &&  TheInventory )
	{
		TheInventory->LoadInventory();
	}

	if (!PlayerMovementComponent)
	{
		PlayerMovementComponent = Cast<UCharacterMovementComponent>(GetMovementComponent());
	}
	if (!ArmsAnimInstance && Mesh1P && Mesh1P->GetAnimInstance() && Cast<URadeAnimInstance>(Mesh1P->GetAnimInstance()))ArmsAnimInstance = Cast<URadeAnimInstance>(Mesh1P->GetAnimInstance());
	if (!BodyAnimInstance && GetMesh() && GetMesh()->GetAnimInstance() && Cast<URadeAnimInstance>(GetMesh()->GetAnimInstance()))BodyAnimInstance = Cast<URadeAnimInstance>(GetMesh()->GetAnimInstance());


	if (ArmsAnimInstance)ArmsAnimInstance->ThePlayer = this;


	ServerSetAnimID(EAnimState::Idle_Run);


	CurrentCameraState = DefaultCameraState;
	UpdateComponentsVisibility();


	bCanFillJetPack = true;
	GetWorldTimerManager().SetTimer(JetPackHandle, this, &ARadeCharacter::JetPackFillUp, JumpJetPack.RestoreSpeed, true);

}


void ARadeCharacter::ResetAnimInstances()
{
	if (!ArmsAnimInstance && Mesh1P && Mesh1P->GetAnimInstance() && Cast<URadeAnimInstance>(Mesh1P->GetAnimInstance()))
	{
		ArmsAnimInstance = Cast<URadeAnimInstance>(Mesh1P->GetAnimInstance());
	}
	if (!BodyAnimInstance && GetMesh() && GetMesh()->GetAnimInstance() && Cast<URadeAnimInstance>(GetMesh()->GetAnimInstance()))
	{
		BodyAnimInstance = Cast<URadeAnimInstance>(GetMesh()->GetAnimInstance());
	}
}


bool ARadeCharacter::Action_Validate()
{
	return true;
}
void ARadeCharacter::Action_Implementation()
{
	BP_Action();
	

	if (bInventoryOpen && ArmsAnimInstance && ArmsAnimInstance->IsAnimState(EAnimState::Idle_Run) 
		&& PlayerMovementComponent&& PlayerMovementComponent->IsMovingOnGround())
	{
		if (TheInventory)
			TheInventory->ActionIndex(CurrentItemSelectIndex);
	}
	else 
	{
		if (currentPickup)
		{
			currentPickup->PickedUp(this);
			currentPickup = NULL;
		}
	}

}

bool ARadeCharacter::AltAction_Validate()
{
	return true;
}
void ARadeCharacter::AltAction_Implementation()
{
	BP_AltAction();



	// Alt action

	if (ThePC)
	{

	}	//printr(ThePC->GetControlRotation().ToString());


}

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

bool ARadeCharacter::FAction_Validate()
{
	return true;
}
void ARadeCharacter::FAction_Implementation()
{
	BP_FAction();
	if (TheInventory && bInventoryOpen)
	{
		TheInventory->ThrowOutIndex(CurrentItemSelectIndex);
	}
}

bool ARadeCharacter::Reload_Validate()
{
	return true;
}
void ARadeCharacter::Reload_Implementation()
{
	// Reload weapon
	BP_Reload();
	if (ArmsAnimInstance && ArmsAnimInstance->IsAnimState(EAnimState::Idle_Run) 
		&& PlayerMovementComponent && PlayerMovementComponent->IsMovingOnGround() 
		&& TheWeapon &&  TheWeapon->CanReload())
	{
		TheWeapon->ReloadWeaponStart();
	}
}



// Toggle hud inventory visibility
void ARadeCharacter::ToggleInventory()
{
	BP_ToggleInventory();

	if (TheHUD)
	{
		TheHUD->ToggleInventory();

		if (CurrentItemSelectIndex != TheHUD->CurrentItemSelectIndex) 
			SetInventorySelectIndex(TheHUD->CurrentItemSelectIndex);

		SetInventoryVisible(TheHUD->bInventoryOpen);
	}	
}
bool ARadeCharacter::SetInventoryVisible_Validate(bool bVisible)
{
	return true;
}
void ARadeCharacter::SetInventoryVisible_Implementation(bool bVisible)
{
	bInventoryOpen = bVisible;
}




void ARadeCharacter::MouseScroll(float Value)
{
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

		if (CurrentItemSelectIndex != TheHUD->CurrentItemSelectIndex)
			SetInventorySelectIndex(TheHUD->CurrentItemSelectIndex);
	}
}

bool ARadeCharacter::SetInventorySelectIndex_Validate(int32 index)
{
	return true;
}
void ARadeCharacter::SetInventorySelectIndex_Implementation(int32 index)
{
	CurrentItemSelectIndex = index;
}



void ARadeCharacter::EquipWeapon(AWeapon* NewWeaponClass)
{
	if (!NewWeaponClass)return;

	// Unequip Current weapon
	if (TheWeapon)
	{
		TheWeapon->UnEquipStart();
	}

	// Set next weapon
	PendingEquipWeapon = NewWeaponClass;

	// Start Player unequip anim even if there is no current weapon
	UnEquipStart();

}
void ARadeCharacter::UnEquipCurrentWeapon()
{
	if (TheWeapon)
	{
		TheWeapon->UnEquipStart();
	}
	UnEquipStart();
}

void ARadeCharacter::EquipStart()
{
	if (PendingEquipWeapon && !TheWeapon)
	{
		// Set next weapon anim type
		ArmsAnimInstance->AnimArchetype = PendingEquipWeapon->AnimArchetype;
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
void ARadeCharacter::EquipEnd()
{
	ServerSetAnimID(EAnimState::Idle_Run);


	if (PendingEquipWeapon!=NULL)PendingEquipWeapon = NULL;
	if (TheHUD)
	{
		TheHUD->BP_WeaponUpdated();
	}

}
void ARadeCharacter::UnEquipStart()
{

	ServerSetAnimID(EAnimState::UnEquip);

	if (TheWeapon)
	{
		FTimerHandle myHandle;
		GetWorldTimerManager().SetTimer(myHandle, this, &ARadeCharacter::UnEquipEnd, TheWeapon->EquipTime, false);
	}
	else 
	{
		FTimerHandle myHandle;
		GetWorldTimerManager().SetTimer(myHandle, this, &ARadeCharacter::UnEquipEnd, DefaultWeaponEquipDelay, false);
	}

	UpdateComponentsVisibility();

}
void ARadeCharacter::UnEquipEnd()
{
	if (TheWeapon)
	{
		TheWeapon->Destroy();
		TheWeapon = NULL;
	}
	if (TheHUD)
	{
		TheHUD->BP_WeaponUpdated();
	}

	if (PendingEquipWeapon)
	{
		EquipStart();
	}
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
		{
			ArmsAnimInstance->AnimArchetype = TheWeapon->AnimArchetype;
		}
		else 
		{
			ArmsAnimInstance->AnimArchetype = EAnimArchetype::EmptyHand;
		}
	}
	if (TheHUD)
	{
		TheHUD->BP_WeaponUpdated();
	}

	UpdateComponentsVisibility();
}


// Fire Pressed


bool ARadeCharacter::FireStart_Validate()
{
	return true;
}

void ARadeCharacter::FireStart_Implementation()
{

	if (TheWeapon && CanShoot())
	{
		TheWeapon->FireStart();
	}
}


bool ARadeCharacter::FireEnd_Validate()
{
	return true;
}

void ARadeCharacter::FireEnd_Implementation()
{
	if (TheWeapon)
	{
	//	print("Fire Released");
		TheWeapon->FireEnd();
	}
}

bool ARadeCharacter::AltFireStart_Validate()
{
	return true;
}
void ARadeCharacter::AltFireStart_Implementation()
{
	if (TheWeapon)
	{
		TheWeapon->AltFireStart();
	}
}

bool ARadeCharacter::AltFireEnd_Validate()
{
	return true;
}
void ARadeCharacter::AltFireEnd_Implementation()
{
	if (TheWeapon)
	{
		TheWeapon->AltFireEnd();
	}
}



// Checking if player can shoot
bool ARadeCharacter::CanShoot()
{
	// Player and mesh is in the state where he can shoot
	if (ArmsAnimInstance && (ArmsAnimInstance->IsAnimState(EAnimState::Idle_Run) || (ArmsAnimInstance->IsAnimState(EAnimState::Jumploop) && bCanFireInAir )))
		return true;
	
	
	
	return false;
}

// Check if player can sprint 
bool ARadeCharacter::CanSprint()
{
	bool bReturnCanShoot = true;
	if (TheWeapon && TheWeapon->bShooting)bReturnCanShoot = false;
	if (ArmsAnimInstance && !ArmsAnimInstance->IsAnimState(EAnimState::Idle_Run)) bReturnCanShoot = false;
	if (PlayerMovementComponent && !PlayerMovementComponent->IsMovingOnGround())bReturnCanShoot = false;

	return bReturnCanShoot;
}

// Player landed on ground
void ARadeCharacter::Landed(const FHitResult& Hit)
{
	
	Super::Landed(Hit);
	if (!bCanFillJetPack)
	{
		bCanFillJetPack = true;
	}

	if (ArmsAnimInstance && BodyAnimInstance)
	{
		ArmsAnimInstance->bInAir = false;
		BodyAnimInstance->bInAir = false;
		ServerSetAnimID(EAnimState::Idle_Run);
	}

}



//////////////// Network Anim

bool ARadeCharacter::ServerSetAnimID_Validate(EAnimState AnimID)
{
	return true;
}
void ARadeCharacter::ServerSetAnimID_Implementation(EAnimState AnimID)
{
	//print("Server Anim ID : "+FString::FromInt(AnimID));
	Global_SetAnimID(AnimID);

	//print("Play Server Anim");
}

void ARadeCharacter::Global_SetAnimID_Implementation(EAnimState AnimID)
{
	if (ArmsAnimInstance)
	{
		ArmsAnimInstance->RecieveGlobalAnimID(AnimID);
		//print("Print global Anim");
	}
}


void ARadeCharacter::Jump()
{
	if (!ArmsAnimInstance || !BodyAnimInstance || !PlayerMovementComponent) return;

	
	//print("Jump pressed");
	if (!ArmsAnimInstance->IsInAir() && PlayerMovementComponent->IsMovingOnGround())
	{
		ACharacter::Jump();
		ServerSetAnimID(EAnimState::JumpStart);
		
	}
	else if (bJetPackEnabled && bCanFillJetPack && JumpJetPack.CurrentChargePercent>JumpJetPack.MinUseablePercent)
	{
		DoubleJump();
		JumpJetPack.CurrentChargePercent = 0;
		bCanFillJetPack = false;
	}
}

bool ARadeCharacter::DoubleJump_Validate()
{
	return true;

	//if (bJetPackEnabled && bCanFillJetPack && JumpJetPack.CurrentChargePercent>JumpJetPack.MinUseablePercent) return true;
}
void ARadeCharacter::DoubleJump_Implementation()
{	
	GetCharacterMovement()->Velocity.Z += JumpJetPack.CurrentChargePercent*JumpJetPack.PushPower;
	JumpJetPack.CurrentChargePercent = 0;
	bCanFillJetPack = false;
}

void ARadeCharacter::JetPackFillUp()
{
	if (bCanFillJetPack)
	{
		if (JumpJetPack.CurrentChargePercent<100)
		{

			JumpJetPack.CurrentChargePercent += JumpJetPack.RestorePower;
			if (JumpJetPack.CurrentChargePercent>100)
			{
				JumpJetPack.CurrentChargePercent = 100;
			}
		}
	}
}

void ARadeCharacter::UpdateComponentsVisibility()
{

	if (CurrentCameraState==ECameraState::FP_Camera)
	{
		//printg("First person");
		if (ThirdPersonCameraComponent)ThirdPersonCameraComponent->Deactivate();
		if (FirstPersonCameraComponent)FirstPersonCameraComponent->Activate();
		if (GetMesh())
		{
			GetMesh()->SetOwnerNoSee(true);
		}
		if (Mesh1P)
		{
			Mesh1P->SetVisibility(true);
		}
		if (TheWeapon)
		{
			if (TheWeapon->Mesh1P)
			{
				TheWeapon->Mesh1P->SetVisibility(true);
				
			}
			if (TheWeapon->Mesh3P)
			{
				TheWeapon->Mesh3P->SetOwnerNoSee(true);
			}
		}
	}

	// Currently Third Person Camera
	else if (CurrentCameraState == ECameraState::TP_Camera)
	{
		if (ThirdPersonCameraComponent)ThirdPersonCameraComponent->Activate();
		if (FirstPersonCameraComponent)FirstPersonCameraComponent->Deactivate();

		if (GetMesh())
		{
			GetMesh()->SetOwnerNoSee(false);
		}
		if (Mesh1P)
		{
			Mesh1P->SetVisibility(false);
		}
		if (TheWeapon)
		{
			if (TheWeapon->Mesh1P)
			{
				TheWeapon->Mesh1P->SetVisibility(false);
			}
			if (TheWeapon->Mesh3P)
			{
				TheWeapon->Mesh3P->SetOwnerNoSee(false);
			}
		}
	}

}


void ARadeCharacter::ServerDie()
{
	FireEnd();

	bDead = true;
	CurrentCameraState = ECameraState::TP_Camera;



	Super::ServerDie();
	//UpdateComponentsVisibility();
}
void ARadeCharacter::GlobalDeath_Implementation()
{

	if (GetMesh())
	{
		Mesh_InGameRelativeLoc = GetMesh()->RelativeLocation;
		Mesh_InGameRelativeRot = GetMesh()->RelativeRotation;
	}

	
	DisableInput(Cast<APlayerController>(Controller));

	if (bCanRevive)
	{
		FTimerHandle MyHandle;
		GetWorldTimerManager().SetTimer(MyHandle, this, &ARadeCharacter::Revive, ReviveTime, false);
	}

	// Event and Ragdoll
	Super::GlobalDeath_Implementation();
	UpdateComponentsVisibility();
	BP_PlayerDied();
}



void ARadeCharacter::Revive()
{
	//printr("Local Revive");
	EnableInput(Cast<APlayerController>(Controller));

	CurrentCameraState = DefaultCameraState;

	Health = MaxHealth/2;
	bDead = false;

	
	//GetCapsuleComponent()->BodyInstance.SetCollisionProfileName("Pawn");

	if (GetMesh())
	{
		//GetMesh()->SetVisibility(true);
		GetMesh()->SetSimulatePhysics(false);
		GetMesh()->AttachTo(RootComponent);
		GetMesh()->RelativeLocation = Mesh_InGameRelativeLoc;
		GetMesh()->RelativeRotation = Mesh_InGameRelativeRot;
		GetMesh()->BodyInstance.SetCollisionProfileName("Pawn");
	}

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

	if (revivePoint)
		GetRootComponent()->SetWorldLocation(revivePoint->GetActorLocation() + FVector(FMath::RandRange(-400, 400), FMath::RandRange(-400, 400), 60));
	GetMovementComponent()->Activate();

	BP_PlayerRevived();
}



// Event Asignment
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

// Vertical Input
void ARadeCharacter::AddControllerPitchInput(float Rate){
	Super::AddControllerPitchInput(Rate*CameraMouseSensivity);
}

// Horizontal Input
void ARadeCharacter::AddControllerYawInput(float Rate){
	Super::AddControllerYawInput(Rate*CameraMouseSensivity);
}

void ARadeCharacter::MoveForward(float Value){
	if (Value != 0.0f)AddMovementInput(GetActorForwardVector(), Value);
}

void ARadeCharacter::MoveRight(float Value){
	if (Value != 0.0f)AddMovementInput(GetActorRightVector(), Value);
}

void ARadeCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARadeCharacter, Mesh1P);
	DOREPLIFETIME(ARadeCharacter, CurrentItemSelectIndex);
	DOREPLIFETIME(ARadeCharacter, bInventoryOpen);
	DOREPLIFETIME(ARadeCharacter, Mesh1P);
	DOREPLIFETIME(ARadeCharacter, CurrentCameraState);


}