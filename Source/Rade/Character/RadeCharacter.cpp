// Copyright 2015-2016 Vagen Ayrapetyan

#include "Rade.h"

#include "Character/RadeCharacter.h"
#include "Character/RadeAnimInstance.h"

#include "Item/Inventory.h"
#include "Item/Item.h"

#include "Weapon/Weapon.h"

#include "UnrealNetwork.h"

#include "System/RadePlayerState.h"



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//						Base 


ARadeCharacter::ARadeCharacter(const class FObjectInitializer& PCIP):Super(PCIP)
{

	bDead = false;
	bReplicates = true;

	// Default fall damage Curve
	FRichCurve* FallDamageCurveData = FallDamageCurve.GetRichCurve();
	FallDamageCurveData->AddKey(FallAcceptableValue, 10);
	FallDamageCurveData->AddKey(1500 ,40);
	FallDamageCurveData->AddKey(2000, 100);

	TheInventory = CreateDefaultSubobject<UInventory>(TEXT("The Inventory"));
	TheInventory->SetIsReplicated(true);

	CharacterName = "Rade Character";
}

// Called when the game starts or when spawned
void ARadeCharacter::BeginPlay()
{
	Super::BeginPlay();

	Jump();

	bDead = false;
	// Get Third Person Anim Instance
	if (GetMesh() && GetMesh()->GetAnimInstance() && Cast<URadeAnimInstance>(GetMesh()->GetAnimInstance()))
		BodyAnimInstance = Cast<URadeAnimInstance>(GetMesh()->GetAnimInstance());

	// Get Default Third Person Mesh Relative Location and Rotation
	if (GetMesh())
	{
		Mesh_DefaultRelativeLoc = GetMesh()->RelativeLocation;
		Mesh_DefaultRelativeRot = GetMesh()->RelativeRotation;
	}

	if (BodyAnimInstance)BodyAnimInstance->TheCharacter = this;


	if (Role >= ROLE_Authority && GetWorld())
	{
		// Spawning Inventory At Server
		if (!TheInventory)
		{
		//	printr("No Inventory");
			TheInventory = GetWorld()->SpawnActor<UInventory>();
		}

		// Add Default Inventory Items
		if (TheInventory )
		{
			// Set player Ref in Inventory
			TheInventory->TheCharacter = this;

			if ( DefaultInventoryItems.Num() > 0)
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

	// Get Player Movement Component
	if (Cast<UCharacterMovementComponent>(GetMovementComponent()))
	{
		CharacterMovementComponent = Cast<UCharacterMovementComponent>(GetMovementComponent());
	}

	FTimerHandle PostBeginPlayHandle;
	GetWorldTimerManager().SetTimer(PostBeginPlayHandle, this, &ARadeCharacter::PostBeginPlay, 1,false);

}

void ARadeCharacter::PostBeginPlay(){
}


//				 Called to equip new Weapon
void ARadeCharacter::EquipWeapon(AWeapon* NewWeaponClass)
{
	if (!NewWeaponClass)return;

	// If player has a weapon, Unequip it
	if (TheWeapon)
		TheWeapon->Destroy();


	Global_SetAnimArchtype(NewWeaponClass->AnimArchetype);

	// Set Animation state
	ServerSetAnimID(EAnimState::Equip);

	// Spawn new weapon
	TheWeapon = GetWorld()->SpawnActor<AWeapon>(NewWeaponClass->GetClass());
	
	if (TheWeapon)
	{
		TheWeapon->SetOwner(this);
		TheWeapon->Mesh3P->AttachTo(GetMesh(), FName("WeaponSocket"));
	}

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//											Take Damage , Death/Reveive


//				Take Damage
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

		if (PlayerState && Cast<ARadePlayerState>(PlayerState))
		{
			Cast<ARadePlayerState>(PlayerState)->DeathCount++;
		}
		if (EventInstigator && EventInstigator->GetPawn() && EventInstigator->GetPawn()->PlayerState && Cast<ARadePlayerState>(EventInstigator->GetPawn()->PlayerState))
		{
			Cast<ARadePlayerState>(EventInstigator->GetPawn()->PlayerState)->KillCount++;
		}

		ServerDie();
	}


	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

//			Server Death
void ARadeCharacter::ServerDie()
{
	if (Health>0 || bDead)return;

	// Throw out Inventory Items on Death
	if (TheInventory && TheInventory->bDropItemsOnDeath)
		TheInventory->ThrowOutAllItems();


	// If Player can revive, revive hit after a delay
	if (bCanRevive)
	{
		FTimerHandle MyHandle;
		GetWorldTimerManager().SetTimer(MyHandle, this, &ARadeCharacter::ServerRevive, ReviveTime, false);
	}


	bDead = true;
	GlobalDeath();
}

// Implementation on all Clients
void ARadeCharacter::GlobalDeath_Implementation()
{

	// save third person mesh Relative Location and rotation before ragdoll
	if (GetMesh())
	{
		Mesh_DefaultRelativeLoc = GetMesh()->RelativeLocation;
		Mesh_DefaultRelativeRot = GetMesh()->RelativeRotation;
	}



	GetCapsuleComponent()->BodyInstance.SetCollisionProfileName("NoCollision");
	Cast<USkeletalMeshComponent>(GetMesh())->SetSimulatePhysics(true);

	

	ForceRagdoll();
	BP_CharacterDeath();
}



// Revive Player
void ARadeCharacter::ServerRevive()
{


	// Resoter Half of player health
	Health = MaxHealth / 2;
	bDead = false;
	if (TheWeapon)
	{
		TheWeapon->Destroy();
		TheWeapon = nullptr;
	}

	

	GetRootComponent()->SetWorldLocation(GetActorLocation() + FVector(0,0,60));

	GlobalRevive();

}



// Implementation on all Clients
void ARadeCharacter::GlobalRevive_Implementation(){
	BP_CharacterRevive();
	GetCapsuleComponent()->BodyInstance.SetCollisionProfileName("Pawn");
	// Restore Third Person Mesh to default State
	if (GetMesh())
	{
		GetMesh()->SetSimulatePhysics(false);
		GetMesh()->AttachTo(RootComponent);
		GetMesh()->RelativeLocation = Mesh_DefaultRelativeLoc;
		GetMesh()->RelativeRotation = Mesh_DefaultRelativeRot;
		GetMesh()->BodyInstance.SetCollisionProfileName("Pawn");
	}
	Global_SetAnimArchtype_Implementation(EAnimArchetype::EmptyHand);
}

// Enable Ragdoll
void ARadeCharacter::ForceRagdoll()
{
	Cast<USkeletalMeshComponent>(GetMesh())->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Cast<USkeletalMeshComponent>(GetMesh())->BodyInstance.SetCollisionProfileName("Ragdoll");
	Cast<USkeletalMeshComponent>(GetMesh())->SetSimulatePhysics(true);
}


// Character Landed on Ground
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


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//											 Network Anim

// Set Server Animation ID
bool ARadeCharacter::ServerSetAnimID_Validate(EAnimState AnimID){
	return true;
}
void ARadeCharacter::ServerSetAnimID_Implementation(EAnimState AnimID)
{
	// Set new State on all users
	Global_SetAnimID(AnimID);
}

// Set Global Animation ID
void ARadeCharacter::Global_SetAnimID_Implementation(EAnimState AnimID)
{
	// Set The Value in anim instances
	if (BodyAnimInstance)
		BodyAnimInstance->RecieveGlobalAnimID(AnimID);

}

//			Check Current Animation State 
bool ARadeCharacter::IsAnimState(EAnimState TheAnimState)
{
	if (BodyAnimInstance)
	{
		if (BodyAnimInstance->IsAnimState(TheAnimState))return true;
		else return false;
	}

	return true;
}
//			 Is Character In Air
bool ARadeCharacter::IsAnimInAir()
{
	// Check each air state separetly.
	if (IsAnimState(EAnimState::JumpEnd) || IsAnimState(EAnimState::Jumploop) || IsAnimState(EAnimState::JumpStart))return true;
	else return false;
}

void ARadeCharacter::Global_SetAnimArchtype_Implementation(EAnimArchetype newAnimArchetype)
{
	if (BodyAnimInstance)BodyAnimInstance->AnimArchetype = newAnimArchetype;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//											 Character Stats


void ARadeCharacter::OnRep_CharacterStatsUpdated() {
	BP_CharacterStatsUpdated();
}

bool ARadeCharacter::SetCharacterStats_Validate(const FString & newName, FLinearColor newColor)
{
	return true;
}

void ARadeCharacter::SetCharacterStats_Implementation(const FString & newName, FLinearColor newColor)
{

	CharacterName = newName;
	CharacterColor = newColor;

	if (Role >= ROLE_Authority)OnRep_CharacterStatsUpdated();
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