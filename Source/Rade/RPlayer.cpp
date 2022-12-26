// Copyright 2015-2023 Vagen Ayrapetyan

#include "RPlayer.h"

#include "RUtilLib/RLog.h"
#include "RInventoryLib/RInventoryComponent.h"
#include "RSaveLib/RSaveMgr.h"
#include "RCharacterLib/RAnimInstance.h"

#include "Engine.h"

#include "RJetpackComponent.h"


//#include "Net/UnrealNetwork.h"

//-----------------------------------------------------------------------------
//             Core
//-----------------------------------------------------------------------------

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

   // --- Inventory
   if (Inventory) Inventory->bSaveLoadInventory = true;

   Jetpack = CreateDefaultSubobject<URJetpackComponent>(TEXT("Jetpack"));


   MoveSpeed        = 1;
   PlayerController = nullptr;
   bAutoRevive      = true;

   //CharacterName = "Rade Player";
   //bCanRevive = true;
}



void ARPlayer::BeginPlay ()
{
   Super::BeginPlay ();

   // --- Gather references

   // Get Player Controller
   if (GetController() && Cast<APlayerController>(GetController())) {
      PlayerController = Cast<APlayerController>(GetController());
   }

   // Get First Person Anim Instance
   if (Mesh1P && Mesh1P->GetAnimInstance() && Cast<URAnimInstance>(Mesh1P->GetAnimInstance()))
      ArmsAnimInstance = Cast<URAnimInstance>(Mesh1P->GetAnimInstance());

   // Set Player Ref in Anim Instance
   //if (ArmsAnimInstance)ArmsAnimInstance->TheCharacter = this;

   // Set Default Anim State
   //Global_SetAnimID_Implementation(EAnimState::Idle_Run);

   // --- Setup Camera
   // Set Current Camera to Default State
   CurrentCameraState = DefaultCameraState;
   UpdateComponentsVisibility();


   // --- Save / Load data
   FRSaveEvent SavedDelegate;
   SavedDelegate.AddDynamic (this, &ARPlayer::OnSave);
   URSaveMgr::OnSave (GetWorld (), SavedDelegate);

   FRSaveEvent LoadedDelegate;
   LoadedDelegate.AddDynamic (this, &ARPlayer::OnLoad);
   URSaveMgr::OnLoad (GetWorld (), LoadedDelegate);


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

void ARPlayer::SaveGame ()
{
   // bool res = URSaveMgr::SaveSync (GetWorld ());
   bool res = URSaveMgr::SaveASync (GetWorld ());
   if (!res) {
      R_LOG ("Save failed");
      return;
   }
}

void ARPlayer::LoadGame ()
{
   // bool res = URSaveMgr::LoadSync (GetWorld ());
   bool res = URSaveMgr::LoadASync (GetWorld ());
   if (!res) {
      R_LOG ("Load failed");
      return;
   }
}

void ARPlayer::OnSave ()
{
   // --- Save player location
   R_LOG ("Saving game. Set data to save file");
   FVector oldData = GetActorLocation ();

   FBufferArchive ToBinary;
   ToBinary << oldData;

   bool res = URSaveMgr::Set (GetWorld (), FString ("PlayerLocation"), ToBinary);

   if (!res) {
      R_LOG ("Set failed");
      return;
   }
}

void ARPlayer::OnLoad ()
{
   // --- Load player location
   R_LOG ("Load finished. Get data from save file");

   TArray<uint8> BinaryArray;
   bool res = URSaveMgr::Get (GetWorld (), FString ("PlayerLocation"), BinaryArray);
   if (!res) {
      R_LOG ("Get failed");
      return;
   }

   FVector newData;
   FMemoryReader FromBinary = FMemoryReader (BinaryArray, true);
   FromBinary.Seek(0);
   FromBinary << newData;

   SetActorLocation (newData);
}

//-----------------------------------------------------------------------------
//             Input
//-----------------------------------------------------------------------------


// Bind input to events
void ARPlayer::SetupPlayerInputComponent (UInputComponent* inputComponent)
{
   // set up gameplay key bindings
   check(inputComponent);

   // Movement Input
   inputComponent->BindAxis   ("MoveForward",      this, &ARPlayer::Input_MoveForward);
   inputComponent->BindAxis   ("MoveRight",        this, &ARPlayer::Input_MoveRight);
   inputComponent->BindAction ("Jump", IE_Pressed, this, &ARPlayer::Input_Jump);

   // Camera Input
   inputComponent->BindAxis   ("Turn",                     this, &ARPlayer::Input_AddControllerYawInput);
   inputComponent->BindAxis   ("LookUp",                   this, &ARPlayer::Input_AddControllerPitchInput);
   inputComponent->BindAction ("ChangeCamera", IE_Pressed, this, &ARPlayer::Input_ChangeCamera);

   // Action/Inventory Input
   inputComponent->BindAction ("Inventory", IE_Pressed, this, &ARPlayer::Input_ToggleInventory);
   inputComponent->BindAxis   ("MouseScroll",           this, &ARPlayer::Input_MouseScroll);

   inputComponent->BindAction ("Action",    IE_Pressed, this, &ARPlayer::Input_Action);
   inputComponent->BindAction ("FAction",   IE_Pressed, this, &ARPlayer::Input_FAction);


   // Weapon Input
   //inputComponent->BindAction("Fire",         IE_Pressed,  this, &ARPlayer::FireStart);
   //inputComponent->BindAction("Fire",         IE_Released, this, &ARPlayer::FireEnd);
   //inputComponent->BindAction("AltFire",      IE_Pressed,  this, &ARPlayer::AltFireStart);
   //inputComponent->BindAction("AltFire",      IE_Released, this, &ARPlayer::AltFireEnd);
   inputComponent->BindAction ("MeleeAction",  IE_Pressed, this, &ARPlayer::Input_MeleeAction);
   inputComponent->BindAction ("Reload",       IE_Pressed, this, &ARPlayer::Input_Reload);

   // Option and chat
   inputComponent->BindAction ("Option", IE_Pressed, this, &ARPlayer::Input_ToggleOption);

   // Save/Load
   inputComponent->BindAction ("Save",       IE_Pressed, this, &ARPlayer::SaveGame);
   inputComponent->BindAction ("Load",       IE_Pressed, this, &ARPlayer::LoadGame);
}

// ---  Movement Input

// Forward/Backward Movement Input
void ARPlayer::Input_MoveForward (float Value)
{
   if (bDead) return;
   if (Value != 0.0f) AddMovementInput (GetActorForwardVector (), Value * MoveSpeed);
}

// Right/Left Movement Input
void ARPlayer::Input_MoveRight (float Value)
{
   if (bDead) return;
   if (Value != 0.0f) AddMovementInput (GetActorRightVector (), Value * MoveSpeed);
}

// Player Pressed Jump
void ARPlayer::Input_Jump ()
{
   if (bDead) return;
   Super::Jump ();

   if (Jetpack) Jetpack->Use ();
}


// ---  Camera Input

// Vertical Rotation Input
void ARPlayer::Input_AddControllerPitchInput (float Rate)
{
   if (Rate != 0.0f) Super::AddControllerPitchInput (Rate * CameraMouseSensivity);
}

// Horizontal Rotation Input
void ARPlayer::Input_AddControllerYawInput (float Rate)
{
   if (Rate != 0.0f) Super::AddControllerYawInput (Rate * CameraMouseSensivity);
}

// Player Pressed CameraChange
void ARPlayer::Input_ChangeCamera()
{
   if (bDead) return;
   // Change Camera
   if (DefaultCameraState == ECameraState::FP_Camera) {
      DefaultCameraState = ECameraState::TP_Camera;
      CurrentCameraState = DefaultCameraState;
      UpdateComponentsVisibility();
   } else if (DefaultCameraState == ECameraState::TP_Camera) {
      DefaultCameraState = ECameraState::FP_Camera;
      CurrentCameraState = DefaultCameraState;
      UpdateComponentsVisibility();
   }
}

// Player Mesh Rotation after after the input
void ARPlayer::FaceRotation (FRotator NewControlRotation, float DeltaTime)
{
   Super::FaceRotation (NewControlRotation,DeltaTime);
   if (FirstPersonCameraComponent) {
      FRotator rot = FirstPersonCameraComponent->GetComponentRotation ();
      rot.Pitch = NewControlRotation.Pitch;
      FirstPersonCameraComponent->SetWorldRotation (rot);
   }
}


// --- Action/Inventory Input

// Toggle HUD Inventory visibility
void ARPlayer::Input_ToggleInventory ()
{
   OnToggleInventory.Broadcast ();
}

// Player Scrolled Mouse Wheel
void ARPlayer::Input_MouseScroll (float Value)
{
   if (Value != 0) OnScrollInventory.Broadcast (Value);
}

void ARPlayer::Input_Action()
{
   OnUseInventory.Broadcast ();
}

// Player Pressed FAction
void ARPlayer::Input_FAction()
{
   OnDropInventory.Broadcast ();
}

// Player Pressed Alt Action
void ARPlayer::Input_MeleeAction()
{

   /*
   if (  TheWeapon
      && IsAnimState(EAnimState::Idle_Run)
      && CharacterMovementComponent
      && CharacterMovementComponent->IsMovingOnGround()) {
      TheWeapon->PreMeleeAttack();
   }
   */
}


// --- Weapon Input

// Player Pressed Reload
void ARPlayer::Input_Reload()
{
   /*
   // Check if Player and Weapon are in state to reload
   if (  IsAnimState(EAnimState::Idle_Run)
      && CharacterMovementComponent
      && CharacterMovementComponent->IsMovingOnGround()
      && TheWeapon
      && TheWeapon->CanReload())
   {
      TheWeapon->ReloadWeaponStart();
   }
   */
}

/*

// Player Pressed Fire Button
bool ARPlayer::FireStart_Validate()
{
   return true;
}

void ARPlayer::FireStart_Implementation()
{
   // Check if player has weapon and is in state to fire weapon
   //if (TheWeapon && CanShoot()) TheWeapon->FireStart();
}

bool ARPlayer::FireEnd_Validate(){
   return true;
}

// Player Released Fire button
void ARPlayer::FireEnd_Implementation()
{
   //if (TheWeapon) TheWeapon->FireEnd();
}

// Player Pressed Alt Fire
bool ARPlayer::AltFireStart_Validate(){
   return true;
}
void ARPlayer::AltFireStart_Implementation()
{
   if (TheWeapon) TheWeapon->AltFireStart();
}

// Player Released AltFire
bool ARPlayer::AltFireEnd_Validate(){
   return true;
}
void ARPlayer::AltFireEnd_Implementation()
{
   if (TheWeapon)
      TheWeapon->AltFireEnd();
}

*/

void ARPlayer::Input_ToggleOption()
{
   OnToggleOption.Broadcast ();
}


//-----------------------------------------------------------------------------
//             Weapon
//-----------------------------------------------------------------------------

/*

// Called to equip new Weapon
void ARPlayer::EquipWeapon(ARadeWeapon* NewWeaponClass)
{
   if (!NewWeaponClass)return;

   // If player has a weapon, Unequip it
   if (TheWeapon) TheWeapon->UnEquipStart();

   // Set next equip weapon
   PendingEquipWeapon = NewWeaponClass;

   // Start Player unequip anim even if there is no current weapon
   UnEquipStart();
}

// Current Weapon in being unequiped
void ARPlayer::UnEquipCurrentWeapon()
{
   if (TheWeapon) TheWeapon->UnEquipStart();
   UnEquipStart();
}

// Player Started equip of new weapon
void ARPlayer::EquipStart()
{
   if (PendingEquipWeapon && !TheWeapon)
   {
      // Set next weapon anim type
      if (ArmsAnimInstance) ArmsAnimInstance->AnimArchetype = PendingEquipWeapon->AnimArchetype;
      if (BodyAnimInstance) BodyAnimInstance->AnimArchetype = PendingEquipWeapon->AnimArchetype;

      // Set Animation state
      ServerSetAnimID(EAnimState::Equip);

      // Spawn new weapon
      TheWeapon = GetWorld()->SpawnActor<ARadeWeapon>(PendingEquipWeapon->GetClass());
      TheWeapon->ThePlayer = this;
      TheWeapon->SetOwner(this);
      TheWeapon->EquipStart();

      FTimerHandle myHandle;
      GetWorldTimerManager().SetTimer(myHandle, this, &ARPlayer::EquipEnd, TheWeapon->EquipTime, false);
   }
   UpdateComponentsVisibility();
}

// Called when equip is finished
void ARPlayer::EquipEnd()
{
   ServerSetAnimID(EAnimState::Idle_Run);

   // Clear pending weapon
   if (PendingEquipWeapon!=NULL)PendingEquipWeapon = NULL;

   // Tell HUD that weapon is updated
   if (TheHUD) TheHUD->BP_WeaponUpdated();
}

// Unequip curent weapon
void ARPlayer::UnEquipStart()
{
   // Set Animation State
   ServerSetAnimID(EAnimState::UnEquip);

   if (TheWeapon) {
      // Set Delay of current weapon
      FTimerHandle myHandle;
      GetWorldTimerManager().SetTimer(myHandle, this, &ARPlayer::UnEquipEnd, TheWeapon->EquipTime, false);
   } else {
      // Set Default Delay
      FTimerHandle myHandle;
      GetWorldTimerManager().SetTimer(myHandle, this, &ARPlayer::UnEquipEnd, DefaultWeaponEquipDelay, false);
   }

   UpdateComponentsVisibility();
}

// Unequip Ended
void ARPlayer::UnEquipEnd()
{
   // Destroy Current weapon
   if (TheWeapon) {
      TheWeapon->Destroy();
      TheWeapon = NULL;
   }

   // Weapon Was Updated
   if (TheHUD) TheHUD->BP_WeaponUpdated();

   // if there is new weapon Start the equip
   if (PendingEquipWeapon) {
      EquipStart();

   // Else return to empty hand state
   } else {
      if (ArmsAnimInstance) ArmsAnimInstance->AnimArchetype = EAnimArchetype::EmptyHand;
      if (BodyAnimInstance) BodyAnimInstance->AnimArchetype = EAnimArchetype::EmptyHand;
      ServerSetAnimID(EAnimState::Idle_Run);
   }
}

void ARPlayer::CurrentWeaponUpdated()
{
   Super::CurrentWeaponUpdated();

   if (ArmsAnimInstance) {
      if (TheWeapon) ArmsAnimInstance->AnimArchetype = TheWeapon->AnimArchetype;
      else            ArmsAnimInstance->AnimArchetype = EAnimArchetype::EmptyHand;
   }

   if (BodyAnimInstance) {
      if (TheWeapon) BodyAnimInstance->AnimArchetype = TheWeapon->AnimArchetype;
      else             BodyAnimInstance->AnimArchetype = EAnimArchetype::EmptyHand;
   }

   if (TheHUD) TheHUD->BP_WeaponUpdated();

   UpdateComponentsVisibility();
}
*/

//-----------------------------------------------------------------------------
//                           State Checking
//-----------------------------------------------------------------------------

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


// Player landed on ground
void ARPlayer::Landed(const FHitResult& Hit)
{
   Super::Landed(Hit);

   if (ArmsAnimInstance || BodyAnimInstance)
      ServerSetAnimID(EAnimState::Idle_Run);
}

*/




// Update First Person and Third Person Components visibility
void ARPlayer::UpdateComponentsVisibility()
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


//-----------------------------------------------------------------------------
//                   Damage/Death/Revive
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
//             Animation
//-----------------------------------------------------------------------------

/*

void ARPlayer::Global_SetAnimID_Implementation(EAnimState AnimID)
{
   Super::Global_SetAnimID_Implementation(AnimID);

   // Set The Value in anim instances
   if (ArmsAnimInstance) ArmsAnimInstance->RecieveGlobalAnimID(AnimID);

}
// Check Anim State on body or arms
bool ARPlayer::IsAnimState(EAnimState TheAnimState)
{
   if (ArmsAnimInstance) {
      if (ArmsAnimInstance->IsAnimState(TheAnimState)) return true;
      else                                              return false;
   } else return Super::IsAnimState(TheAnimState);
}

void ARPlayer::Global_SetAnimArchtype_Implementation(EAnimArchetype newAnimArchetype)
{
   Super::Global_SetAnimArchtype_Implementation(newAnimArchetype);
   if (ArmsAnimInstance) ArmsAnimInstance->AnimArchetype = newAnimArchetype;
}
*/




//-----------------------------------------------------------------------------
//             Network Chat, Props and Replication
//-----------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------
//       Util
//-----------------------------------------------------------------------------

ARPlayer* ARPlayer::GetLocalRadePlayer (UObject* WorldContextObject)
{
   if (!WorldContextObject) return nullptr;
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




// Replication of data
void ARPlayer::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
   Super::GetLifetimeReplicatedProps(OutLifetimeProps);

   //DISABLE_REPLICATED_PROPERTY (ARPlayer, FirstPersonCameraComponent);
   //DISABLE_REPLICATED_PROPERTY (ARPlayer, ThirdPersonCameraBoom);
   //DISABLE_REPLICATED_PROPERTY (ARPlayer, ThirdPersonCameraComponent);

   //DOREPLIFETIME(ARPlayer, BodyAnimInstance);
   //DOREPLIFETIME(ARPlayer, BodyAnimInstance);
   //DOREPLIFETIME(ARPlayer, BodyAnimInstance);

   //DOREPLIFETIME(ARPlayer, bInventoryOpen);
   //DOREPLIFETIME(ARPlayer, CurrentItemSelectIndex);
   //DOREPLIFETIME(ARPlayer, CurrentCameraState);
   //DOREPLIFETIME(ARPlayer, currentPickup);
}

