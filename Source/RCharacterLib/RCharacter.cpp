// Copyright 2015-2023 Vagen Ayrapetyan

#include "RCharacter.h"
#include "RUtilLib/RLog.h"
#include "RStatusLib/RDamageType.h"
#include "RStatusLib/RStatusMgrComponent.h"
#include "REquipmentMgrComponent.h"
#include "Net/UnrealNetwork.h"

class AController;

//=============================================================================
//                  Base Character
//=============================================================================

ARCharacter::ARCharacter ()
{
   EquipmentMgr = CreateDefaultSubobject<UREquipmentMgrComponent> (TEXT("EquipmentManager"));
   EquipmentMgr->SetIsReplicated (true);

   StatusMgr = CreateDefaultSubobject<URStatusMgrComponent> (TEXT("StatusManager"));
   StatusMgr->SetIsReplicated (true);

   bReplicates  = true;
}

// Replication
void ARCharacter::GetLifetimeReplicatedProps (TArray<FLifetimeProperty> &OutLifetimeProps) const
{
   Super::GetLifetimeReplicatedProps (OutLifetimeProps);
   DOREPLIFETIME (ARCharacter, EquipmentMgr);
   DOREPLIFETIME (ARCharacter, StatusMgr);
}

// Called when the game starts or when spawned
void ARCharacter::BeginPlay()
{
   Super::BeginPlay ();

   if (GetMesh ()) {
      Mesh_DefaultRelativeLoc = GetMesh ()->GetRelativeLocation ();
      Mesh_DefaultRelativeRot = GetMesh ()->GetRelativeRotation ();
   }
}

//=============================================================================
//                       Take Damage, Death
//=============================================================================

//         Server Death
void ARCharacter::Die_Server_Implementation (AActor *DeathCauser, AController* EventInstigator)
{
   if (!HasAuthority ()) return;

   Die_Client (DeathCauser, EventInstigator);
   Die        (DeathCauser, EventInstigator);

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

void ARCharacter::ForceRagdoll ()
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
                               FDamageEvent const& DamageEvent,
                               AController* EventInstigator,
                               AActor* DamageCauser)
{
   float ActualDamage = Super::TakeDamage (DamageAmount, DamageEvent, EventInstigator, DamageCauser);
   return StatusMgr->TakeDamage (ActualDamage, DamageEvent, EventInstigator, DamageCauser);
}

// Character Landed on Ground
void ARCharacter::Landed (const FHitResult& Hit)
{
   // Take only vertical velocity
   float FallVelocityZ = GetCharacterMovement ()->Velocity.GetAbs ().Z;
   UGameplayStatics::ApplyDamage (this, FallVelocityZ, GetController (), Hit.GetActor (), URDamageType_Fall::StaticClass ());
   Super::Landed (Hit);
}

