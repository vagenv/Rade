// Copyright 2015 Vagen Ayrapetyan

#pragma once

#include "Weapon/Weapon.h"
#include "LauncherWeapon.generated.h"

UCLASS()
class RADE_API ALauncherWeapon : public AWeapon
{
	GENERATED_BODY()
	
public:

	// Fire Event 
	virtual void Fire()override;

	// Projectile Class
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fire Projectile")
		TSubclassOf <class AProjectile> GrenadeArchetype;
	
};
