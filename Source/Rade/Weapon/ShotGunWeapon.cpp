// Copyright 2015 Vagen Ayrapetyan

#include "Rade.h"

#include "Weapon/ShotGunWeapon.h"
#include "Character/RadePlayer.h"


// The Fire Event
void AShotGunWeapon::Fire()
{
	Super::Fire();
	
	if (Mesh1P && ThePlayer && ThePlayer->Controller)
	{
		// Get Player Camera location and Rotation rotation 
		FVector CamLoc;
		FRotator CamRot;
		ThePlayer->Controller->GetPlayerViewPoint(CamLoc, CamRot); 

		// Get Mesh Fire Socket Location and calculatre Fire end point
		const FVector StartTrace = Mesh1P->GetSocketLocation(TEXT("MuzzleFlashSocket"));
		const FVector Direction = CamRot.Vector();
		const FVector EndTrace = StartTrace + Direction *MainFire.FireDistance;

		// Derivation by X and Y of each Fire
		float localx;
		float localy;

		FVector offsetVector;
		for (int32 i = 0; i < BallsPerShot; i++)
		{
			// Get Random withing range
			localx = FMath::FRandRange(-1, 1);
			localy = FMath::FRandRange(-1, 1);

			// If not withing circle, divide it
			if (FMath::Pow(localx, 2) + FMath::Pow(localy, 2) > 1)
			{
				localx /= 2;
				localy /= 2;
			}

			// The fire end offset  
			offsetVector = GetActorForwardVector()*localx*SpreadValue + GetActorUpVector()*localy*SpreadValue;


			// Set trace values and perform trace to retrieve hit info
			FCollisionQueryParams TraceParams(FName(TEXT("WeaponTrace")), true, this);
			TraceParams.bTraceAsyncScene = true;
			TraceParams.bReturnPhysicalMaterial = true;
			TraceParams.AddIgnoredActor(ThePlayer);
			FHitResult Hit(ForceInit);


			if (GetWorld()->LineTraceSingleByChannel(Hit, StartTrace, EndTrace + offsetVector, ECC_WorldStatic, TraceParams))
			{
				// Check if hit actor is player
				ARadeCharacter* EnemyPlayer = Cast<ARadeCharacter>(Hit.GetActor());
				if (EnemyPlayer)
				{
					// Hit Enemy
					BP_HitEnemy(Hit);

					// Apply Damage
					UGameplayStatics::ApplyDamage(EnemyPlayer, MainFire.FireDamage, ThePlayer->Controller, Cast<AActor>(this), UDamageType::StaticClass());
				}
			}
		}
	}	
}
