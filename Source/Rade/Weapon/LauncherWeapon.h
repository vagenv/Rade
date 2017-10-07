// Copyright 2015-2017 Vagen Ayrapetyan

#pragma once

#include "Weapon/Weapon.h"
#include "LauncherWeapon.generated.h"


// Base for Projecitle Launcher Weapon
UCLASS()
class RADE_API ALauncherWeapon : public AWeapon
{
	GENERATED_BODY()
public:
	ALauncherWeapon(const class FObjectInitializer& PCIP);
	// Fire Event 
	virtual void Fire()override;

	// Projectile Class
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Launcher Weapon")
		TSubclassOf <class AProjectile> GrenadeArchetype;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Launcher Weapon")
		float ProjectileVelocity=500;
};
