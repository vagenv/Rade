// Copyright 2015-2023 Vagen Ayrapetyan

#include "RPlayer.h"

#include "RUtilLib/RLog.h"
#include "RUtilLib/RCheck.h"
#include "RSaveLib/RSaveMgr.h"
#include "REquipmentLib/REquipmentMgrComponent.h"
#include "RStatusLib/RStatusMgrComponent.h"

#include "RJetpackComponent.h"

#include "Engine.h"

#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

#include "Net/UnrealNetwork.h"

//=============================================================================
//             Core
//=============================================================================

ARPlayer::ARPlayer()
{

   // Set size for collision capsule
   GetCapsuleComponent()->InitCapsuleSize (42.f, 96.0f);

   // --- Camera
   // Create a CameraComponent
   FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
   FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent(), NAME_None);
   FirstPersonCameraComponent->SetRelativeLocation(FVector(0, 0, 64.f)); // Position the camera

   // Create a camera boom (pulls in towards the player if there is a collision)
   ThirdPersonCameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
   ThirdPersonCameraBoom->SetupAttachment(RootComponent, NAME_None);
   ThirdPersonCameraBoom->TargetArmLength = 150;
   ThirdPersonCameraBoom->SetRelativeLocation (FVector(0,50,100));
   ThirdPersonCameraBoom->bUsePawnControlRotation = true;

   // Create a follow camera
   ThirdPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("PlayerCamera"));
   ThirdPersonCameraComponent->SetupAttachment(ThirdPersonCameraBoom, USpringArmComponent::SocketName);

   // --- Mesh
   // Set First Person Mesh
   Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
   Mesh1P->SetOnlyOwnerSee(true);
   Mesh1P->SetupAttachment(FirstPersonCameraComponent, NAME_None);
   Mesh1P->SetRelativeLocation(FVector(0.f, 0.f, -150.f));
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

   Jetpack = CreateDefaultSubobject<URJetpackComponent> (TEXT("Jetpack"));

   // --- Inventory
   EquipmentMgr->bSaveLoad = true;
   EquipmentMgr->bCheckClosestPickup = true;
   StatusMgr->bSaveLoad = true;

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

   // Get Player Controller
   if (GetController() && Cast<APlayerController>(GetController())) {
      PlayerController = Cast<APlayerController>(GetController());

      //Add Input Mapping Context
      if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
   }

   // // Get First Person Anim Instance
   // if (Mesh1P && Mesh1P->GetAnimInstance() && Cast<URAnimInstance>(Mesh1P->GetAnimInstance()))
   //    ArmsAnimInstance = Cast<URAnimInstance>(Mesh1P->GetAnimInstance());

   // --- Setup Camera
   // Set Current Camera to Default State
   CurrentCameraState = DefaultCameraState;
   UpdateComponentsVisibility();


   if (HasAuthority ()) {
      // --- Save / Load data
      FRSaveEvent SavedDelegate;
      SavedDelegate.AddDynamic (this, &ARPlayer::OnSave);
      URSaveMgr::OnSave (GetWorld (), SavedDelegate);

      FRSaveEvent LoadedDelegate;
      LoadedDelegate.AddDynamic (this, &ARPlayer::OnLoad);
      URSaveMgr::OnLoad (GetWorld (), LoadedDelegate);
   }

   // --- Seed out Spawn Location a bit
   //FVector RandomLoc = FVector(FMath::RandRange(-100, 100), FMath::RandRange(-100, 100), 0);
   //SetActorLocation(GetActorLocation() + RandomLoc);
}

void ARPlayer::EndPlay (const EEndPlayReason::Type EndPlayReason)
{
   Super::EndPlay (EndPlayReason);
}

void ARPlayer::Tick (float DeltaTime)
{
   Super::Tick (DeltaTime);
}

void ARPlayer::Input_SaveGame ()
{
   bool res = URSaveMgr::SaveASync (GetWorld ());
   if (!res) {
      R_LOG ("Save failed");
      return;
   }
}

void ARPlayer::Input_LoadGame ()
{
   bool res = URSaveMgr::LoadASync (GetWorld ());
   if (!res) {
      R_LOG ("Load failed");
      return;
   }
}

void ARPlayer::OnSave ()
{
   R_RETURN_IF_NOT_ADMIN;
   FString SaveUniqueId = GetOwner ()->GetName () + "_Player";
   // --- Save player location
   FVector  loc = GetActorLocation ();
   FRotator rot = GetActorRotation ();

   FBufferArchive ToBinary;
   ToBinary << loc << rot;
   if (!URSaveMgr::Set (GetWorld (), SaveUniqueId, ToBinary)) {
      R_LOG_PRINTF ("Failed to save [%s] Location.", *SaveUniqueId);
   }
}

void ARPlayer::OnLoad ()
{
   R_RETURN_IF_NOT_ADMIN;
   FString SaveUniqueId = GetOwner ()->GetName () + "_Player";

   // Get binary data from save file
   TArray<uint8> BinaryArray;
   if (!URSaveMgr::Get (GetWorld (), SaveUniqueId, BinaryArray)) {
      R_LOG_PRINTF ("Failed to load [%s] Location.", *SaveUniqueId);
      return;
   }

   FVector  loc;
   FRotator rot;
   FMemoryReader FromBinary = FMemoryReader (BinaryArray, true);
   FromBinary.Seek (0);
   FromBinary << loc << rot;

   SetActorLocation (loc);
   SetActorRotation (rot);
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

      EnhancedInputComponent->BindAction (IA_ChangeCamera, ETriggerEvent::Started, this, &ARPlayer::Input_ChangeCamera);
      EnhancedInputComponent->BindAction (IA_Action,       ETriggerEvent::Started, this, &ARPlayer::Input_Action);
      EnhancedInputComponent->BindAction (IA_AltAction,    ETriggerEvent::Started, this, &ARPlayer::Input_AltAction);

      EnhancedInputComponent->BindAction (IA_Save, ETriggerEvent::Started, this, &ARPlayer::Input_SaveGame);
      EnhancedInputComponent->BindAction (IA_Load, ETriggerEvent::Started, this, &ARPlayer::Input_LoadGame);
	}
}

// ---  Movement Input

void ARPlayer::Input_Move (const FInputActionValue& Value)
{
   if (StatusMgr->bDead) return;

	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr) {
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation ();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix (YawRotation).GetUnitAxis (EAxis::X);

		// get right vector
		const FVector RightDirection = FRotationMatrix (YawRotation).GetUnitAxis (EAxis::Y);

		// add movement
		AddMovementInput (ForwardDirection, MovementVector.Y);
		AddMovementInput (RightDirection,   MovementVector.X);
	}
}

void ARPlayer::Input_Look (const FInputActionValue& Value)
{
   if (StatusMgr->bDead) return;
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr) {
		// add yaw and pitch input to controller
		AddControllerYawInput (LookAxisVector.X);
		AddControllerPitchInput (LookAxisVector.Y);
	}
}

// Player Mesh Rotation after after the input
void ARPlayer::FaceRotation (FRotator NewControlRotation, float DeltaTime)
{
   Super::FaceRotation (NewControlRotation,DeltaTime);
   if (FirstPersonCameraComponent) {
      FRotator rot = FirstPersonCameraComponent->GetComponentRotation();
      rot.Pitch = NewControlRotation.Pitch;
      FirstPersonCameraComponent->SetWorldRotation (rot);
   }
}

// Player Pressed Jump
void ARPlayer::Input_Jump ()
{
   if (StatusMgr->bDead) return;
   Jetpack->Use ();
   Super::Jump ();
}

// Player Pressed CameraChange
void ARPlayer::Input_ChangeCamera ()
{
   if (StatusMgr->bDead) return;

   // Change Camera
   if (DefaultCameraState == ECameraState::FP_Camera) {
      DefaultCameraState = ECameraState::TP_Camera;
      CurrentCameraState = DefaultCameraState;
      UpdateComponentsVisibility ();
   } else if (DefaultCameraState == ECameraState::TP_Camera) {
      DefaultCameraState = ECameraState::FP_Camera;
      CurrentCameraState = DefaultCameraState;
      UpdateComponentsVisibility ();
   }
   Input_OnChangeCamera.Broadcast ();
}

void ARPlayer::Input_Action ()
{
   Input_OnAction.Broadcast ();
}

void ARPlayer::Input_AltAction ()
{
   Input_OnAltAction.Broadcast ();
}

//=============================================================================
//                           State Checking
//=============================================================================

/*
// Checking if player can shoot
bool ARPlayer::CanShoot()
{
   // Player and mesh is in the state where he can shoot
   if (   IsAnimState(EAnimState::Idle_Run)
      || (IsAnimState(EAnimState::Jumploop) && bCanFireInAir))
      return true;

   return false;
}

// Check if player can sprint
bool ARPlayer::CanSprint()
{
   bool bReturnCanShoot = true;
   if (TheWeapon && TheWeapon->bShooting)  bReturnCanShoot = false;
   if (!IsAnimState(EAnimState::Idle_Run)) bReturnCanShoot = false;
   if (  CharacterMovementComponent
      && !CharacterMovementComponent->IsMovingOnGround())
      bReturnCanShoot = false;

   return bReturnCanShoot;
}
*/

// Update First Person and Third Person Components visibility
void ARPlayer::UpdateComponentsVisibility ()
{
   if (CurrentCameraState == ECameraState::FP_Camera) {
      if (ThirdPersonCameraComponent) ThirdPersonCameraComponent->Deactivate();
      if (FirstPersonCameraComponent) FirstPersonCameraComponent->Activate();

      if (GetMesh()) GetMesh()->SetOwnerNoSee(true);

      if (Mesh1P) Mesh1P->SetVisibility(true);

      /*
      if (TheWeapon) {
         if (TheWeapon->Mesh1P) TheWeapon->Mesh1P->SetVisibility(true);
         if (TheWeapon->Mesh3P) TheWeapon->Mesh3P->SetOwnerNoSee(true);
      }
      */

   // Currently Third Person Camera
   } else if (CurrentCameraState == ECameraState::TP_Camera) {

      if (ThirdPersonCameraComponent) ThirdPersonCameraComponent->Activate();
      if (FirstPersonCameraComponent) FirstPersonCameraComponent->Deactivate();

      if (GetMesh()) GetMesh()->SetOwnerNoSee(false);
      if (Mesh1P)    Mesh1P->SetVisibility(false);

      /*
      if (TheWeapon) {
         if (TheWeapon->Mesh1P) TheWeapon->Mesh1P->SetVisibility(false);
         if (TheWeapon->Mesh3P) TheWeapon->Mesh3P->SetOwnerNoSee(false);
      }
      */
   }
}


//=============================================================================
//                   Damage/Death/Revive
//=============================================================================
/*
void ARPlayer::Die (AActor *DeathCauser, AController* EventInstigator)
{
   Super::Die (DeathCauser, EventInstigator);
}
*/

/*
// Player Died , called on Server
void ARPlayer::ServerDie()
{
   // Stop any Fire
   FireEnd();

   // Switch to third Person View on death, to look at body
   CurrentCameraState = ECameraState::TP_Camera;

   Super::ServerDie();

   APlayerState *playerState = GetPlayerState();
   if (GetPlayerState() && Cast<ARPlayerState>(playerState))
   {
      Cast<ARPlayerState>(playerState)->DeathCount++;
   }
   if (  EventInstigator
      && EventInstigator->GetPawn()
      && EventInstigator->GetPawn()->GetPlayerState()
      && Cast<ARPlayerState>(EventInstigator->GetPawn()->GetPlayerState()))
   {
      Cast<ARPlayerState>(EventInstigator->GetPawn()->GetPlayerState ())->KillCount++;
   }

}
// Player Died, Called on all users
void ARPlayer::GlobalDeath_Implementation()
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
void ARPlayer::ServerRevive()
{
   Super::ServerRevive();

   // Find The Closest Revive Point
   TActorIterator<APlayerStart> p(GetWorld());
   APlayerStart* revivePoint = *p;

   for (TActorIterator<APlayerStart> ActorItr(GetWorld()); ActorItr; ++ActorItr) {
      if (revivePoint
         &&   FVector::Dist (revivePoint->GetActorLocation(), GetActorLocation())
            > FVector::Dist (ActorItr->GetActorLocation(),    GetActorLocation()))
      {
         revivePoint = *ActorItr;
      }
   }
   // Create a small offset from the spawn point
   if (revivePoint)
      GetRootComponent()->SetWorldLocation(revivePoint->GetActorLocation() + FVector(FMath::RandRange(-400, 400),
                                           FMath::RandRange(-400, 400), 60));

}

void ARPlayer::GlobalRevive_Implementation()
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

*/

//=============================================================================
//             Network Chat, Props and Replication
//=============================================================================

/*
bool ARPlayer::AddChatMessage_Validate(const FString & TheMessage)
{
   return true;
}
void ARPlayer::AddChatMessage_Implementation(const FString & TheMessage)
{
   if (GetWorld() && GetWorld()->GetGameState<ARadeGameState>()) {
      GetWorld()->GetGameState<ARadeGameState>()->AddNewChatMessage(TheMessage, this);
   }
}

void ARPlayer::SetCharacterStats_Implementation(const FString &newName, FLinearColor newColor)
{
   Super::SetCharacterStats_Implementation(newName,newColor);
   if (GetPlayerState()) {
      GetPlayerState()->SetPlayerName (newName);
      if (Cast<ARPlayerState>(GetPlayerState()))Cast<ARPlayerState>(GetPlayerState())->PlayerColor = newColor;
   }
}

*/

//=============================================================================
//       Util
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




