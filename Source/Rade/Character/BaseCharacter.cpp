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


	FRichCurve* FallDamageCurveData = FallDamageCurve.GetRichCurve();
	FallDamageCurveData->AddKey(FallAcceptableValue, 10);
	FallDamageCurveData->AddKey(1500 ,40);
	FallDamageCurveData->AddKey(2000, 100);

}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (Role >= ROLE_Authority && GetWorld())
	{
		//!TheInventory && 
		FActorSpawnParameters SpawnParam;
		SpawnParam.Owner = this;
		SpawnParam.Instigator = Instigator;
		TheInventory = GetWorld()->SpawnActor<AInventory>(SpawnParam);

		// Add Default Inventory
		if (TheInventory)
		{
			for (int32 i = 0; i< DefaultInventoryItems.Num(); i++)
			{
				TheInventory->AddItem(DefaultInventoryItems[i]);
			}
		}
	
			
	}	
}

float ABaseCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser)
{
	if (bDead)
	{
		return 0;
	}
	Health -= DamageAmount;


	//print("Health Left:  "+FString::FromInt(Health));

	if (Health <= 0)
	{
		Health = 0;
		Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
		ServerDie();
		//print("Character died");
		//if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Character Died"));
	}
	/*
	*/

	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}
void ABaseCharacter::ServerDie()
{
	if (Health>0 && !bDead)return;
	bDead = true;
	GlobalDeath();
}

void ABaseCharacter::GlobalDeath_Implementation()
{
	//if (TheInventory)TheInventory->DropEverything();
	//GetMovementComponent()->Deactivate();
	ForceRagdoll();
	BP_Death();
}

void ABaseCharacter::ForceRagdoll()
{
	//printg("The ID is :  "+FString::FromInt(GetUniqueID()));
	//GetCapsuleComponent()->BodyInstance.SetCollisionProfileName("NoCollision");
	Cast<USkeletalMeshComponent>(GetMesh())->BodyInstance.SetCollisionProfileName("Ragdoll");
	Cast<USkeletalMeshComponent>(GetMesh())->SetSimulatePhysics(true);
}

void ABaseCharacter::CurrentWeaponUpdated(){
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

void ABaseCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
//	DOREPLIFETIME(ABaseCharacter, Mesh);
	DOREPLIFETIME(ABaseCharacter, TheInventory);
	DOREPLIFETIME(ABaseCharacter, bDead);
	DOREPLIFETIME(ABaseCharacter, TheWeapon);
	DOREPLIFETIME(ABaseCharacter, Health);
}