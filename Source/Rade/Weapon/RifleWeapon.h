// Copyright 2015 Vagen Ayrapetyan

#pragma once

#include "Weapon/Weapon.h"
#include "RifleWeapon.generated.h"

/**
 * 
 */
UCLASS()
class RADE_API ARifleWeapon : public AWeapon
{
	GENERATED_BODY()
	
		virtual void Fire()override;

};
