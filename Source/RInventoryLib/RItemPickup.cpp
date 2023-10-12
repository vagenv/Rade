// Copyright 2015-2023 Vagen Ayrapetyan

#include "RItemPickup.h"
#include "RInventoryComponent.h"
#include "RItemPickupMgrComponent.h"
#include "RUtilLib/RLog.h"
#include "RUtilLib/RUtil.h"
#include "RUtilLib/RCheck.h"
#include "RUtilLib/RTimer.h"

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
   SetRootComponent (MeshComponent);

   static ConstructorHelpers::FObjectFinder<UStaticMesh>
      defaultMesh (TEXT("StaticMesh'/Game/Rade/Meshes/BasicMeshes/Shapes/Shape_Cube.Shape_Cube'"));
   MeshComponent->SetStaticMesh (defaultMesh.Object);

   // Set Trigger Component
   TriggerSphere = CreateDefaultSubobject<USphereComponent> (TEXT("TriggerSphere"));
   TriggerSphere->InitSphereRadius (400);
   TriggerSphere->SetIsReplicated (true);
   TriggerSphere->BodyInstance.SetCollisionProfileName ("Pickup");
   TriggerSphere->SetupAttachment (MeshComponent);

   Inventory = CreateDefaultSubobject<URInventoryComponent>(TEXT("Inventory"));
   Inventory->SetIsReplicated (true);

   Description.Label = "Item Pickup";

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
   DOREPLIFETIME (ARItemPickup, Description);
}

FRUIDescription ARItemPickup::GetDescription_Implementation ()
{
   return Description;
}

void ARItemPickup::BeginPlay ()
{
   Super::BeginPlay ();

   Inventory->OnInventoryUpdated.AddDynamic (this, &ARItemPickup::OnInventoryUpdate);

   // Enable overlap after a delay
   FTimerHandle TempHandle;
   RTIMER_START (TempHandle,
                 this, &ARItemPickup::ActivatePickupOverlap,
                 PickupActivationDelay,
                 false);
}

// Activate Overlap detection
void ARItemPickup::ActivatePickupOverlap ()
{
   // Check if Any Player is within the range to pickup this item
   for (TActorIterator<AActor> ActorItr(URUtil::GetWorld (this)); ActorItr; ++ActorItr) {

      if (!IsValid (*ActorItr)) continue;

      // Only if actor has Inventory
      if (ActorItr->FindComponentByClass<URInventoryComponent>()) {
         float distance = FVector::Distance (ActorItr->GetActorLocation (), GetActorLocation());
         if (distance > TriggerSphere->GetUnscaledSphereRadius ()) continue;
         OnBeginOverlap (nullptr, *ActorItr, nullptr, 0, false, FHitResult());
      }
   }

   // Enable Overlap Component if no pending kill set
   if (IsValid (this) && IsValid (TriggerSphere)) {
      TriggerSphere->OnComponentBeginOverlap.AddDynamic (this, &ARItemPickup::OnBeginOverlap);
      TriggerSphere->OnComponentEndOverlap.AddDynamic   (this, &ARItemPickup::OnEndOverlap);
   }
}

// Player Entered The Pickup Area
void ARItemPickup::OnBeginOverlap (UPrimitiveComponent* OverlappedComponent,
                                   AActor* OtherActor,
                                   UPrimitiveComponent* OtherComp,
                                   int32 OtherBodyIndex,
                                   bool bFromSweep,
                                   const FHitResult &SweepResult)
{
   // Invalid or itself
   if (!IsValid (OtherActor) || OtherActor == this) return;

   // Add to Inventory Directly
   if (bAutoPickup) {

      if (HasAuthority ()) {
         if (URInventoryComponent* ActorInventory = URUtil::GetComponent<URInventoryComponent> (OtherActor))
            Inventory->TransferAll (ActorInventory);
      }

   // Add to Tracking
   } else {
      URItemPickupMgrComponent* PickupTracker = URUtil::GetComponent<URItemPickupMgrComponent> (OtherActor);
      if (!IsValid (PickupTracker)) return;

      PickupTracker->Pickup_Register (this);

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
   // Invalid or itself
   if (!IsValid (OtherActor) || OtherActor == this) return;

  URItemPickupMgrComponent* PickupTracker = URUtil::GetComponent<URItemPickupMgrComponent> (OtherActor);
   if (!IsValid (PickupTracker)) return;
   PickupTracker->Pickup_Unregister (this);

   // BP Event that player Exited
   BP_PlayerLeft (OtherActor);
}

void ARItemPickup::OnInventoryUpdate ()
{
   if (Inventory->GetItems ().Num () == 0) {
      if (R_IS_VALID_WORLD && OnPickupEmptied.IsBound ()) OnPickupEmptied.Broadcast ();
      if (bAutoDestroy) Destroy ();
   }
}

