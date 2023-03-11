// Copyright 2015-2023 Vagen Ayrapetyan

#include "RPlayer.h"
#include "RJump.h"

#include "RUtilLib/RUtil.h"
#include "RUtilLib/RLog.h"
#include "RUtilLib/RCheck.h"
#include "RTargetable/RTargetingComponent.h"
#include "REquipmentLib/REquipmentMgrComponent.h"
#include "RStatusLib/RStatusMgrComponent.h"
#include "RAbilityLib/RAbilityMgrComponent.h"

#include "Engine.h"
#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Net/UnrealNetwork.h"


//=============================================================================
//             Core
//=============================================================================

ARPlayer::ARPlayer ()
{
   // Set size for collision capsule
   GetCapsuleComponent ()->InitCapsuleSize (42.f, 96.0f);

   // Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement ()->bOrientRotationToMovement = true; // Character moves in the direction of input...
	GetCharacterMovement ()->RotationRate = FRotator (0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement ()->JumpZVelocity = 700.f;
	GetCharacterMovement ()->AirControl = 0.35f;
	GetCharacterMovement ()->MaxWalkSpeed = 500.f;
	GetCharacterMovement ()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement ()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoomComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT ("CameraBoom"));
	CameraBoomComponent->SetupAttachment (RootComponent);
	CameraBoomComponent->TargetArmLength = 400.0f; // The camera follows at this distance behind the character
	CameraBoomComponent->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	CameraComponent = CreateDefaultSubobject<UCameraComponent> (TEXT ("FollowCamera"));
	CameraComponent->SetupAttachment (CameraBoomComponent, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	CameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

   // --- Inventory
   EquipmentMgr->bSaveLoad = true;
   EquipmentMgr->bCheckClosestPickup = true;
   StatusMgr->bSaveLoad = true;

   // --- Targeting
   TargetingComponent = CreateDefaultSubobject<URTargetingComponent> (TEXT ("Targeting"));
   TargetingComponent->SetupAttachment (CameraComponent);

   bAutoRevive = true;
}

// Replication of data
void ARPlayer::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
   Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void ARPlayer::BeginPlay ()
{
   Super::BeginPlay ();
   SetTickGroup (TG_PostUpdateWork);

   // Get Player Controller
   if (GetController () && Cast<APlayerController>(GetController ())) {
      PlayerController = Cast<APlayerController>(GetController ());

      //Add Input Mapping Context
      if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
            ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem> (PlayerController->GetLocalPlayer ()))
		{
			Subsystem->AddMappingContext (DefaultMappingContext, 0);
		}
   }

   if (HasAuthority ()) {
      // --- Save / Load data
      // Careful with collision of 'UniqueSaveId'
      FString UniqueSaveId = GetName () + "_Player";
      Init_Save (this, UniqueSaveId);
   }
}

void ARPlayer::EndPlay (const EEndPlayReason::Type EndPlayReason)
{
   Super::EndPlay (EndPlayReason);
}

void ARPlayer::Tick (float DeltaTime)
{
   Super::Tick (DeltaTime);

   // Set Rotation
   if (PlayerController && TargetingComponent->IsTargeting ())
      PlayerController->SetControlRotation (TargetingComponent->GetTargetRotation ());
}


//=============================================================================
//             Input
//=============================================================================

// Bind input to events
void ARPlayer::SetupPlayerInputComponent (UInputComponent* PlayerInputComponent)
{
   if (!ensure (PlayerInputComponent)) return;

   // Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {

		// Moving
		EnhancedInputComponent->BindAction (IA_Move, ETriggerEvent::Triggered, this, &ARPlayer::Input_Move);

		//Looking
		EnhancedInputComponent->BindAction (IA_Look, ETriggerEvent::Triggered, this, &ARPlayer::Input_Look);

      //Jumping
		EnhancedInputComponent->BindAction (IA_Jump, ETriggerEvent::Started,   this, &ARPlayer::Input_Jump);
		EnhancedInputComponent->BindAction (IA_Jump, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

      EnhancedInputComponent->BindAction (IA_TargetFocus, ETriggerEvent::Started, this, &ARPlayer::Input_TargetFocus);
	}
}

// ---  Movement Input
void ARPlayer::Input_Move (const FInputActionValue& Value)
{
   if (!Controller) return;
   if (StatusMgr->IsDead ()) return;

	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr) {
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation (0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix (YawRotation).GetUnitAxis (EAxis::X);

		// get right vector
		const FVector RightDirection   = FRotationMatrix (YawRotation).GetUnitAxis (EAxis::Y);

		// add movement
		AddMovementInput (ForwardDirection, MovementVector.Y);
		AddMovementInput (RightDirection,   MovementVector.X);
	}
}

// --- Look
void ARPlayer::Input_Look (const FInputActionValue& Value)
{
   if (!Controller) return;
   if (StatusMgr->IsDead ()) return;

   // Input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

   // add yaw and pitch input to controller
   AddControllerYawInput   (LookAxisVector.X);
   AddControllerPitchInput (LookAxisVector.Y);

   TargetingComponent->TargetAdjust ();
}

// Player Pressed Jump
void ARPlayer::Input_Jump ()
{
   if (URAbility_Jump *JumpAbility = URUtil::GetComponent<URAbility_Jump> (this)) {
      if (JumpAbility->CanUse ()) JumpAbility->Use_Custom ();
   }

   if (URAbility_DoubleJump *JumpAbility = URUtil::GetComponent<URAbility_DoubleJump> (this)) {
      if (JumpAbility->CanUse ()) JumpAbility->Use_Server ();
   }
}

void ARPlayer::Input_TargetFocus ()
{
   TargetingComponent->TargetToggle ();
}

//=============================================================================
//                Save/Load
//=============================================================================

void ARPlayer::OnSave (FBufferArchive &SaveData)
{
   FVector  loc = GetActorLocation ();
   FRotator rot = GetActorRotation ();
   SaveData << loc << rot;
}

void ARPlayer::OnLoad (FMemoryReader &LoadData)
{
   FVector  loc;
   FRotator rot;
   LoadData << loc << rot;

   SetActorLocation (loc);
   SetActorRotation (rot);
}

//=============================================================================
//             Util
//=============================================================================

ARPlayer* ARPlayer::GetLocalRadePlayer (UObject* WorldContextObject)
{
   if (!ensure (WorldContextObject)) return nullptr;
   UWorld* World = GEngine->GetWorldFromContextObject (WorldContextObject, EGetWorldErrorMode::ReturnNull);
   if (World) {
      APlayerController *localPC = World->GetFirstPlayerController ();
      if (localPC) {
         APawn *localPawn = localPC->GetPawn ();
         if (localPawn && Cast<ARPlayer>(localPawn))
            return Cast<ARPlayer>(localPawn);
      }
   }
   return nullptr;
}

