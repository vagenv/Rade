// Copyright 2015 Vagen Ayrapetyan

#pragma once

#include "Weapon/Weapon.h"
#include "ShotGunWeapon.generated.h"

// Base for any Shotgun Weapon
UCLASS()
class RADE_API AShotGunWeapon : public AWeapon
{
	GENERATED_BODY()
public:

	// Override The Fire Event
	virtual void Fire()override;

	// Cone fire Spead Angle (In Degres)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
		float SpreadAngle=45;

	// Number of Fires per Shot
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
		int32 BallsPerShot;
	
};
