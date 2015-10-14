// Copyright 2015 Vagen Ayrapetyan

#pragma once

#include "Weapon/Weapon.h"
#include "ShotGunWeapon.generated.h"

/**
 * 
 */
UCLASS()
class RADE_API AShotGunWeapon : public AWeapon
{
	GENERATED_BODY()
public:

	// Override The Fire Event
	virtual void Fire()override;

	// Cone fire Spead Value
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
		float SpreadValue;

	// Number of Fires
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
		int32 BallsPerShot;
	
};
