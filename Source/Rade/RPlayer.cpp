// Copyright 2015-2023 Vagen Ayrapetyan

#include "RPlayer.h"
#include "RJump.h"

#include "RUtilLib/RUtil.h"
#include "RUtilLib/RLog.h"
#include "RUtilLib/RCheck.h"
#include "RTargetable/RTargetableComponent.h"
#include "RTargetable/RTargetableMgr.h"
#include "RSaveLib/RSaveMgr.h"
#include "REquipmentLib/REquipmentMgrComponent.h"
#include "RStatusLib/RStatusMgrComponent.h"
#include "RAbilityLib/RAbilityMgrComponent.h"

#include "Engine.h"
#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Net/UnrealNetwork.h"

#include "DrawDebugHelpers.h"

//=============================================================================
//             Core
//=============================================================================

ARPlayer::ARPlayer ()
{

   // Set size for collision capsule
   GetCapsuleComponent ()->InitCapsuleSize (42.f, 96.0f);

   // --- Camera
   // Create a CameraComponent
   FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent> (TEXT ("FirstPersonCamera"));
   FirstPersonCameraComponent->SetupAttachment (GetCapsuleComponent (), NAME_None);
   FirstPersonCameraComponent->SetRelativeLocation (FVector(0, 0, 64.f)); // Position the camera

   // Create a camera boom (pulls in towards the player if there is a collision)
   ThirdPersonCameraBoom = CreateDefaultSubobject<USpringArmComponent> (TEXT ("CameraBoom"));
   ThirdPersonCameraBoom->SetupAttachment (RootComponent, NAME_None);
   ThirdPersonCameraBoom->TargetArmLength = 150;
   ThirdPersonCameraBoom->SetRelativeLocation (FVector(0,50,100));
   ThirdPersonCameraBoom->bUsePawnControlRotation = true;

   // Create a follow camera
   ThirdPersonCameraComponent = CreateDefaultSubobject<UCameraComponent> (TEXT ("PlayerCamera"));
   ThirdPersonCameraComponent->SetupAttachment (ThirdPersonCameraBoom, USpringArmComponent::SocketName);

   // --- Mesh
   // Set First Person Mesh
   Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent> (TEXT ("CharacterMesh1P"));
   Mesh1P->SetOnlyOwnerSee (true);
   Mesh1P->SetupAttachment (FirstPersonCameraComponent, NAME_None);
   Mesh1P->SetRelativeLocation (FVector (0.f, 0.f, -150.f));
   Mesh1P->bCastDynamicShadow = false;
   Mesh1P->CastShadow = false;
   Mesh1P->bOnlyOwnerSee = true;
   Mesh1P->SetIsReplicated (true);

   // Set Third Person Mesh
   GetMesh()->SetOwnerNoSee (true);
   GetMesh()->SetupAttachment (RootComponent, NAME_None);
   GetMesh()->bCastDynamicShadow = true;
   GetMesh()->CastShadow = true;
   GetMesh()->bOwnerNoSee = true;
   GetMesh()->SetIsReplicated (true);

   // --- Inventory
   EquipmentMgr->bSaveLoad = true;
   EquipmentMgr->bCheckClosestPickup = true;
   StatusMgr->bSaveLoad = true;

   // --- Targetable
   FRichCurve* TargetAngleToLerpPowerData = TargetAngleToLerpPower.GetRichCurve ();
   TargetAngleToLerpPowerData->AddKey (0,   0);
   TargetAngleToLerpPowerData->AddKey (1,  20);
   TargetAngleToLerpPowerData->AddKey (5,  10);
   TargetAngleToLerpPowerData->AddKey (20,  5);
   TargetAngleToLerpPowerData->AddKey (40,  1);

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
   if (GetController() && Cast<APlayerController>(GetController ())) {
      PlayerController = Cast<APlayerController>(GetController ());

      //Add Input Mapping Context
      if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
   }

   // --- Setup Camera
   // Set Current Camera to Default State
   CurrentCameraState = DefaultCameraState;
   UpdateComponentsVisibility ();

   if (HasAuthority ()) {
      // --- Save / Load data
      // Careful with collision of 'UniqueSaveId'
      FString UniqueSaveId = GetName () + "_Player";
      Init_Save (this, UniqueSaveId);
   }

   TargetMgr = URTargetableMgr::GetInstance (this);

   GetWorldTimerManager().SetTimer (TargetCheckHandle, this, &ARPlayer::TargetCheck, 1, true);
}

void ARPlayer::EndPlay (const EEndPlayReason::Type EndPlayReason)
{
   Super::EndPlay (EndPlayReason);
}

void ARPlayer::Tick (float DeltaTime)
{
   Super::Tick (DeltaTime);
   TargetingTick (DeltaTime);
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

      EnhancedInputComponent->BindAction (IA_ChangeCamera, ETriggerEvent::Started, this, &ARPlayer::TargetSearch);
      EnhancedInputComponent->BindAction (IA_Action,       ETriggerEvent::Started, this, &ARPlayer::Input_Action);
      EnhancedInputComponent->BindAction (IA_AltAction,    ETriggerEvent::Started, this, &ARPlayer::Input_AltAction);
	}
}

// ---  Movement Input

void ARPlayer::Input_Move (const FInputActionValue& Value)
{
   if (StatusMgr->IsDead ()) return;

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
   if (StatusMgr->IsDead ()) return;
   if (TargetCurrent) return;

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
   Super::FaceRotation (NewControlRotation, DeltaTime);
   if (FirstPersonCameraComponent) {
      FRotator rot = FirstPersonCameraComponent->GetComponentRotation ();
      rot.Pitch = NewControlRotation.Pitch;
      FirstPersonCameraComponent->SetWorldRotation (rot);
   }
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

// Player Pressed CameraChange
void ARPlayer::Input_ChangeCamera ()
{
   if (StatusMgr->IsDead ()) return;

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
//             Input
//=============================================================================


void ARPlayer::TargetingTick (float DeltaTime)
{
   if (TargetCurrent) {

      // --- Collect values
      FVector  CameraLocation = FirstPersonCameraComponent->GetComponentLocation ();
      FRotator CameraRotation = FirstPersonCameraComponent->GetComponentRotation ();
      FVector  CameraDir      = CameraRotation.Vector ();

      FVector TargetLocation = TargetCurrent->GetComponentLocation ();
      FVector TargetDir      = TargetLocation - CameraLocation;
      TargetDir.Normalize ();

      // Camera lerp speed
      float LerpPower = 4;

      // Transform Angle to Lerp power
      const FRichCurve* TargetAngleToLerpPowerData = TargetAngleToLerpPower.GetRichCurveConst ();
      if (TargetAngleToLerpPowerData) {
         float Angle = URTargetableMgr::GetAngle (CameraDir, TargetDir);
         LerpPower = TargetAngleToLerpPowerData->Eval (Angle);
      }

      // Lerp to Target Rotation
      FRotator TargetRot = FMath::Lerp (CameraDir, TargetDir, DeltaTime * LerpPower).Rotation ();

      // Set Rotation
      GetController ()->SetControlRotation (TargetRot);
   }
}

void ARPlayer::TargetSearch ()
{
   if (TargetCurrent) {
      TargetCurrent->SetIsTargeted (false);
      TargetCurrent = nullptr;
   } else {

      if (TargetMgr) {

         TArray<AActor*> blacklist;
         blacklist.Add (this);

         TargetCurrent = TargetMgr->Find (FirstPersonCameraComponent->GetComponentLocation (),
                                          FirstPersonCameraComponent->GetComponentRotation (),
                                          blacklist);

         if (TargetCurrent) TargetCurrent->SetIsTargeted (true);
      }
   }
   OnTargetUpdated.Broadcast ();
}

void ARPlayer::TargetCheck ()
{
   if (TargetCurrent && TargetMgr) {

      bool RemoveTarget = false;

      float Distance = FVector::Dist (GetActorLocation (), TargetCurrent->GetComponentLocation ());
      if (Distance > TargetMgr->SearchDistance) RemoveTarget = true;
      if (!TargetCurrent->GetIsTargetable ())   RemoveTarget = true;

      if (RemoveTarget) {
         TargetCurrent->SetIsTargeted (false);
         TargetCurrent = nullptr;
         OnTargetUpdated.Broadcast ();
      }
   }
}

//=============================================================================
//                           State Checking
//=============================================================================

// Update First Person and Third Person Components visibility
void ARPlayer::UpdateComponentsVisibility ()
{
   if (CurrentCameraState == ECameraState::FP_Camera) {
      if (ThirdPersonCameraComponent) ThirdPersonCameraComponent->Deactivate();
      if (FirstPersonCameraComponent) FirstPersonCameraComponent->Activate();

      if (GetMesh()) GetMesh()->SetOwnerNoSee(true);
      if (Mesh1P) Mesh1P->SetVisibility(true);

   // Currently Third Person Camera
   } else if (CurrentCameraState == ECameraState::TP_Camera) {

      if (ThirdPersonCameraComponent) ThirdPersonCameraComponent->Activate();
      if (FirstPersonCameraComponent) FirstPersonCameraComponent->Deactivate();

      if (GetMesh()) GetMesh()->SetOwnerNoSee(false);
      if (Mesh1P)    Mesh1P->SetVisibility(false);
   }
}

//=============================================================================
//             Save/Load
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

