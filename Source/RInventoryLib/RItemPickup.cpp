// Copyright 2015-2023 Vagen Ayrapetyan

#include "RItemPickup.h"
#include "RInventoryComponent.h"
#include "RUtilLib/RLog.h"
#include "RUtilLib/RUtil.h"
#include "RUtilLib/RCheck.h"
#include "RUtilLib/RTimer.h"
#include "RInteractLib/RInteractComponent.h"

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

   Inventory = CreateDefaultSubobject<URInventoryComponent>(TEXT("Inventory"));
   Inventory->SetIsReplicated (true);


   Interact = CreateDefaultSubobject<URInteractComponent>(TEXT("Interact"));
   Interact->SetupAttachment (MeshComponent);
   Interact->SetRelativeLocation (FVector (0, 0, 40));

   Description.Label = "Item Pickup";

   bReplicates = true;
   SetReplicatingMovement (true);
}

// Replication
void ARItemPickup::GetLifetimeReplicatedProps (TArray<FLifetimeProperty> &OutLifetimeProps) const
{
   Super::GetLifetimeReplicatedProps (OutLifetimeProps);

   DOREPLIFETIME (ARItemPickup, MeshComponent);
   DOREPLIFETIME (ARItemPickup, Inventory);
   DOREPLIFETIME (ARItemPickup, PickupActivationDelay);
   DOREPLIFETIME (ARItemPickup, bAutoDestroy);
   DOREPLIFETIME (ARItemPickup, Description);
}

FRUIDescription ARItemPickup::GetDescription_Implementation () const
{
   return Description;
}

void ARItemPickup::BeginPlay ()
{
   Super::BeginPlay ();
   Interact->SetIsInteractable (false);
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
   Interact->SetIsInteractable (true);
}

void ARItemPickup::OnInventoryUpdate ()
{
   if (Inventory->GetItems ().Num () == 0) {
      if (R_IS_VALID_WORLD && OnPickupEmptied.IsBound ()) OnPickupEmptied.Broadcast ();
      if (bAutoDestroy) Destroy ();
   }
}

