// Copyright 2015 Vagen Ayrapetyan

#pragma once

#include "Weapon/Weapon.h"
#include "ConstructorWeapon.generated.h"


UCLASS()
class RADE_API AConstructorWeapon : public AWeapon
{
	GENERATED_BODY()
	
public:

	AConstructorWeapon(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay()override;



	// Level Block Archetype
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Constructor Weapon")
		TSubclassOf <class ALevelBlock> BlockArchetype;

	// The Level Block Constructor
	UPROPERTY()
		class ALevelBlockConstructor* TheLevelBlockConstructor;

	// Cube Draw Timer Handle
	FTimerHandle DrawCubeHandle;

	// Box Draw Size
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DrawBox")
		float BoxDrawSize = 50;

	// Box Draw Update Speed
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DrawBox")
		float BoxDrawUpdate = 0.2f;

	// Box Draw Color
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DrawBox")
		FColor DrawBoxColor = FColor::White;

	// Box Draw thickness
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DrawBox")
		float DrawBoxThickness = 4;



	// Auto Destroy Block?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Constructor Weapon")
		bool bAutoDestroyBlocks = false;

	// Auto Destroy Time
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Constructor Weapon")
		float BlockRestoreTime = 3;

	// restore Ammo after block Destroyed?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Constructor Weapon")
		bool bRestoreAmmoAfterBlockDestroy = false;

	// Restore Ammo with time
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Constructor Weapon")
		bool bRestoreAmmo = false;

	// Restore Amoo Speed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Constructor Weapon")
		float AmmoRestoreTime = 0.4f;

	// Restore Ammo value
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Constructor Weapon")
		float AmmoRestoreValue = 1;


protected:

	// The Fire Event
	virtual void Fire()override;

	// The Alt Fire Event
	virtual void AltFire()override;

	// Ammo restore Event
	void AmmoRestore();

	// Ammo Restore Timerhandle
	FTimerHandle AmmoRestoreHandle;

	// Draw Box Event
	virtual void DrawBox();

public:

	// Enable Draw box on client 
	UFUNCTION(Client, Reliable)
	void Client_EnableDrawBox();
	virtual void Client_EnableDrawBox_Implementation();

	// Disable Draw box on client
	UFUNCTION(Client, Reliable)
	void Client_DisableDrawBox();
	virtual void Client_DisableDrawBox_Implementation();


	// Eqquip End
	virtual void EquipEnd()override;

	// Equip Start
	virtual void UnEquipStart()override;



};
