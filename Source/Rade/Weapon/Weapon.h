// Copyright 2015 Vagen Ayrapetyan

#pragma once

#include "Item/Item.h"
#include "Weapon.generated.h"


// You can hide some categories from the blueprint view if you want
//UCLASS(config = Game, BlueprintType, hidecategories = ("Animation", "Physics", "Clothing", "Collision", "Rendering", "Lighting", "Base", "Replication", "Input", "Actor"))

UCLASS(config = Game, BlueprintType, hidecategories = ("Clothing", "Input"))
class RADE_API AWeapon : public AItem
{
	GENERATED_BODY()

public:

	//				 Basic Events

	AWeapon(const class FObjectInitializer& PCIP);

	virtual void BeginPlay()override;


	//				 Weapon Details


	// First Person Mesh
	UPROPERTY(Replicated, VisibleDefaultsOnly, Category = Mesh)
		USkeletalMeshComponent* Mesh1P;

	//  Third Person Mesh
	UPROPERTY(Replicated,VisibleDefaultsOnly, Category = Mesh)
		USkeletalMeshComponent* Mesh3P;

	// Weapon Animation Archtype
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
		EAnimArchetype AnimArchetype = EAnimArchetype::Pistol;


	//      Fire Props

	// Use Ammo on Fire
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
		bool bUseAmmo = true;

	// Main Fire Data
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category = "Weapon")
		FFireStats MainFire;

	// Weapon has Alternative Fire?
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
		bool bAltFireEnabled = false;

	// Weapon Alternative Fire Has Separate ammo
	// Example 1 :  Single Fire , Triple Fire
	// Example 2 :  Main Fire, Grenade Fire
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
		bool bAltFireSeparateAmmo = false;

	// Alternative Fire Data
	UPROPERTY(Replicated,EditAnywhere, BlueprintReadOnly, Category = "Weapon")
		FFireStats AlternativeFire;

	// Weapon Equip Time
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
		float EquipTime = 0.25;

	// Weapon Reload Time
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
		float ReloadTime = 0.6;


	// Is Weapon Shooting
	bool bShooting = false;

	// Is Reloading
	bool bReloading = false;

	// Is Weapon Equiped
	bool bEquiped = false;

private:

	// Main Fire Timer Handle
	FTimerHandle PreFireTimeHandle;

	// Alternative Fire Timer Handle
	FTimerHandle PreAltFireTimeHandle;


public:

	// The Actual Fire Event
	virtual void Fire(){ BP_Fire(); }

	// Check Everything before the actual fire Example: check ammo, check player state, use ammo , etc
	virtual void PreFire();

	// Player Pressed Fire
	virtual void FireStart();

	// Player Released Fire
	virtual void FireEnd();

	// The Actual Alternative Fire Event
	virtual void AltFire(){ BP_AltFire(); }

	// Pre Alt Fire Checking
	virtual void PreAltFire();
	
	// Alt Fire Pressed
	virtual void AltFireStart();

	// Alt Fire Released
	virtual void AltFireEnd();


	// Save the current/equiped weapon stats to inventory
	void SaveCurrentWeaponStats();


	// Equip Started
	virtual void EquipStart();

	// Equip Ended
	virtual void EquipEnd();

	// Unequip Started
	virtual void UnEquipStart();

	// Unequip ended
	virtual void UnEquipEnd();

	// Reload Started
	virtual void ReloadWeaponStart();

	// Reload Ended
	virtual void ReloadWeaponEnd();


	// Can player Reload
	virtual bool CanReload();

	// Can Player Fire
	virtual bool CanShoot();

	// Use Main Fire Ammo
	void UseMainFireAmmo();

	// Use AltFire Ammo
	void UseAltFireAmmo();

	// Add Ammo by numbers 
	virtual void AddAmmo(float newAmmo, int32 newClip=0);

	// Add Ammo from weapon
	virtual void AddAmmo(AWeapon* weapAmmo);




	//				Blueprint Events 


	// Hit Enemy Character
	UFUNCTION(BlueprintImplementableEvent, Category = "Fire")
		void BP_HitEnemy(FHitResult HitData);

	// Blueprint Event:  Pre Fire
	UFUNCTION(BlueprintImplementableEvent, Category = "Fire")
		void BP_PreFire();

	// Blueprint Event:  Fire
	UFUNCTION(BlueprintImplementableEvent, Category = "Fire")
		void BP_Fire();

	// Blueprint Event:  Fire Pressed
	UFUNCTION(BlueprintImplementableEvent, Category = "Fire")
		void BP_FirePress_Start();

	// Blueprint Event:  Fire  Released
	UFUNCTION(BlueprintImplementableEvent, Category = "Fire")
		void BP_FirePress_End();

	// Blueprint Event:  Pre Alt Fire
	UFUNCTION(BlueprintImplementableEvent, Category = "Fire")
		void BP_PreAltFire();

	// Blueprint Event:  Alternative Fire
	UFUNCTION(BlueprintImplementableEvent, Category = "Fire")
		void BP_AltFire();

	// Blueprint Event:  AltFire  Pressed
	UFUNCTION(BlueprintImplementableEvent, Category = "Fire")
		void BP_AltFirePress_Start();

	// Blueprint Event:  AltFire  Released
	UFUNCTION(BlueprintImplementableEvent, Category = "Fire")
		void BP_AltFirePress_End();

	// Blueprint Event:  Reload Start
	UFUNCTION(BlueprintImplementableEvent, Category = "Fire")
		void BP_Reload_Start();

	// Blueprint Event:  Reload Finished
	UFUNCTION(BlueprintImplementableEvent, Category = "Fire")
		void BP_Reload_End();

	// Blueprint Event:  Tried firing , but no ammo
	UFUNCTION(BlueprintImplementableEvent, Category = "Fire")
		void BP_NoAmmo();

	// Blueprint Event:   Equip Start
	UFUNCTION(BlueprintImplementableEvent, Category = "Fire")
		void BP_Equip_Start();

	// Blueprint Event:   Equip End
	UFUNCTION(BlueprintImplementableEvent, Category = "Fire")
		void BP_Equip_End();

	// Blueprint Event:   UnEquip Start
	UFUNCTION(BlueprintImplementableEvent, Category = "Fire")
		void BP_Unequip_Start();

	// Blueprint Event:   unEquip End
	UFUNCTION(BlueprintImplementableEvent, Category = "Fire")
		void BP_Unequip_End();

	// Blueprint Event: Main Fire Ammo Used  
	UFUNCTION(BlueprintImplementableEvent, Category = "Fire")
		void BP_Ammo_Used();

	// Blueprint Event: Alt Main Fire Ammo Used 
	UFUNCTION(BlueprintImplementableEvent, Category = "Fire")
		void BP_AltAmmo_Used();

};
