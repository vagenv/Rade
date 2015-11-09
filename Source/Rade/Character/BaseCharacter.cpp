// Copyright 2015 Vagen Ayrapetyan

#include "Rade.h"

#include "Character/BaseCharacter.h"
#include "Item/Inventory.h"
#include "Item/Item.h"
#include "UnrealNetwork.h"

ABaseCharacter::ABaseCharacter(const class FObjectInitializer& PCIP):Super(PCIP)
{

	bDead = false;
	bReplicates = true;


	// Default fall damage Curve
	FRichCurve* FallDamageCurveData = FallDamageCurve.GetRichCurve();
	FallDamageCurveData->AddKey(FallAcceptableValue, 10);
	FallDamageCurveData->AddKey(1500 ,40);
	FallDamageCurveData->AddKey(2000, 100);



	TheInventory = CreateDefaultSubobject<UInventory>(TEXT("TheInventory"));
	TheInventory->SetIsReplicated(true);
}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (Role >= ROLE_Authority && GetWorld())
	{
		// Spawning Inventory At Server
		if (!TheInventory)
		{
			printr("No Inventory");
			TheInventory = GetWorld()->SpawnActor<UInventory>();
		}

		// Add Default Inventory Items
		if (TheInventory)
		{
			for (int32 i = 0; i< DefaultInventoryItems.Num(); i++)
			{
				if (DefaultInventoryItems.IsValidIndex(i))
				{
					TheInventory->AddItem(DefaultInventoryItems[i]);
				}
					
			}
		}
	
			
	}	
}

float ABaseCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser)
{
	// 
	if (bDead)
	{
		return 0;
	}

	Health -= DamageAmount;

	// Death Event
	if (Health <= 0)
	{
		Health = 0;
		Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
		ServerDie();
	}


	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

// Server Death
void ABaseCharacter::ServerDie()
{
	if (Health>0 && !bDead)return;
	bDead = true;
	GlobalDeath();
}

// Implementation on all Clients
void ABaseCharacter::GlobalDeath_Implementation()
{
	ForceRagdoll();
	BP_Death();
}

// Enable Ragdoll
void ABaseCharacter::ForceRagdoll()
{
	Cast<USkeletalMeshComponent>(GetMesh())->BodyInstance.SetCollisionProfileName("Ragdoll");
	Cast<USkeletalMeshComponent>(GetMesh())->SetSimulatePhysics(true);
}


void ABaseCharacter::Landed(const FHitResult& Hit)
{
	// Player landed on ground

	FVector fallV = GetCharacterMovement()->Velocity;

	// Fall Damage
	if (fallV.GetAbs().Z>FallAcceptableValue)
	{
		FRichCurve* FallDamageCurveData = FallDamageCurve.GetRichCurve();
		//printr(FString::FromInt(fallV.GetAbs().Z));

		float dmg = 0;
		if (FallDamageCurveData)dmg = FallDamageCurveData->Eval((fallV.GetAbs().Z));
		TakeDamage(dmg, FDamageEvent(), Controller, this);
	}

	Super::Landed(Hit);
}

// Replication 
void ABaseCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABaseCharacter, TheInventory);
	DOREPLIFETIME(ABaseCharacter, bDead);
	DOREPLIFETIME(ABaseCharacter, TheWeapon);
	DOREPLIFETIME(ABaseCharacter, Health);
}