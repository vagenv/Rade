// Copyright 2015-2023 Vagen Ayrapetyan

#include "RItemPickup.h"
#include "RItemAction.h"
#include "RInventoryComponent.h"
#include "RUtilLib/RLog.h"

#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ARItemPickup::ARItemPickup ()
{
   // Set Static Mesh Component
   MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
   MeshComponent->SetMobility (EComponentMobility::Movable);
   MeshComponent->SetCollisionResponseToAllChannels (ECollisionResponse::ECR_Block);
   MeshComponent->SetCollisionEnabled (ECollisionEnabled::PhysicsOnly);
   MeshComponent->BodyInstance.SetCollisionProfileName("BlockAll");
   MeshComponent->SetSimulatePhysics (true);
   MeshComponent->SetIsReplicated (true);
   MeshComponent->bAutoActivate = true;

   static ConstructorHelpers::FObjectFinder<UStaticMesh>
      defaultMesh (TEXT("StaticMesh'/Game/Rade/Meshes/BasicMeshes/Shapes/Shape_Cube.Shape_Cube'"));
   MeshComponent->SetStaticMesh (defaultMesh.Object);
   SetRootComponent (MeshComponent);

   // Set Trigger Component
   TriggerSphere = CreateDefaultSubobject<USphereComponent> (TEXT("TriggerSphere"));
   TriggerSphere->InitSphereRadius (400);
   TriggerSphere->SetIsReplicated (true);
   TriggerSphere->BodyInstance.SetCollisionProfileName ("Pickup");
   TriggerSphere->SetupAttachment (MeshComponent);

   Inventory = CreateDefaultSubobject<URInventoryComponent>(TEXT("Inventory"));
   Inventory->SetIsReplicated (true);

   bReplicates = true;
   SetReplicatingMovement (true);
}

// Replication
void ARItemPickup::GetLifetimeReplicatedProps (TArray<FLifetimeProperty> &OutLifetimeProps) const
{
   Super::GetLifetimeReplicatedProps (OutLifetimeProps);

   DOREPLIFETIME (ARItemPickup, TriggerSphere);
   DOREPLIFETIME (ARItemPickup, MeshComponent);
   DOREPLIFETIME (ARItemPickup, Inventory);
   DOREPLIFETIME (ARItemPickup, PickupActivationDelay);
   DOREPLIFETIME (ARItemPickup, bAutoPickup);
   DOREPLIFETIME (ARItemPickup, bAutoDestroy);
}

void ARItemPickup::BeginPlay ()
{
   Super::BeginPlay ();

   Inventory->OnInventoryUpdated.AddDynamic (this, &ARItemPickup::OnInventoryUpdate);

   // Enable overlap after a delay
   FTimerHandle MyHandle;
   GetWorldTimerManager().SetTimer (MyHandle,
                                    this,
                                    &ARItemPickup::ActivatePickupOverlap,
                                    PickupActivationDelay,
                                    false);
}

// Activate Overlap detection
void ARItemPickup::ActivatePickupOverlap ()
{

   /*
   // Check if Any Player is within the range to pickup this item
   if (GetLocalRole() >= ROLE_Authority && bAutoPickup) {

      for (TActorIterator<AActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
      {
         float distance = FVector::Distance (ActorItr->GetActorLocation (), GetActorLocation());
         if (distance > TriggerSphere->GetUnscaledSphereRadius()) continue;

         OnBeginOverlap (nullptr, *ActorItr, nullptr, 0, false, FHitResult());
      }
   }
   */

   // Enable Overlap Component
   TriggerSphere->OnComponentBeginOverlap.AddDynamic (this, &ARItemPickup::OnBeginOverlap);
   TriggerSphere->OnComponentEndOverlap.AddDynamic   (this, &ARItemPickup::OnEndOverlap);
}

// Player Entered The Pickup Area
void ARItemPickup::OnBeginOverlap (UPrimitiveComponent* OverlappedComponent,
                                   AActor* OtherActor,
                                   UPrimitiveComponent* OtherComp,
                                   int32 OtherBodyIndex,
                                   bool bFromSweep,
                                   const FHitResult & SweepResult)
{
   // Null or itself
   if (OtherActor == nullptr || OtherActor == this) return;

   // Only Inventory containing actors
   URInventoryComponent *PlayerInventory = OtherActor->FindComponentByClass<URInventoryComponent>();
   if (PlayerInventory == nullptr) return;

   if (bAutoPickup) {
      if (HasAuthority ())
         Inventory->TransferAll (PlayerInventory);
   } else {
      PlayerInventory->Pickup_Add (this);

      // BP Event that player entered
      BP_PlayerEntered (OtherActor);
   }
}

// Player Exited The Pickup Area
void ARItemPickup::OnEndOverlap (UPrimitiveComponent* OverlappedComponent,
                                 AActor* OtherActor,
                                 UPrimitiveComponent* OtherComp,
                                 int32 OtherBodyIndex)
{
   // Null or itself
   if (OtherActor == nullptr || OtherActor == this) return;

   // Does not have inventory
   URInventoryComponent *PlayerInventory = OtherActor->FindComponentByClass<URInventoryComponent>();
   if (PlayerInventory == nullptr) return;

   PlayerInventory->Pickup_Rm (this);

   // BP Event that player Exited
   BP_PlayerLeft (OtherActor);
}

// The Actual event of pickup
void ARItemPickup::PickedUp (AActor *InventoryOwner)
{
   // if (!InventoryOwner) return;
   // URInventoryComponent *PlayerInventory = InventoryOwner->FindComponentByClass<URInventoryComponent> ();
   // if (!Inventory) return;

   // if (bAutoDestroy) PlayerInventory->CurrentPickups.RemoveSingle (this);
   // PlayerInventory->OnPickupsUpdated.Broadcast ();
   // BP_PickedUp (InventoryOwner);
   // if (bAutoDestroy) Destroy ();
}

void ARItemPickup::OnInventoryUpdate ()
{
   if (bAutoDestroy && Inventory->GetItems ().Num () == 0) {
      Destroy ();
   }
}

