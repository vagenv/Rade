// Copyright 2015-2023 Vagen Ayrapetyan

#include "RCharacter.h"
//#include "RadeAnimInstance.h"
//#include "../Weapon/RadeWeapon.h"
#include "Net/UnrealNetwork.h"
#include "RInventoryLib/RInventoryComponent.h"
#include "RUtilLib/RLog.h"

class AController;

//=============================================================================
//                  Base Character
//=============================================================================

ARCharacter::ARCharacter ()
{
   // Default fall damage Curve
   FRichCurve* FallDamageCurveData = FallDamageCurve.GetRichCurve ();
   FallDamageCurveData->AddKey (1000, 0); // Minimum
   FallDamageCurveData->AddKey (1500, 40);
   FallDamageCurveData->AddKey (2000, 100);

   Inventory = CreateDefaultSubobject<URInventoryComponent>(TEXT("Inventory"));
   //Inventory->SetIsReplicated(true);
   bReplicates  = true;
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
   OnDeath.Broadcast ();
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
   OnRevive.Broadcast ();
   GetCapsuleComponent()->BodyInstance.SetCollisionProfileName ("Pawn");
   USkeletalMeshComponent *skelMesh = GetMesh ();
   if (skelMesh) {
      skelMesh->SetSimulatePhysics (false);
      skelMesh->AttachToComponent (RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
      skelMesh->SetRelativeLocation (Mesh_DefaultRelativeLoc);
      skelMesh->SetRelativeRotation (Mesh_DefaultRelativeRot);
      skelMesh->BodyInstance.SetCollisionProfileName ("Pawn");
   }
}



// Take Damage
float ARCharacter::TakeDamage (float DamageAmount,
                               struct FDamageEvent const& DamageEvent,
                               AController* EventInstigator,
                               AActor* DamageCauser)
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
   const FRichCurve* FallDamageCurveData = FallDamageCurve.GetRichCurve ();
   if (FallDamageCurveData) {
      // Horizontal velocity is ignored
      float FallVelocityZ = GetCharacterMovement()->Velocity.GetAbs ().Z;
      float dmg = FallDamageCurveData->Eval (FallVelocityZ);
      dmg = UGameplayStatics::ApplyDamage (this, dmg, GetController (), Hit.GetActor (), UDamageType::StaticClass ());
   }

   Super::Landed (Hit);
}

