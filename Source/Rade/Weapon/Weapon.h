// Copyright 2015 Vagen Ayrapetyan

#pragma once

#include "Item/Item.h"
#include "Weapon.generated.h"



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


	UPROPERTY(Replicated, VisibleDefaultsOnly, Category = Mesh)
		USkeletalMeshComponent* Mesh1P;

	UPROPERTY(Replicated,VisibleDefaultsOnly, Category = Mesh)
		USkeletalMeshComponent* Mesh3P;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
		EAnimArchetype AnimArchetype = EAnimArchetype::Pistol;


	//      Fire Props
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
		bool bUseAmmo = true;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category = "Weapon")
		FFireStats MainFire;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
		bool bAltFireEnabled = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
		bool bAltFireSeparateAmmo = false;
	UPROPERTY(Replicated,EditAnywhere, BlueprintReadOnly, Category = "Weapon")
		FFireStats AlternativeFire;




	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
		float EquipTime = 0.25;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
		float ReloadTime = 0.6;



	bool bShooting = false;
	bool bReloading = false;
	bool bEquiped = false;
private:
	FTimerHandle PreFireTimeHandle;
	FTimerHandle PreAltFireTimeHandle;

	//			 Weapon Internal Events
public:

	virtual void InventoryUse(class ARadeCharacter* player)override;


	virtual void Fire(){ BP_Fire(); }
	virtual void PreFire();


	virtual void FireStart();
	virtual void FireEnd();


	virtual void AltFire(){ BP_AltFire(); }
	virtual void PreAltFire();
	

	virtual void AltFireStart();
	virtual void AltFireEnd();


	void SaveCurrentWeaponStats();

	void StopFireAnim();

	// Equip and unequip
	virtual void EquipStart();
	virtual void EquipEnd();
	virtual void UnEquipStart();
	virtual void UnEquipEnd();


	virtual void ReloadWeaponStart();
	virtual void ReloadWeaponEnd();


	virtual bool CanReload();
	virtual bool CanShoot();

	void UseMainFireAmmo();
	void UseAltFireAmmo();

	virtual void AddAmmo(float newAmmo, int32 newClip=0);
	virtual void AddAmmo(AWeapon* weapAmmo);




	////////////////////////////////////////////////////////////


	//				Blueprint Events 




	UFUNCTION(BlueprintImplementableEvent, Category = "Fire")
		void BP_HitEnemy(FHitResult HitData);

	UFUNCTION(BlueprintImplementableEvent, Category = "Fire")
		void BP_PreFire();
	UFUNCTION(BlueprintImplementableEvent, Category = "Fire")
		void BP_Fire();
	UFUNCTION(BlueprintImplementableEvent, Category = "Fire")
		void BP_FirePress_Start();
	UFUNCTION(BlueprintImplementableEvent, Category = "Fire")
		void BP_FirePress_End();

	UFUNCTION(BlueprintImplementableEvent, Category = "Fire")
		void BP_PreAltFire();
	UFUNCTION(BlueprintImplementableEvent, Category = "Fire")
		void BP_AltFire();
	UFUNCTION(BlueprintImplementableEvent, Category = "Fire")
		void BP_AltFirePress_Start();
	UFUNCTION(BlueprintImplementableEvent, Category = "Fire")
		void BP_AltFirePress_End();

	UFUNCTION(BlueprintImplementableEvent, Category = "Fire")
		void BP_Reload_Start();
	UFUNCTION(BlueprintImplementableEvent, Category = "Fire")
		void BP_Reload_End();

	UFUNCTION(BlueprintImplementableEvent, Category = "Fire")
		void BP_NoAmmo();

	
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Fire")
		void BP_Equip_Start();
	UFUNCTION(BlueprintImplementableEvent, Category = "Fire")
		void BP_Equip_End();

	UFUNCTION(BlueprintImplementableEvent, Category = "Fire")
		void BP_Unequip_Start();
	UFUNCTION(BlueprintImplementableEvent, Category = "Fire")
		void BP_Unequip_End();

	UFUNCTION(BlueprintImplementableEvent, Category = "Fire")
		void BP_Ammo_Used();
	UFUNCTION(BlueprintImplementableEvent, Category = "Fire")
		void BP_AltAmmo_Used();




};
