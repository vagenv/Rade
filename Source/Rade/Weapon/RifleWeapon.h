// Copyright 2015-2021 Vagen Ayrapetyan

#pragma once

#include "Weapon.h"
#include "RifleWeapon.generated.h"

// Base for any Rifle weapon
UCLASS()
class RADE_API ARifleWeapon : public AWeapon
{
	GENERATED_BODY()
public:
	ARifleWeapon(const class FObjectInitializer& PCIP);
	// Main Fire Event
	virtual void Fire()override;
};
