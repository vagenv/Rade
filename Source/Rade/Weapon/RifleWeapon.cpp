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
		//	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, Hit.GetActor()->GetName());

		ABaseCharacter* EnemyPlayer = Cast<ABaseCharacter>(Hit.GetActor()); // typecast to the item class to the hit actor
		if (EnemyPlayer)
		{
			BP_HitEnemy(Hit);
			//printr("Apply Damage");
			//EnemyPlayer->TakeDamage(Player, currentWeapon->MainDamage, 0);
			EnemyPlayer->TakeDamage(MainFire.FireDamage, FDamageEvent(), ThePlayer->Controller, ThePlayer);


			// Hit event

			// Spawn blood effect
			//UGameplayStatics::SpawnEmitterAtLocation(this, HitFX, Hit.Location, Hit.Normal.Rotation(), true);

		}
	}


	/*
	/// Beam Weapon
	if (Cast<UBeamWeapon>(currentWeapon))
	{
	// Casting

	FVector CamLoc;
	FRotator CamRot;
	Cast<AInventoryTutorialCharacter>(Player)->Controller->GetPlayerViewPoint(CamLoc, CamRot); // Get the camera position and rotation
	const FVector StartTrace = Mesh->GetSocketLocation(TEXT("FireSocket"));
	//	CamLoc; // trace start is the camera location
	const FVector Direction = CamRot.Vector();
	const FVector EndTrace = StartTrace + Direction * Cast<UBeamWeapon>(currentWeapon)->MainFireDistance;

	// Perform trace to retrieve hit info
	FCollisionQueryParams TraceParams(FName(TEXT("WeaponTrace")), true, this);
	TraceParams.bTraceAsyncScene = true;
	TraceParams.bReturnPhysicalMaterial = true;
	TraceParams.AddIgnoredActor(Player);

	FHitResult Hit(ForceInit);
	if (GetWorld()->LineTraceSingle(Hit, StartTrace, EndTrace, ECC_WorldStatic, TraceParams))
	{

	//	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, Hit.GetActor()->GetName());

	AWarrior* EnemyPlayer = Cast<AWarrior>(Hit.GetActor()); // typecast to the item class to the hit actor
	if (EnemyPlayer)
	{

	EnemyPlayer->MyTakeDamage(Player, currentWeapon->MainDamage, 0);

	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, Hit.BoneName.GetPlainNameString());
	DrawDebugLine(
	GetWorld(),
	StartTrace,
	Hit.Location,
	FColor(255, 0, 0),
	false,
	3,
	0,
	1
	);
	}
	}

	}
	*/



	/*
	// try and fire a projectile
	if (ProjectileClass != NULL)
	{
	const FRotator SpawnRotation = GetControlRotation();
	// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
	const FVector SpawnLocation = GetActorLocation() + SpawnRotation.RotateVector(GunOffset);

	UWorld* const World = GetWorld();
	if (World != NULL)
	{
	// spawn the projectile at the muzzle
	World->SpawnActor<AShooterProjectile>(ProjectileClass, SpawnLocation, SpawnRotation);
	}
	}

	// try and play the sound if specified
	if (FireSound != NULL)
	{
	UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}
	*/
}





