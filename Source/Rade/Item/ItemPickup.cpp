// Copyright 2015 Vagen Ayrapetyan

#include "Rade.h"
#include "Character/RadeCharacter.h"

#include "Item/ItemPickup.h"
#include "Item/Inventory.h"
#include "Item/Item.h"
#include "UnrealNetwork.h"

// Sets default values
AItemPickup::AItemPickup(const class FObjectInitializer& PCIP)
	: Super(PCIP)
{



	RootComponent = PCIP.CreateDefaultSubobject<USceneComponent>(this, TEXT("RootComponent"));
	RootComponent->SetIsReplicated(true);

	SkeletalMesh = PCIP.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("SkeletalMesh"));
	SkeletalMesh->AttachParent = RootComponent;
	SkeletalMesh->SetNetAddressable(); 
	SkeletalMesh->SetIsReplicated(true);
	
	SkeletalMesh->bRenderCustomDepth = true;
	SkeletalMesh->MarkRenderStateDirty();
	SkeletalMesh->SetRenderCustomDepth(false);

	Mesh = PCIP.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("Mesh"));
	Mesh->AttachParent = RootComponent;
	Mesh->SetNetAddressable();
	Mesh->SetIsReplicated(true);
	Mesh->SetRenderCustomDepth(false);

	TriggerSphere = PCIP.CreateDefaultSubobject<USphereComponent>(this, TEXT("TriggerSphere"));
	TriggerSphere->InitSphereRadius(300);
	TriggerSphere->AttachParent = RootComponent;

	bReplicates = true;
}


void AItemPickup::BeginPlay()
{
	Super::BeginPlay();

	// Set Pickup Skeletal Mesh Outline
	if (SkeletalMesh)SkeletalMesh->SetRenderCustomDepth(false);

	// Set Pickup Mesh Outline
	if (Mesh)Mesh->SetRenderCustomDepth(false);


	if (Role < ROLE_Authority)
		return;

	SetReplicates(true);

	// Start Activate Delay on server
	FTimerHandle MyHandle;
	GetWorldTimerManager().SetTimer(MyHandle, this, &AItemPickup::ActivatePickupOverlap, PickupActivationDelay, false);
}

// Activate Overlap detection
void AItemPickup::ActivatePickupOverlap()
{
	TriggerSphere->OnComponentBeginOverlap.AddDynamic(this, &AItemPickup::OnBeginOverlap);
	TriggerSphere->OnComponentEndOverlap.AddDynamic(this, &AItemPickup::OnEndOverlap);
}

// Activate Pickup Physics
void AItemPickup::ActivatePickupPhysics()
{
	if (Mesh->StaticMesh)
	{
		SkeletalMesh->DestroyComponent();
		TriggerSphere->AttachTo(Mesh);

		Mesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
		Mesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		Mesh->SetSimulatePhysics(true);
		Mesh->WakeRigidBody();
	}
	else if (SkeletalMesh->SkeletalMesh)
	{
		Mesh->DestroyComponent();
		TriggerSphere->AttachTo(SkeletalMesh,NAME_None, EAttachLocation::SnapToTarget);

		SkeletalMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
		SkeletalMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		SkeletalMesh->SetSimulatePhysics(true);
		SkeletalMesh->WakeRigidBody();		
	}
}

// Player Entered The Pickup Area
void AItemPickup::OnBeginOverlap(AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL) && Cast<ARadeCharacter>(OtherActor) != NULL && Cast<ARadeCharacter>(OtherActor)->TheInventory!=NULL)
	{
		// BP Event that player entered
		BP_PlayerEntered(Cast<ARadeCharacter>(OtherActor));

		// Auto give player the item
		if (bAutoPickup)
		{
			PickedUp(OtherActor);
		}

		// Wait player Input
		else
		{
			// Set player ref of item pickup
			Cast<ARadeCharacter>(OtherActor)->currentPickup = this;

			// Activate highlight
			if (SkeletalMesh)SkeletalMesh->SetRenderCustomDepth(true);
			if (Mesh)Mesh->SetRenderCustomDepth(true);
		}
	}
}

// Player Exited The Pickup Area
void AItemPickup::OnEndOverlap(AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL) && Cast<ARadeCharacter>(OtherActor) != NULL)
	{
		// BP Event that player Exited
		BP_PlayerExited(Cast<ARadeCharacter>(OtherActor));

		// Clean Pickup reference in player class 
		if (Cast<ARadeCharacter>(OtherActor)->currentPickup == this)
			Cast<ARadeCharacter>(OtherActor)->currentPickup = NULL;

		// Disable pickup highlight
		if (SkeletalMesh)SkeletalMesh->SetRenderCustomDepth(false);
		if (Mesh)Mesh->SetRenderCustomDepth(false);
	}
}

// The Actual event of pickup
void AItemPickup::PickedUp(AActor * Player)
{
	// check the distance between pickup and player 
	if (Player && Item != NULL && Cast<ARadeCharacter>(Player)->TheInventory && FVector::Dist(GetActorLocation(), Player->GetActorLocation())<PickupDistance)
	{
		//  Add item to inventory
		Cast<ARadeCharacter>(Player)->TheInventory->ItemPickedUp(this);

		// BP event that item was picked up in player blueprint
		BP_PickedUp(Cast<ARadeCharacter>(Player));
	}
	Destroy();
}

void AItemPickup::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AItemPickup, Mesh);
	DOREPLIFETIME(AItemPickup, SkeletalMesh);
}