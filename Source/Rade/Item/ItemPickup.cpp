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

	bReplicates = true;
	//SetReplicates(true);

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
	//Mesh->SetIsReplicated(true);

	TriggerSphere = PCIP.CreateDefaultSubobject<USphereComponent>(this, TEXT("TriggerSphere"));
	TriggerSphere->InitSphereRadius(300);
	TriggerSphere->AttachParent = RootComponent;
	//TriggerSphere->SetIsReplicated(true);

}


void AItemPickup::BeginPlay()
{
	Super::BeginPlay();
	if (SkeletalMesh)SkeletalMesh->SetRenderCustomDepth(false);
	if (Mesh)Mesh->SetRenderCustomDepth(false);
//	printr("Pickup Spawned");
	if (Role < ROLE_Authority)
		return;

	SetReplicateMovement(true);
	SetReplicates(true);

	FTimerHandle MyHandle;
	GetWorldTimerManager().SetTimer(MyHandle, this, &AItemPickup::ActivatePickupOverlap, PickupCollisionDelay, false);

}
void AItemPickup::ActivatePickupOverlap()
{

	TriggerSphere->OnComponentBeginOverlap.AddDynamic(this, &AItemPickup::OnBeginOverlap);
	TriggerSphere->OnComponentEndOverlap.AddDynamic(this, &AItemPickup::OnEndOverlap);
}


void AItemPickup::ActivatePickup()
{

	if (Mesh->StaticMesh)
	{
		SkeletalMesh->DestroyComponent();
		TriggerSphere->AttachTo(Mesh);

		Mesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
		Mesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		Mesh->SetSimulatePhysics(true);
		Mesh->WakeRigidBody();
		//Mesh->SetIsReplicated(true);
	}
	else if (SkeletalMesh->SkeletalMesh)
	{
		Mesh->DestroyComponent();
		TriggerSphere->AttachTo(SkeletalMesh,NAME_None, EAttachLocation::SnapToTarget);

		SkeletalMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
		SkeletalMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		SkeletalMesh->SetSimulatePhysics(true);
		SkeletalMesh->WakeRigidBody();		
	//	SkeletalMesh->SetIsReplicated(true);
	}
}

void AItemPickup::OnBeginOverlap(AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL) && Cast<ARadeCharacter>(OtherActor) != NULL && Cast<ARadeCharacter>(OtherActor)->TheInventory!=NULL)
	{
		
		BP_PlayerEntered(Cast<ARadeCharacter>(OtherActor));
		if (bAutoPickup)
		{
			PickedUp(OtherActor);
		}
		else
		{
			Cast<ARadeCharacter>(OtherActor)->currentPickup = this;
			if (SkeletalMesh)SkeletalMesh->SetRenderCustomDepth(true);
			if (Mesh)Mesh->SetRenderCustomDepth(true);
		}
	}
}

void AItemPickup::OnEndOverlap(AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL) && Cast<ARadeCharacter>(OtherActor) != NULL)
	{
		BP_PlayerExited(Cast<ARadeCharacter>(OtherActor));
		if (Cast<ARadeCharacter>(OtherActor)->currentPickup == this)
		{
			Cast<ARadeCharacter>(OtherActor)->currentPickup = NULL;
			if (SkeletalMesh)SkeletalMesh->SetRenderCustomDepth(false);
			if (Mesh)Mesh->SetRenderCustomDepth(false);
		}
	}
}


void AItemPickup::PickedUp(AActor * Player)
{
	FVector CurrentLoc;
	if (SkeletalMesh)CurrentLoc = SkeletalMesh->GetComponentLocation();
	else if (Mesh)CurrentLoc = Mesh->GetComponentLocation();
	// Add Item
	if (Player && Item != NULL && Cast<ARadeCharacter>(Player)->TheInventory && FVector::Dist(CurrentLoc, Player->GetActorLocation())<PickupDistance)
	{
		Cast<ARadeCharacter>(Player)->TheInventory->ItemPickedUp(this);
		BP_PickedUp(Cast<ARadeCharacter>(Player));
		//Cast<ARadeCharacter>(Player)->TheInventory->AddItem(Item);
		//printg("Player Picked Up");
	}
	Destroy();
}

void AItemPickup::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AItemPickup, Mesh);
	DOREPLIFETIME(AItemPickup, SkeletalMesh);
}