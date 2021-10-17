// Copyright 2015-2021 Vagen Ayrapetyan

#include "ItemPickup.h"
#include "Inventory.h"
#include "Item.h"
#include "../Character/RadePlayer.h"
#include "../Rade.h"

#include "Net/UnrealNetwork.h"


// Sets default values
AItemPickup::AItemPickup(const class FObjectInitializer& PCIP)
	: Super(PCIP)
{
	// Set Root Component
	RootComponent = PCIP.CreateDefaultSubobject<USceneComponent>(this, TEXT("RootComponent"));
	RootComponent->SetIsReplicated(true);
   SetRootComponent(RootComponent);

	// Set Skeletal Mesh Component
	SkeletalMesh = PCIP.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("SkeletalMesh"));
	SkeletalMesh->SetIsReplicated(true);
	SkeletalMesh->BodyInstance.SetCollisionProfileName("BlockAll");
	SkeletalMesh->SetSimulatePhysics(true);
   SkeletalMesh->SetupAttachment (GetRootComponent ());
	
	// Set Static Mesh Component
	Mesh = PCIP.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("Mesh"));
	Mesh->SetIsReplicated(true);
	Mesh->BodyInstance.SetCollisionProfileName("BlockAll");
	Mesh->SetSimulatePhysics(true);
	Mesh->bAutoActivate = true;
   Mesh->SetupAttachment (GetRootComponent ());

	// Set Trigger Component
	TriggerSphere = PCIP.CreateDefaultSubobject<USphereComponent>(this, TEXT("TriggerSphere"));
	TriggerSphere->InitSphereRadius(300);
	TriggerSphere->SetIsReplicated(true);
	TriggerSphere->BodyInstance.SetCollisionProfileName("Pickup");

	bReplicates = true;
	SetReplicatingMovement (true);
}


void AItemPickup::BeginPlay()
{
	Super::BeginPlay();

	// Disable Skeletal Mesh Outline
	if (SkeletalMesh) {
		SkeletalMesh->SetRenderCustomDepth(false);
		SkeletalMesh->SetIsReplicated(true);
	}

	// Disable Static Mesh Outline
	if (Mesh) Mesh->SetRenderCustomDepth(false);


	// Enable overlap After A Delay
	FTimerHandle MyHandle;
	GetWorldTimerManager().SetTimer(MyHandle, this, &AItemPickup::ActivatePickupOverlap, PickupActivationDelay, false);


	// If Mesh is Set , Activate Pickup
	if (  (Mesh         && Mesh->GetStaticMesh ())
		|| (SkeletalMesh && SkeletalMesh->SkeletalMesh)) {
		ActivatePickupPhysics();

	// Else Activate it after a delay
	} else  {
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
	if (Mesh) Mesh->DestroyComponent();

	// Attach Trigger to Skeletal Mesh Component
	TriggerSphere->AttachToComponent(SkeletalMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, NAME_None);

	// Set Its Properties
	SkeletalMesh->SetMobility(EComponentMobility::Movable);
	SkeletalMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	SkeletalMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

	// Set It as Root
	SetRootComponent(SkeletalMesh);

	// Enable Replication of Its Movement
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
	TriggerSphere->AttachToComponent(Mesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, NAME_None);

	// Set Its Properties
	Mesh->SetMobility(EComponentMobility::Movable);
	Mesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	Mesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

	// Set It as Root
	SetRootComponent(Mesh);

	// Enable Replication of Its Movement
	SetReplicateMovement(true);
}

// Activate Overlap detection
void AItemPickup::ActivatePickupOverlap()
{
	// Check if Any Player is Within the range to pickup this actor
	if (GetLocalRole() >= ROLE_Authority) {
		for (TActorIterator<ARadePlayer> RadeActorItr(GetWorld()); RadeActorItr; ++RadeActorItr) {
			if (  RadeActorItr
				&& TriggerSphere
				&& FVector::Dist(RadeActorItr->GetActorLocation(), GetActorLocation()) < TriggerSphere->GetUnscaledSphereRadius())
			{
				OnBeginOverlap(nullptr,*RadeActorItr, nullptr, 0, false, FHitResult());
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
	if (Mesh->GetStaticMesh ()) SetAsMeshPickup();

	// Enable As Skeletal Mesh Pickup
	else if (SkeletalMesh->SkeletalMesh) SetAsSkeletalMeshPickup();
}

// Player Entered The Pickup Area
void AItemPickup::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, 
                                 AActor* OtherActor, 
                                 UPrimitiveComponent* OtherComp, 
                                 int32 OtherBodyIndex, 
                                 bool bFromSweep, 
                                 const FHitResult & SweepResult)
{
	ARadePlayer *radePlayer = Cast<ARadePlayer>(OtherActor);
	if (radePlayer != nullptr && 
       OtherActor != this && 
       radePlayer->TheInventory != nullptr)
	{
		// BP Event that player entered
		BP_PlayerEntered(radePlayer);

		// Auto give player the item
		if (bAutoPickup) {
			if (GetLocalRole() >= ROLE_Authority) PickedUp(radePlayer);

		// Wait player Input
		} else {
			// Set player ref of item pickup
			//Cast<ARadePlayer>(OtherActor)->currentPickup = this;
			if (radePlayer->IsLocallyControlled()) {
				if (SkeletalMesh != nullptr) SkeletalMesh->SetRenderCustomDepth(true);
				if (Mesh != nullptr)         Mesh->SetRenderCustomDepth(true);
			}
			if (GetLocalRole() >= ROLE_Authority)
				radePlayer->currentPickup = this;
		}
	}
}

// Player Exited The Pickup Area
void AItemPickup::OnEndOverlap(UPrimitiveComponent* OverlappedComponent,AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ARadePlayer *radePlayer = Cast<ARadePlayer>(OtherActor);
	if (OtherActor != this && radePlayer != nullptr) {
		// BP Event that player Exited
		BP_PlayerLeft(radePlayer);

		
		// Clean Pickup reference in player class
		if (GetLocalRole() >= ROLE_Authority && radePlayer->currentPickup == this)
		radePlayer->currentPickup = nullptr;

		// Enable Highlighting on Local Users
		if (radePlayer->IsLocallyControlled()) {
			if (SkeletalMesh != nullptr) SkeletalMesh->SetRenderCustomDepth(false);
			if (Mesh			  != nullptr) Mesh->SetRenderCustomDepth(false);
		}
	}
}

// The Actual event of pickup
void AItemPickup::PickedUp(ARadePlayer * Player)
{
	// check the distance between pickup and player 
	if (  Player != nullptr
		&& Item != nullptr
		&& Player->TheInventory != nullptr) {
		//  Add item to inventory
		Player->TheInventory->ItemPickedUp(this);

		// BP event that item was picked up in player blueprint
		BP_PickedUp(Player);
	}
	Destroy();
}