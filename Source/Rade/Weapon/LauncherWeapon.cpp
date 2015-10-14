// Copyright 2015 Vagen Ayrapetyan

#include "Rade.h"

#include "Weapon/LauncherWeapon.h"
#include "Weapon/Projectile.h"

void ALauncherWeapon::Fire()
{
	Super::Fire();


	if (GrenadeArchetype)
	{
		UWorld* const World = GetWorld();
		if (World && Mesh1P)
		{
			// Set Spawn Paramets
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = Instigator;

			// Spawn at socket location
			AProjectile* TheProjectile = World->SpawnActor<AProjectile>(GrenadeArchetype, Mesh1P->GetSocketLocation(TEXT("MuzzleFlashSocket")), Mesh1P->GetSocketRotation(TEXT("MuzzleFlashSocket")));

			// Set Projectile Velocity
			if (TheProjectile)
			{
				TheProjectile->InitVelocity(Mesh1P->GetSocketRotation(TEXT("MuzzleFlashSocket")).Vector());
			}
		}
	}
}

