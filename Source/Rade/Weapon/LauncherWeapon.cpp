// Copyright 2015-2016 Vagen Ayrapetyan

#include "Rade.h"

#include "Character/RadePlayer.h"

#include "Weapon/LauncherWeapon.h"
#include "Weapon/Projectile.h"


ALauncherWeapon::ALauncherWeapon(const class FObjectInitializer& PCIP) : Super(PCIP)
{
	ItemName = "Launcher";
}


void ALauncherWeapon::Fire()
{
	Super::Fire();

	if (GrenadeArchetype)
	{
		UWorld* const World = GetWorld();
		if (World && Mesh1P && ThePlayer && ThePlayer->Controller)
		{
			// Get Camera Rotation
			FVector CamLoc;
			FRotator CamRot;
			ThePlayer->Controller->GetPlayerViewPoint(CamLoc, CamRot);

			// Calculate Origin and Direction of Fire
			const FVector StartTrace = GetFireSocketTransform().GetLocation();
			const FVector Direction = CamRot.Vector();

			// Set Spawn Paramets
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = Instigator;


			// Spawn at Fire socket location
			AProjectile* TheProjectile = World->SpawnActor<AProjectile>(GrenadeArchetype, StartTrace, CamRot);

			// Set Projectile Velocity
			if (TheProjectile) 
			{
				
				TheProjectile->InitVelocity(Direction*ProjectileVelocity);
			}
				
		}
	}
}

