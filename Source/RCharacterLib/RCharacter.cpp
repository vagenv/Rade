// Copyright 2015-2023 Vagen Ayrapetyan

#include "RCharacter.h"
//#include "RadeAnimInstance.h"
//#include "../Weapon/RadeWeapon.h"
#include "Net/UnrealNetwork.h"
#include "RInventoryLib/RInventoryComponent.h"
#include "RUtilLib/RLog.h"

//=============================================================================
//                  Base Character
//=============================================================================

ARCharacter::ARCharacter()
{
   // Default fall damage Curve
   FallDamageMinVelocity = 1000;
   FRichCurve* FallDamageCurveData = FallDamageCurve.GetRichCurve ();
   FallDamageCurveData->AddKey (FallDamageMinVelocity, 10);
   FallDamageCurveData->AddKey (1500 ,40);
   FallDamageCurveData->AddKey (2000, 100);

   Inventory = CreateDefaultSubobject<URInventoryComponent>(TEXT("Inventory"));
   //Inventory->SetIsReplicated(true);

   HealthMax    = 100;
   Health       = 90;
   bDead        = false;
   bReplicates  = true;
   //CharacterName = "Rade Character";
}

// Replication
void ARCharacter::GetLifetimeReplicatedProps (TArray<FLifetimeProperty> &OutLifetimeProps) const
{
   Super::GetLifetimeReplicatedProps (OutLifetimeProps);

   DOREPLIFETIME (ARCharacter, HealthMax);
   DOREPLIFETIME (ARCharacter, Health);
   DOREPLIFETIME (ARCharacter, bDead);

   /*
   DOREPLIFETIME(ARCharacter, TheInventory);
   DOREPLIFETIME(ARCharacter, TheWeapon);

   DOREPLIFETIME(ARCharacter, CharacterName);
   DOREPLIFETIME(ARCharacter, CharacterColor);
   */
}


// Called when the game starts or when spawned
void ARCharacter::BeginPlay()
{
   Super::BeginPlay ();
   bDead = false;
   if (GetMesh ()) {
      Mesh_DefaultRelativeLoc = GetMesh ()->GetRelativeLocation ();
      Mesh_DefaultRelativeRot = GetMesh ()->GetRelativeRotation ();
   }

   /*
   // Get Third Person Anim Instance
   if (GetMesh() && GetMesh()->GetAnimInstance() && Cast<URAnimInstance>(GetMesh()->GetAnimInstance())) {
      BodyAnimInstance = Cast<URAnimInstance>(GetMesh()->GetAnimInstance());
      BodyAnimInstance->TheCharacter = this;
   }
   */
}

/*
//             Called to equip new Weapon
void ARCharacter::EquipWeapon(ARadeWeapon* NewWeaponClass)
{
   if (!NewWeaponClass)return;

   // If player has a weapon, Un-equip it
   if (TheWeapon) TheWeapon->Destroy();

   Global_SetAnimArchtype(NewWeaponClass->AnimArchetype);

   // Set Animation state
   ServerSetAnimID(ERAnimState::Equip);

   // Spawn new weapon
   TheWeapon = GetWorld()->SpawnActor<ARadeWeapon>(NewWeaponClass->GetClass());

   if (TheWeapon) {
      TheWeapon->SetOwner(this);
      TheWeapon->Mesh3P->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, FName("WeaponSocket"));
   }
}

void ARCharacter::CurrentWeaponUpdated ()
{
}

*/
//=============================================================================
//                       Take Damage, Death
//=============================================================================


//         Server Death
void ARCharacter::Die_Server_Implementation (AActor *DeathCauser, AController* EventInstigator)
{
   if (!HasAuthority ()) return;

   if (bDead) return;

   bDead = true;

   Die_Client (DeathCauser, EventInstigator);
   Die        (DeathCauser, EventInstigator);

   /*
   if (TheWeapon) {
      TheWeapon->Destroy();
      TheWeapon = nullptr;
   }
   // Throw out Inventory Items on Death
   if (TheInventory && TheInventory->bDropItemsOnDeath)
      TheInventory->ThrowOutAllItems();
   */

   // If character should be automatically revived, revive after a delay
   if (bAutoRevive)  {
      FTimerHandle MyHandle;
      GetWorldTimerManager().SetTimer (MyHandle, this, &ARCharacter::Revive_Server, ReviveTime, false);
   }
}

void ARCharacter::Die_Client_Implementation (AActor *DeathCauser, AController* EventInstigator)
{
   if (HasAuthority ()) return;
   Die (DeathCauser, EventInstigator);
}
void ARCharacter::Die (AActor *DeathCauser, AController* EventInstigator)
{
   ForceRagdoll ();
   BP_Died (DeathCauser, EventInstigator);
}

void ARCharacter::ForceRagdoll()
{
   USkeletalMeshComponent *skelMesh = Cast<USkeletalMeshComponent>(GetMesh());
   if (skelMesh) {
      skelMesh->SetSimulatePhysics(true);
      skelMesh->BodyInstance.SetCollisionProfileName ("Ragdoll");
      skelMesh->SetCollisionEnabled (ECollisionEnabled::QueryAndPhysics);
   }
}

// --- Revive character
void ARCharacter::Revive_Server_Implementation ()
{
   if (!HasAuthority ()) return;

   if (!bDead) return;

   bDead  = false;
   Health = HealthMax / 2;

   GetRootComponent ()->SetWorldLocation (GetActorLocation() + FVector (0, 0, 30));

   Revive_Client ();
   Revive ();
}

void ARCharacter::Revive_Client_Implementation ()
{
   if (HasAuthority ()) return;
   Revive ();
}
void ARCharacter::Revive ()
{
   GetCapsuleComponent()->BodyInstance.SetCollisionProfileName("Pawn");
   if (GetMesh()) {
      GetMesh()->SetSimulatePhysics (false);
      GetMesh()->AttachToComponent (RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
      GetMesh()->SetRelativeLocation (Mesh_DefaultRelativeLoc);
      GetMesh()->SetRelativeRotation (Mesh_DefaultRelativeRot);
      GetMesh()->BodyInstance.SetCollisionProfileName ("Pawn");
   }
   // Revived
   BP_Revived ();

   // Global_SetAnimArchtype_Implementation(ERAnimArchetype::EmptyHand);
}



// Take Damage
float ARCharacter::TakeDamage (float DamageAmount,
                                  struct FDamageEvent const& DamageEvent,
                                  class AController* EventInstigator,
                                  class AActor* DamageCauser)
{
   float ActualDamage = Super::TakeDamage (DamageAmount, DamageEvent, EventInstigator, DamageCauser);
   if (ActualDamage != .0f && !bDead) {
      Health -= ActualDamage;

      if (Health < 0) Health = 0;
      if (Health == 0) {
         Die_Server (DamageCauser, EventInstigator);
      }
   }
   return ActualDamage;
}

// Character Landed on Ground
void ARCharacter::Landed (const FHitResult& Hit)
{
   // Horizontal velocity is ignored
   float FallVelocityZ = GetCharacterMovement()->Velocity.GetAbs ().Z;

   // Fall Damage
   if (FallVelocityZ > FallDamageMinVelocity)  {
      FRichCurve* FallDamageCurveData = FallDamageCurve.GetRichCurve ();

      float dmg = 0;
      if (FallDamageCurveData) dmg = FallDamageCurveData->Eval (FallVelocityZ);
      dmg = UGameplayStatics::ApplyDamage (this, dmg, GetController (), Hit.GetActor (), UDamageType::StaticClass ());
   }
   Super::Landed (Hit);
}

/*

//=============================================================================
//                         Network Anim
//=============================================================================

// Set Server Animation ID
bool ARCharacter::ServerSetAnimID_Validate(ERAnimState AnimID){
   return true;
}
void ARCharacter::ServerSetAnimID_Implementation(ERAnimState AnimID)
{
   // Set new State on all users
   Global_SetAnimID(AnimID);
}

// Set Global Animation ID
void ARCharacter::Global_SetAnimID_Implementation(ERAnimState AnimID)
{
   // Set The Value in anim instances
   if (BodyAnimInstance)
      BodyAnimInstance->RecieveGlobalAnimID(AnimID);
}

//         Check Current Animation State
bool ARCharacter::IsAnimState(ERAnimState ThERAnimState)
{
   if (BodyAnimInstance)
   {
      if (BodyAnimInstance->IsAnimState(ThERAnimState))return true;
      else return false;
   }

   return true;
}
//          Is Character In Air
bool ARCharacter::IsAnimInAir()
{
   // Check each air state separately.
   if (  IsAnimState(ERAnimState::JumpEnd)
      || IsAnimState(ERAnimState::Jumploop)
      || IsAnimState(ERAnimState::JumpStart))
      return true;
   else return false;
}

void ARCharacter::Global_SetAnimArchtype_Implementation(ERAnimArchetype newAnimArchetype)
{
   if (BodyAnimInstance) BodyAnimInstance->AnimArchetype = newAnimArchetype;
}

//=============================================================================
//                          Character Stats
//=============================================================================

void ARCharacter::OnRep_CharacterStatsUpdated() {
   BP_CharacterStatsUpdated();
}

bool ARCharacter::SetCharacterStats_Validate(const FString & newName, FLinearColor newColor)
{
   return true;
}

void ARCharacter::SetCharacterStats_Implementation(const FString & newName, FLinearColor newColor)
{
   CharacterName  = newName;
   CharacterColor = newColor;
   if (GetLocalRole() >= ROLE_Authority) OnRep_CharacterStatsUpdated();
}
*/

