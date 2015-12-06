// Copyright 2015 Vagen Ayrapetyan

#include "Rade.h"

#include "Character/RadeCharacter.h"
#include "Item/Inventory.h"
#include "Item/Item.h"
#include "UnrealNetwork.h"

ARadeCharacter::ARadeCharacter(const class FObjectInitializer& PCIP):Super(PCIP)
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

	CharacterName = "Rade Character";
}

// Called when the game starts or when spawned
void ARadeCharacter::BeginPlay()
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

	FTimerHandle PostBeginPlayHandle;
	GetWorldTimerManager().SetTimer(PostBeginPlayHandle, this, &ARadeCharacter::PostBeginPlay, 1,false);

}

void ARadeCharacter::PostBeginPlay()
{


}


float ARadeCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser)
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
void ARadeCharacter::ServerDie()
{
	if (Health>0 && !bDead)return;
	bDead = true;
	GlobalDeath();
}

// Implementation on all Clients
void ARadeCharacter::GlobalDeath_Implementation()
{
	ForceRagdoll();
	BP_Death();
}

// Enable Ragdoll
void ARadeCharacter::ForceRagdoll()
{
	Cast<USkeletalMeshComponent>(GetMesh())->BodyInstance.SetCollisionProfileName("Ragdoll");
	Cast<USkeletalMeshComponent>(GetMesh())->SetSimulatePhysics(true);
}


void ARadeCharacter::Landed(const FHitResult& Hit)
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
void ARadeCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARadeCharacter, Health);

	DOREPLIFETIME(ARadeCharacter, TheInventory);
	DOREPLIFETIME(ARadeCharacter, TheWeapon);

	DOREPLIFETIME(ARadeCharacter, bDead);

	DOREPLIFETIME(ARadeCharacter, CharacterName);
	DOREPLIFETIME(ARadeCharacter, CharacterColor);
}