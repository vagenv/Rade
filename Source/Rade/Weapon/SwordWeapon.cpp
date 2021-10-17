// Copyright 2015-2021 Vagen Ayrapetyan

#include "SwordWeapon.h"
#include "../Rade.h"
#include "../Character/RadePlayer.h"


ASwordWeapon::ASwordWeapon(const class FObjectInitializer& PCIP) : Super(PCIP)
{
	// Set First Person Overlap BOX
	Mesh1P_MeleeAttackCollisionBox = PCIP.CreateDefaultSubobject<UBoxComponent>(this, TEXT("FirstPerson_MeleeBoxComponent"));
	Mesh1P_MeleeAttackCollisionBox->SetupAttachment(Mesh1P);
	Mesh1P_MeleeAttackCollisionBox->BodyInstance.SetCollisionProfileName("OverlapAll");
	Mesh1P_MeleeAttackCollisionBox->SetBoxExtent(FVector(5, 25, 70));

	// Set Third Person Overlap BOX
	Mesh3P_MeleeAttackCollisionBox = PCIP.CreateDefaultSubobject<UBoxComponent>(this, TEXT("ThirdPerson_MeleeBoxComponent"));
	Mesh3P_MeleeAttackCollisionBox->SetupAttachment(Mesh3P);
	Mesh3P_MeleeAttackCollisionBox->BodyInstance.SetCollisionProfileName("OverlapAll");
	Mesh3P_MeleeAttackCollisionBox->SetBoxExtent(FVector(5, 25, 70));

	ItemName = "Sword Weapon";
}

void ASwordWeapon::BeginPlay()
{
	Super::BeginPlay();
	
	// Disable Tracing at the start
	bTracingMeleeAttack = false;

	// Bind Overlap Box on server
	if (GetLocalRole() >= ROLE_Authority) {
		Mesh1P_MeleeAttackCollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ASwordWeapon::OnSwordWeaponBoxBeginOverlap);
		Mesh3P_MeleeAttackCollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ASwordWeapon::OnSwordWeaponBoxBeginOverlap);
	}
}

// Start Tracing
void ASwordWeapon::StartMeleeAttackTrace(){
	bTracingMeleeAttack = true;
}

// End Tracing
void ASwordWeapon::EndMeleeAttackTrace(){
	bTracingMeleeAttack = false;
	HitActors.Empty();
}

// Box Overlap Event
void ASwordWeapon::OnSwordWeaponBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent,AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	ARadeCharacter *radeOther = Cast<ARadeCharacter>(OtherActor);
	if (	OtherActor != this
		&& OtherActor != ThePlayer
		&& OtherComp  != nullptr
		&& GetOwner() != OtherActor
		&& radeOther != nullptr
		&& bTracingMeleeAttack)
	{
		// Check if character was already slashed
		if (!HitActors.Contains(OtherActor)) {
			// Add Character to Slashed List
			HitActors.Add(radeOther);

			// BP Event
			BP_HitEnemy(SweepResult);

         AController* instigator = NULL;
         if (ThePlayer && ThePlayer->Controller) instigator = ThePlayer->Controller;

         // Apply Damage
         UGameplayStatics::ApplyDamage(radeOther, MainFire.FireDamage, instigator, Cast<AActor>(this), UDamageType::StaticClass());
		}
	}
}
