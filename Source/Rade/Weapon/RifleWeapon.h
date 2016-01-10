// Copyright 2015 Vagen Ayrapetyan

#pragma once

#include "Weapon/Weapon.h"
#include "RifleWeapon.generated.h"

// Base for any Rifle weapon
UCLASS()
class RADE_API ARifleWeapon : public AWeapon
{
	GENERATED_BODY()
	
	// Main Fire Event
	virtual void Fire()override;
};
