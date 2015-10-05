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

	
		virtual void Fire()override;



	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
		float SpreadValue;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
		int32 BallsPerShot;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
		bool bDrawDebugLines = false;
	
};
