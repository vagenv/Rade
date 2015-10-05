// Copyright 2015 Vagen Ayrapetyan

#include "Rade.h"

#include "Weapon/LauncherWeapon.h"
#include "Weapon/Projectile.h"

void ALauncherWeapon::Fire()
{

	Super::Fire();
	// try and fire a projectile
	if (GrenadeArchetype != NULL)
	{

		UWorld* const World = GetWorld();
		if (World && Mesh1P)
		{

			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = Instigator;
			// spawn the projectile at the muzzle
			//FVector loc = Mesh1P->GetSocketLocation(TEXT("MuzzleFlashSocket"));
			//FRotator rot = Mesh1P->GetSocketRotation(TEXT("MuzzleFlashSocket"));
		
		//	DrawDebugLine(GetWorld(),loc,loc+rot.Vector()*400,FColor::Red);

			AProjectile* theProjectile = World->SpawnActor<AProjectile>(GrenadeArchetype, Mesh1P->GetSocketLocation(TEXT("MuzzleFlashSocket")), Mesh1P->GetSocketRotation(TEXT("MuzzleFlashSocket")));
			//AProjectile* theProjectile = World->SpawnActor<AProjectile>(GrenadeArchetype,loc,rot );

			if (theProjectile)
			{
				// find launch direction
				theProjectile->InitVelocity(Mesh1P->GetSocketRotation(TEXT("MuzzleFlashSocket")).Vector());

			}
		}
	}
}

