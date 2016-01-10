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
	// Set Root Component
	RootComponent = PCIP.CreateDefaultSubobject<USceneComponent>(this, TEXT("RootComponent"));
	RootComponent->SetIsReplicated(true);

	// Set Skeletal Mesh Component
	SkeletalMesh = PCIP.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("SkeletalMesh"));
	SkeletalMesh->SetIsReplicated(true);
	SkeletalMesh->BodyInstance.SetCollisionProfileName("BlockAll");
	SkeletalMesh->SetSimulatePhysics(true);
	SkeletalMesh->AttachParent = RootComponent;
	
	// Set Static Mesh Component
	Mesh = PCIP.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("Mesh"));
	Mesh->SetIsReplicated(true);
	Mesh->BodyInstance.SetCollisionProfileName("BlockAll");
	Mesh->SetSimulatePhysics(true);
	Mesh->bAutoActivate = true;
	Mesh->AttachParent = RootComponent;;

	// Set Trigger Component
	TriggerSphere = PCIP.CreateDefaultSubobject<USphereComponent>(this, TEXT("TriggerSphere"));
	TriggerSphere->InitSphereRadius(300);
	TriggerSphere->SetIsReplicated(true);
	TriggerSphere->BodyInstance.SetCollisionProfileName("Pickup");

	bReplicates = true;
	bReplicateMovement = true;	
}


void AItemPickup::BeginPlay()
{
	Super::BeginPlay();

	// Disable Skeletal Mesh Outline
	if (SkeletalMesh)
	{
		SkeletalMesh->SetRenderCustomDepth(false);
		SkeletalMesh->SetIsReplicated(true);
	}

	// Disable Static Mesh Outline
	if (Mesh)Mesh->SetRenderCustomDepth(false);


	// Enable overlap After A Delay
	FTimerHandle MyHandle;
	GetWorldTimerManager().SetTimer(MyHandle, this, &AItemPickup::ActivatePickupOverlap, PickupActivationDelay, false);


	// If Mesh is Set , Activate Pickup
	if ((Mesh && Mesh->StaticMesh) || (SkeletalMesh && SkeletalMesh->SkeletalMesh))
		ActivatePickupPhysics();

	// Else Activate it after a delay
	else 
	{
		FTimerHandle MyActivatePhysicsHandle;
		GetWorldTimerManager().SetTimer(MyActivatePhysicsHandle, this, &AItemPickup::ActivatePickupPhysics, 0.1f, false);
	}

}

// Enable It as a Skeletal Mesh Pickup
bool AItemPickup::SetAsSkeletalMeshPickup_Validate(){
	return true;
}
void AItemPickup::SetAsSkeletalMeshPickup_Implementation()
{
	// Destroy Extra Component
	if (Mesh)Mesh->DestroyComponent();

	// Attach Trigger to Skeletal Mesh Component
	TriggerSphere->AttachTo(SkeletalMesh, NAME_None, EAttachLocation::SnapToTarget);

	// Set Its Properties
	SkeletalMesh->SetMobility(EComponentMobility::Movable);
	SkeletalMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	SkeletalMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

	// Set It as Root
	SetRootComponent(SkeletalMesh);

	// Enable Replicattion of Its Movement
	SetReplicateMovement(true);
}

// Enable It as a Static Mesh Pickup
bool AItemPickup::SetAsMeshPickup_Validate(){
	return true;
}
void AItemPickup::SetAsMeshPickup_Implementation()
{
	// Destroy Extra Component
	if (SkeletalMesh)SkeletalMesh->DestroyComponent();

	// Attach Trigger to Static Mesh Component
	TriggerSphere->AttachTo(Mesh, NAME_None, EAttachLocation::SnapToTarget);

	// Set Its Properties
	Mesh->SetMobility(EComponentMobility::Movable);
	Mesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	Mesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

	// Set It as Root
	SetRootComponent(Mesh);

	// Enable Replicattion of Its Movement
	SetReplicateMovement(true);
}

// Activate Overlap detection
void AItemPickup::ActivatePickupOverlap()
{
	// Check if Any Player is Within the range to pickup this actor
	if (Role >= ROLE_Authority)
	{
		for (TActorIterator<ARadePlayer> RadeActorItr(GetWorld()); RadeActorItr; ++RadeActorItr)
		{
			if (RadeActorItr && TriggerSphere && FVector::Dist(RadeActorItr->GetActorLocation(), GetActorLocation()) < TriggerSphere->GetUnscaledSphereRadius())
			{
				OnBeginOverlap(*RadeActorItr, nullptr, 0, false, FHitResult());
				return;
			}
		}
	}

	// Enable Overlap Component
	TriggerSphere->OnComponentBeginOverlap.AddDynamic(this, &AItemPickup::OnBeginOverlap);
	TriggerSphere->OnComponentEndOverlap.AddDynamic(this, &AItemPickup::OnEndOverlap);
}

// Activate Pickup Physics
void AItemPickup::ActivatePickupPhysics()
{
	// If Already activated (One of components Destroyed) Stop
	if (!Mesh || !SkeletalMesh)return;
	
	// Enable As Static Mesh Pickup
	if (Mesh->StaticMesh)
		SetAsMeshPickup();

	// Enable As Skeletal Mesh Pickup
	else if (SkeletalMesh->SkeletalMesh)
		SetAsSkeletalMeshPickup();		

}

// Player Entered The Pickup Area
void AItemPickup::OnBeginOverlap(AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if ((OtherActor != nullptr) && (OtherActor != this) && Cast<ARadePlayer>(OtherActor) != nullptr && Cast<ARadePlayer>(OtherActor)->TheInventory != nullptr)
	{
		// BP Event that player entered
		BP_PlayerEntered(Cast<ARadePlayer>(OtherActor));

		// Auto give player the item
		if (bAutoPickup)
		{
			if (Role >= ROLE_Authority)
				PickedUp(Cast<ARadePlayer>(OtherActor));
		}

		// Wait player Input
		else
		{
			// Set player ref of item pickup
			//Cast<ARadePlayer>(OtherActor)->currentPickup = this;
			if (Cast<ARadePlayer>(OtherActor)->IsLocallyControlled())
			{
				if (SkeletalMesh != nullptr)SkeletalMesh->SetRenderCustomDepth(true);
				if (Mesh != nullptr)Mesh->SetRenderCustomDepth(true);
			}
			if (Role >= ROLE_Authority)
				Cast<ARadePlayer>(OtherActor)->currentPickup = this;

		}
	}
}

// Player Exited The Pickup Area
void AItemPickup::OnEndOverlap(AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if ((OtherActor != nullptr) && (OtherActor != this) && Cast<ARadePlayer>(OtherActor) != nullptr)
	{
		// BP Event that player Exited
		BP_PlayerLeft(Cast<ARadePlayer>(OtherActor));

		
		// Clean Pickup reference in player class
		if (Role>=ROLE_Authority && Cast<ARadePlayer>(OtherActor)->currentPickup == this)
		Cast<ARadePlayer>(OtherActor)->currentPickup = nullptr;

		// Enable Highlighting on Local Users
		if (Cast<ARadePlayer>(OtherActor)->IsLocallyControlled())
		{
			if (SkeletalMesh != nullptr)SkeletalMesh->SetRenderCustomDepth(false);
			if (Mesh != nullptr)Mesh->SetRenderCustomDepth(false);
		}
	}
}

// The Actual event of pickup
void AItemPickup::PickedUp(ARadePlayer * Player)
{
	// check the distance between pickup and player 
	if (Player != nullptr && Item != nullptr && Player->TheInventory != nullptr)
	{
		//  Add item to inventory
		Player->TheInventory->ItemPickedUp(this);

		// BP event that item was picked up in player blueprint
		BP_PickedUp(Player);
	}
	Destroy();
}

/*
void AItemPickup::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

}

*/