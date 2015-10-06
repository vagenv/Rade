// Copyright 2015 Vagen Ayrapetyan

#include "Rade.h"

#include "Character/RadeCharacter.h"
#include "Character/RadeAnimInstance.h"

#include "Item/Inventory.h"
#include "Weapon/Weapon.h"
#include "UnrealNetwork.h"

#include "RadePC.h"



AWeapon::AWeapon(const class FObjectInitializer& PCIP)
	: Super(PCIP)
{
	RootComponent = PCIP.CreateDefaultSubobject<USceneComponent>(this, TEXT("Root Component"));

	Mesh1P = PCIP.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("WeaponMesh1P"));
	//Mesh1P->bReceivesDecals = false;
	Mesh1P->CastShadow = false;
	Mesh1P->AttachParent = RootComponent;
	Mesh1P->bOnlyOwnerSee = true;
	Mesh1P->bVisible = true;

	Mesh3P = PCIP.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("WeaponMesh3P"));
	//Mesh3P->bReceivesDecals = false;
	Mesh3P->CastShadow = true;
	Mesh3P->AttachParent = RootComponent;
	Mesh3P->bOwnerNoSee = true;
	Mesh3P->bVisible = true;
	PrimaryActorTick.bCanEverTick = false;
	
	bReplicates = true;
	Mesh1P->SetIsReplicated(true);
	Mesh3P->SetIsReplicated(true);		
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	bReloading = false;
	bShooting = false;
}

/////////////////////////////////   Main Fire Part
void AWeapon::FireStart()
{

	BP_FirePress_Start();

	// If not currently in main and alternative shooting mode/delay 
	if (ThePlayer && !ThePlayer->GetWorldTimerManager().IsTimerActive(PreFireTimeHandle) &&
		!ThePlayer->GetWorldTimerManager().IsTimerActive(PreAltFireTimeHandle))
		ThePlayer->GetWorldTimerManager().SetTimer(PreFireTimeHandle, this, &AWeapon::PreFire, MainFire.FireSpeed, true, 0);

	bShooting = true;

}

void AWeapon::FireEnd()
{
	BP_FirePress_End();
	bShooting = false;
}



void AWeapon::PreFire()
{
	BP_PreFire();

	if (!ThePlayer)return;

	if (!bShooting )
	{
		ThePlayer->GetWorldTimerManager().ClearTimer(PreFireTimeHandle);
		return;
	}

	// Currently Equiping the weapon
	if (ThePlayer->ArmsAnimInstance && ThePlayer->ArmsAnimInstance->IsAnimState(EAnimState::Equip))
	{
		FTimerHandle MyHandle;
		ThePlayer->GetWorldTimerManager().SetTimer(MyHandle, this, &AWeapon::PreFire, 0.2, false);
		return;
	}

	// Wrong Player Anim State
	if (!CanShoot())return;

	// Ammo Check
	if (!MainFire.CanFire())
	{
		BP_NoAmmo();
		if (CanReload())ReloadWeaponStart();
		return;
	}

	// Whole fire event
	Fire();

	if (bUseAmmo)UseMainFireAmmo();


	// Play Anim
	ThePlayer->ServerSetAnimID(EAnimState::Fire);

	// Decrease move speed when shooting
	ThePlayer->ResetMoveSpeed();

	///	??
	FTimerHandle MyHandle;
	ThePlayer->GetWorldTimerManager().SetTimer(MyHandle, this, &AWeapon::StopFireAnim, 0.08, false);
}



/////////////////////////////////////////////  Alt Fire Part

void AWeapon::AltFireStart()
{
	// IF alt fire enabled
	if (!bAltFireEnabled)return;

	bShooting = true;

	// If not currently in main and alternative shooting mode/delay 
	BP_AltFirePress_Start();
	if (!ThePlayer->GetWorldTimerManager().IsTimerActive(PreFireTimeHandle) && !ThePlayer->GetWorldTimerManager().IsTimerActive(PreAltFireTimeHandle))
		ThePlayer->GetWorldTimerManager().SetTimer(PreAltFireTimeHandle, this, &AWeapon::PreAltFire, AlternativeFire.FireSpeed, true, 0);

}

void AWeapon::AltFireEnd()
{
	bShooting = false;

	BP_AltFirePress_End();

	if (ThePlayer->GetWorldTimerManager().IsTimerActive(PreAltFireTimeHandle))
		ThePlayer->GetWorldTimerManager().ClearTimer(PreAltFireTimeHandle);

}

void AWeapon::PreAltFire()
{
	if (!ThePlayer)return;

	BP_PreAltFire();

	// If not shooting , stop everything
	if (!bShooting)
	{
		ThePlayer->GetWorldTimerManager().ClearTimer(PreAltFireTimeHandle);
		return;
	}


	// Currently Equiping the weapon
	if (ThePlayer->ArmsAnimInstance && ThePlayer->ArmsAnimInstance->IsAnimState(EAnimState::Equip))
	{
		FTimerHandle MyHandle;
		ThePlayer->GetWorldTimerManager().SetTimer(MyHandle, this, &AWeapon::PreAltFire, 0.2, false);
		return;
	}

	// Wrong Player Anim State
	if (!CanShoot())return;

	// Ammo Check
	if (bAltFireSeparateAmmo)
	{
		// Seprate Ammo check
		if (!AlternativeFire.CanFire())
		{
			BP_NoAmmo();
			return;
		}
	}
	else 
	{
		// Unified ammo Check
		if (AlternativeFire.FireCost > 0 && AlternativeFire.FireCost > MainFire.CurrentAmmo)
		{
			BP_NoAmmo();
			return;
		}
	}

	

	// Whole fire event
	AltFire();

	UseAltFireAmmo();



	// Play Anim
	ThePlayer->ServerSetAnimID(EAnimState::Fire);
	ThePlayer->ResetMoveSpeed();


	FTimerHandle MyHandle;
	ThePlayer->GetWorldTimerManager().SetTimer(MyHandle, this, &AWeapon::StopFireAnim, 0.08, false);

}

// For What?????

void AWeapon::StopFireAnim()
{
	if (!ThePlayer)return;

	//IsMovingOnGround
	if (ThePlayer->PlayerMovementComponent && ThePlayer->PlayerMovementComponent->IsMovingOnGround())ThePlayer->ServerSetAnimID(EAnimState::Idle_Run);
	else
	{
		//print("End shoot in air");
		ThePlayer->ServerSetAnimID(EAnimState::Jumploop);
	}

}



// Use Item

void AWeapon::InventoryUse(ARadeCharacter* player)
{
	Super::InventoryUse(player);

	if (!ThePlayer) return;
}



void AWeapon::EquipStart()
{
	if (!ThePlayer)return;

	BP_Equip_Start();

	//->TheWeapon = this;

	if (Mesh1P)	{
		AttachRootComponentTo(ThePlayer->Mesh1P, FName("WeaponSocket"));
		Mesh1P->RelativeLocation = (Mesh1P->RelativeLocation);
		Mesh1P->RelativeRotation = (Mesh1P->RelativeRotation);
	}
	if (Mesh3P)	{
		
		Mesh3P->AttachTo(ThePlayer->GetMesh(), FName("WeaponSocket"));
		Mesh3P->RelativeLocation = (Mesh3P->RelativeLocation);
		Mesh3P->RelativeRotation = (Mesh3P->RelativeRotation);
		Mesh3P->SetOwnerNoSee(true);
		Mesh3P->bOwnerNoSee = true;
	}

	// Load Weapon Data
	if (ThePlayer && ThePlayer->TheInventory)
	{
		for (int32 i = 0; i < ThePlayer->TheInventory->Items.Num(); i++)
		{

			if (ThePlayer->TheInventory->Items.IsValidIndex(i) &&
				ThePlayer->TheInventory->Items[i].Archetype && 
				ThePlayer->TheInventory->Items[i].Archetype->GetDefaultObject()	&&
				ThePlayer->TheInventory->Items[i].Archetype->GetDefaultObject()->GetClass() == GetClass())
			{
				
				MainFire = ThePlayer->TheInventory->Items[i].MainFireStats;
				AlternativeFire = ThePlayer->TheInventory->Items[i].AltFireStats;
				break;			
			}

		}
	}
	FTimerHandle myHandle;
	ThePlayer->GetWorldTimerManager().SetTimer(myHandle, this, &AWeapon::EquipEnd, EquipTime, false);

}


void AWeapon::EquipEnd()
{
	BP_Equip_End();

	if (ThePlayer && ThePlayer->TheInventory) ThePlayer->TheInventory->SaveInventory();
}


void AWeapon::UnEquipStart()
{
	SaveCurrentWeaponStats();
	BP_Unequip_Start();
	FTimerHandle myHandle;
	ThePlayer->GetWorldTimerManager().SetTimer(myHandle, this, &AWeapon::UnEquipEnd, EquipTime, false);

}
void AWeapon::UnEquipEnd()
{
	BP_Unequip_End();

	if (ThePlayer && ThePlayer->TheInventory) ThePlayer->TheInventory->SaveInventory();

	//printg("Unequip End");
	//SaveCurrentWeaponStats();
	/*
	if (ThePlayer && ThePlayer->TheInventory)
	{
		for (int32 i = 0; i < ThePlayer->TheInventory->Items.Num(); i++)
		{

			if (ThePlayer->TheInventory->Items.IsValidIndex(i) &&
				ThePlayer->TheInventory->Items[i].Archetype &&
				ThePlayer->TheInventory->Items[i].Archetype->GetDefaultObject() &&
				ThePlayer->TheInventory->Items[i].Archetype->GetDefaultObject()->GetClass() == GetClass())
			{
				printb("Weapon Data saved");
				ThePlayer->TheInventory->Items[i].WeaponStats = MainFire;
				break;
			}
			//else printb("wrong item");

		}
	}
	*/
	Destroy();

}

void AWeapon::ReloadWeaponStart()
{
	if (!ThePlayer)return;
	BP_Reload_Start();

	bReloading = true;
	ThePlayer->ServerSetAnimID(EAnimState::Reload);
	FTimerHandle myHandle;
	ThePlayer->GetWorldTimerManager().SetTimer(myHandle, this, &AWeapon::ReloadWeaponEnd, ReloadTime, false);
}

void AWeapon::ReloadWeaponEnd()
{
	BP_Reload_End();

	MainFire.ClipNumber--;
	MainFire.CurrentAmmo = MainFire.ClipSize;
	bReloading = false;

	ThePlayer->ServerSetAnimID(EAnimState::Idle_Run);

}


bool AWeapon::CanReload()
{
	if (bReloading || !ThePlayer || !MainFire.CanReload())
	{
		return false;
	}
	return true;
}

bool AWeapon::CanShoot()
{

	if (!ThePlayer || !ThePlayer->ArmsAnimInstance || !ThePlayer->CanShoot())return false;

	return true;
}


// Save Weapon Data
void AWeapon::SaveCurrentWeaponStats()
{
	if (!ThePlayer || !ThePlayer->TheInventory)return;

	for (int32 i = 0; i < ThePlayer->TheInventory->Items.Num(); i++)
	{
		
		if (ThePlayer->TheInventory->Items[i].Archetype->GetDefaultObject()->GetClass() == GetClass())
		{
		//	printg("Weapon Data Saved");
			ThePlayer->TheInventory->Items[i].MainFireStats = MainFire;
			ThePlayer->TheInventory->Items[i].AltFireStats = AlternativeFire;
			return;
			
		}
	}
	//printr("Weapon Not Found");
}

void AWeapon::UseMainFireAmmo(){

	BP_Ammo_Used();
	MainFire.CurrentAmmo -= MainFire.FireCost;
	//printr("Ammo Left" + FString::FromInt(MainFire.CurrentAmmo));
}
void  AWeapon::UseAltFireAmmo(){
	BP_AltAmmo_Used();
	if (bAltFireSeparateAmmo)AlternativeFire.CurrentAmmo -= AlternativeFire.FireCost;
	else MainFire.CurrentAmmo -= AlternativeFire.FireCost;
}

// Add Ammo
void AWeapon::AddAmmo(AWeapon* weapAmmo)
{
	if (weapAmmo)
	{
		MainFire.AddAmmo(weapAmmo->MainFire.CurrentAmmo, weapAmmo->MainFire.ClipNumber);
	}
}
void AWeapon::AddAmmo(float newAmmo, int32 newClip)
{
	MainFire.AddAmmo(newAmmo,newClip);
}

void AWeapon::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AWeapon, MainFire);
	DOREPLIFETIME(AWeapon, AlternativeFire);
	DOREPLIFETIME(AWeapon, Mesh1P);
	DOREPLIFETIME(AWeapon, Mesh3P);


}