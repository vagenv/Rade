// Copyright 2015 Vagen Ayrapetyan

#pragma once

#include "Weapon/Weapon.h"
#include "ConstructorWeapon.generated.h"

/**
 * 
 */
UCLASS()
class RADE_API AConstructorWeapon : public AWeapon
{
	GENERATED_BODY()
	
	
public:
	AConstructorWeapon(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay()override;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Constructor Weapon")
		TSubclassOf <class ALevelBlock> BlockArchetype;


	UPROPERTY()
	class ALevelBlockConstructor* TheLevelBlockConstructor;

	FTimerHandle DrawCubeHandle;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DrawBox")
		float BoxDrawSize = 50;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DrawBox")
		float BoxDrawUpdate = 0.2f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DrawBox")
		FColor DrawBoxColor = FColor::White;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DrawBox")
		float DrawBoxThickness = 4;



	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Constructor Weapon")
		bool bAutoDestroyBlocks = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Constructor Weapon")
		float BlockRestoreTime = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Constructor Weapon")
		bool bRestoreAmmoAfterBlockDestroy = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Constructor Weapon")
		bool bRestoreAmmo = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Constructor Weapon")
		float AmmoRestoreTime = 0.4f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Constructor Weapon")
		float AmmoRestoreValue = 1;


protected:
	virtual void Fire()override;
	virtual void AltFire()override;

	void AmmoRestore();
	FTimerHandle AmmoRestoreHandle;

	virtual void DrawBox();

public:

	UFUNCTION(Client, Reliable)
	void Client_EnableDrawBox();
	virtual void Client_EnableDrawBox_Implementation();

	UFUNCTION(Client, Reliable)
	void Client_DisableDrawBox();
	virtual void Client_DisableDrawBox_Implementation();


	virtual void EquipEnd()override;
	virtual void UnEquipStart()override;



};
