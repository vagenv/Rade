// Copyright 2015 Vagen Ayrapetyan

#include "Rade.h"

#include "Weapon/ShotGunWeapon.h"
#include "Character/RadeCharacter.h"



void AShotGunWeapon::Fire()
{
	Super::Fire();
	// Casting 
	FVector CamLoc;
	FRotator CamRot;
	Cast<ARadeCharacter>(ThePlayer)->Controller->GetPlayerViewPoint(CamLoc, CamRot); // Get the camera position and rotation
	const FVector StartTrace = Mesh1P->GetSocketLocation(TEXT("MuzzleFlashSocket"));
	//	CamLoc; // trace start is the camera location
	const FVector Direction = CamRot.Vector();
	const FVector EndTrace = StartTrace + Direction *MainFire.FireDistance;

	float localx;
	float localy;
	FVector offsetVector;
	for (int32 i = 0; i < BallsPerShot; i++)
	{
		localx = FMath::FRandRange(-1, 1);
		localy = FMath::FRandRange(-1, 1);
		if (FMath::Pow(localx, 2) + FMath::Pow(localy, 2) > 1)
		{
			localx /= 2;
			localy /= 2;
		}

		// Cast lines
		offsetVector = GetActorForwardVector()*localx*SpreadValue + GetActorUpVector()*localy*SpreadValue;

		if (bDrawDebugLines)DrawDebugLine(
		GetWorld(),
		StartTrace,
		EndTrace+offsetVector,
		FColor(255, 0, 0),
		false,
		3,
		0,
		1
		);
	
		// Perform trace to retrieve hit info
		FCollisionQueryParams TraceParams(FName(TEXT("WeaponTrace")), true, this);
		TraceParams.bTraceAsyncScene = true;
		TraceParams.bReturnPhysicalMaterial = true;
		TraceParams.AddIgnoredActor(ThePlayer);




		FHitResult Hit(ForceInit);
		if (GetWorld()->LineTraceSingleByChannel(Hit, StartTrace, EndTrace + offsetVector, ECC_WorldStatic, TraceParams))
		{

			//	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, Hit.GetActor()->GetName());

			ABaseCharacter* EnemyPlayer = Cast<ABaseCharacter>(Hit.GetActor()); // typecast to the item class to the hit actor
			if (EnemyPlayer)
			{

				//EnemyPlayer->TakeDamage(Player, currentWeapon->MainDamage, 0);
				EnemyPlayer->TakeDamage(MainFire.FireDamage, FDamageEvent(), ThePlayer->Controller, ThePlayer);


				// Spawn blood effect
				//UGameplayStatics::SpawnEmitterAtLocation(this, HitFX, Hit.Location, Hit.Normal.Rotation(), true);

			}
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
}
