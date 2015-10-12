// Copyright 2015 Vagen Ayrapetyan

#include "Rade.h"
#include "Weapon/RifleWeapon.h"
#include "Character/RadeCharacter.h"


void ARifleWeapon::Fire()
{
	Super::Fire();

	// Casting 
	FVector CamLoc;
	FRotator CamRot;
	ThePlayer->Controller->GetPlayerViewPoint(CamLoc, CamRot); // Get the camera position and rotation
	const FVector StartTrace = Mesh1P->GetSocketLocation(TEXT("MuzzleFlashSocket"));
	//	CamLoc; // trace start is the camera location
	const FVector Direction = CamRot.Vector();
	const FVector EndTrace = StartTrace + Direction *MainFire.FireDistance;

	// Perform trace to retrieve hit info
	FCollisionQueryParams TraceParams(FName(TEXT("WeaponTrace")), true, this);
	TraceParams.bTraceAsyncScene = true;
	TraceParams.bReturnPhysicalMaterial = true;
	TraceParams.AddIgnoredActor(ThePlayer);

	//print("Fire Event");
	FHitResult Hit(ForceInit);
	if (GetWorld()->LineTraceSingleByChannel(Hit, StartTrace, EndTrace, ECC_WorldStatic, TraceParams))
	{
		//printr("Hit Something");

		ABaseCharacter* EnemyPlayer = Cast<ABaseCharacter>(Hit.GetActor()); // typecast to the item class to the hit actor
		if (EnemyPlayer)
		{
			BP_HitEnemy(Hit);
			EnemyPlayer->TakeDamage(MainFire.FireDamage, FDamageEvent(), ThePlayer->Controller, ThePlayer);
		}
	}
}





