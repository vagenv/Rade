// Copyright 2015 Vagen Ayrapetyan

#include "Rade.h"
#include "Character/RadePlayer.h"

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
	
	//SkeletalMesh->SetNetAddressable(); 
	//SkeletalMesh->SetIsReplicated(true);
	
	//SkeletalMesh->bRenderCustomDepth = true;
	//SkeletalMesh->MarkRenderStateDirty();
	//SkeletalMesh->SetRenderCustomDepth(false);

	Mesh = PCIP.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("Mesh"));
	Mesh->AttachParent = RootComponent;
	//Mesh->SetNetAddressable();
//	Mesh->SetIsReplicated(true);
//	Mesh->SetRenderCustomDepth(false);

	TriggerSphere = PCIP.CreateDefaultSubobject<USphereComponent>(this, TEXT("TriggerSphere"));
	TriggerSphere->InitSphereRadius(300);
	TriggerSphere->AttachParent = RootComponent;

	bReplicates = true;
	bReplicateMovement = true;
}


void AItemPickup::BeginPlay()
{
	Super::BeginPlay();

	SetReplicateMovement(true);
	SetReplicates(true);

	// Set Pickup Skeletal Mesh Outline
	if (SkeletalMesh)
	{
		SkeletalMesh->SetRenderCustomDepth(false);
		SkeletalMesh->SetIsReplicated(true);
	}

	// Set Pickup Mesh Outline
	if (Mesh)Mesh->SetRenderCustomDepth(false);


	if (Role < ROLE_Authority)
		return;


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

		Mesh->SetMobility(EComponentMobility::Movable);
		Mesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
		Mesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		Mesh->SetSimulatePhysics(true);
		Mesh->WakeRigidBody();
	}
	else if (SkeletalMesh->SkeletalMesh)
	{
		Mesh->DestroyComponent();
		TriggerSphere->AttachTo(SkeletalMesh,NAME_None, EAttachLocation::SnapToTarget);

		SkeletalMesh->SetMobility(EComponentMobility::Movable);
		SkeletalMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
		SkeletalMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		SkeletalMesh->SetSimulatePhysics(true);
		SkeletalMesh->WakeRigidBody();		
	}
}

// Player Entered The Pickup Area
void AItemPickup::OnBeginOverlap(AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr) && Cast<ARadePlayer>(OtherActor) != nullptr && Cast<ARadePlayer>(OtherActor)->TheInventory != NULL)
	{
		// BP Event that player entered
		BP_PlayerEntered(Cast<ARadePlayer>(OtherActor));

		// Auto give player the item
		if (bAutoPickup)
		{
			PickedUp(OtherActor);
		}

		// Wait player Input
		else
		{
			// Set player ref of item pickup
			Cast<ARadePlayer>(OtherActor)->currentPickup = this;

			ThePickupPlayer = Cast<ARadePlayer>(OtherActor);

			bIsHighlighted = true;
			OnRep_ToggleHighlighted();
		}
	}
}

// Player Exited The Pickup Area
void AItemPickup::OnEndOverlap(AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr) && Cast<ARadePlayer>(OtherActor) != nullptr)
	{
		// BP Event that player Exited
		BP_PlayerExited(Cast<ARadePlayer>(OtherActor));

		// Clean Pickup reference in player class 
		if (Cast<ARadePlayer>(OtherActor)->currentPickup == this)
			Cast<ARadePlayer>(OtherActor)->currentPickup = nullptr;

		// Disable pickup highligh
		bIsHighlighted = false;

		OnRep_ToggleHighlighted();

	//	ThePickupPlayer = nullptr;
	}
}

// The Actual event of pickup
void AItemPickup::PickedUp(AActor * Player)
{
	// check the distance between pickup and player 
	if (Player && Item != nullptr && Cast<ARadePlayer>(Player)->TheInventory )
	{
		//  Add item to inventory
		Cast<ARadePlayer>(Player)->TheInventory->ItemPickedUp(this);

		// BP event that item was picked up in player blueprint
		BP_PickedUp(Cast<ARadePlayer>(Player));
	}
	Destroy();
}


void AItemPickup::OnRep_ToggleHighlighted()
{
	if (ThePickupPlayer && ThePickupPlayer->IsLocallyControlled())
	{
		if (bIsHighlighted)
		{
		//	printg("Show");
			// Activate highlight
			if (SkeletalMesh)SkeletalMesh->SetRenderCustomDepth(true);
			if (Mesh)Mesh->SetRenderCustomDepth(true);
		}
		else
		{
			if (SkeletalMesh)SkeletalMesh->SetRenderCustomDepth(false);
			if (Mesh)Mesh->SetRenderCustomDepth(false);

			//printg("Hide");
		}
	}




}

void AItemPickup::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AItemPickup, Mesh);
	DOREPLIFETIME(AItemPickup, SkeletalMesh);

	DOREPLIFETIME(AItemPickup, ThePickupPlayer);

	DOREPLIFETIME(AItemPickup, bIsHighlighted);
}