// Copyright 2015 Vagen Ayrapetyan

#include "Rade.h"

#include "Character/RadePlayer.h"
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
	Mesh1P->CastShadow = false;
	Mesh1P->AttachParent = RootComponent;
	Mesh1P->bOnlyOwnerSee = true;
	Mesh1P->bVisible = true;

	Mesh3P = PCIP.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("WeaponMesh3P"));
	Mesh3P->CastShadow = true;
	Mesh3P->AttachParent = RootComponent;
	Mesh3P->bOwnerNoSee = true;
	Mesh3P->bVisible = true;


	bReplicates = true;

	Mesh1P->SetIsReplicated(true);
	Mesh3P->SetIsReplicated(true);	

	PrimaryActorTick.bCanEverTick = false;
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	// Double check at the begining
	bReloading = false;
	bShooting = false;
}

// Player Pressed Fire
void AWeapon::FireStart()
{
	// BP Fire Started
	BP_FirePress_Start();

	// Shooting Enabled
	bShooting = true;

	// If not currently in main and alternative shooting mode/delay  call The Fire event
	if (ThePlayer && !ThePlayer->GetWorldTimerManager().IsTimerActive(PreFireTimeHandle) &&
		!ThePlayer->GetWorldTimerManager().IsTimerActive(PreAltFireTimeHandle))
		ThePlayer->GetWorldTimerManager().SetTimer(PreFireTimeHandle, this, &AWeapon::PreFire, MainFire.FireSpeed, true, 0);

	

}

// Player Released Fire
void AWeapon::FireEnd()
{
	// BP Fire Released
	BP_FirePress_End();

	// Shooting Disabled
	bShooting = false;
}

// Pre Fire
void AWeapon::PreFire()
{
	// BP Pre Fire
	BP_PreFire();

	if (!ThePlayer)return;

	// If player stopped shooting stop event
	if (!bShooting )
	{
		ThePlayer->GetWorldTimerManager().ClearTimer(PreFireTimeHandle);
		return;
	}

	// Currently Equiping this weapon, delay a bit
	if (ThePlayer->IsAnimState(EAnimState::Equip))
	{
		// try after a small delay
		FTimerHandle MyHandle;
		ThePlayer->GetWorldTimerManager().SetTimer(MyHandle, this, &AWeapon::PreFire, 0.2, false);
		return;
	}

	// Wrong Weapon or Player Anim State
	if (!CanShoot())return;

	// Ammo Check
	if (!MainFire.CanFire())
	{
		//  BP No Ammo
		BP_NoAmmo();

		// if have ammo, start reloading
		if (CanReload())ReloadWeaponStart();
		return;
	}

	// The real fire event
	Fire();

	// Use Ammo
	if (bUseAmmo)UseMainFireAmmo();


	// Set Player Anim State
	ThePlayer->ServerSetAnimID(EAnimState::Fire);

	// Decrease move speed when shooting
	ThePlayer->ResetMoveSpeed();

	///	Stop Fire Anim
	FTimerHandle MyHandle;
	ThePlayer->GetWorldTimerManager().SetTimer(MyHandle, this, &AWeapon::StopFireAnim, 0.1, false);
}


// Alt Fire Pressed
void AWeapon::AltFireStart()
{
	// IF alt fire enabled
	if (!bAltFireEnabled)return;

	// If not currently in main and alternative shooting mode/delay , call the Pre Fire event
	BP_AltFirePress_Start();
	if (!ThePlayer->GetWorldTimerManager().IsTimerActive(PreFireTimeHandle) && !ThePlayer->GetWorldTimerManager().IsTimerActive(PreAltFireTimeHandle))
		ThePlayer->GetWorldTimerManager().SetTimer(PreAltFireTimeHandle, this, &AWeapon::PreAltFire, AlternativeFire.FireSpeed, true, 0);

	// Start shooting
	bShooting = true;

}

// Alt Fire Released
void AWeapon::AltFireEnd()
{
	// Stop Shooting
	bShooting = false;

	// BP Stop Alt Fire
	BP_AltFirePress_End();

	// Clear Fire Timer
	if (ThePlayer->GetWorldTimerManager().IsTimerActive(PreAltFireTimeHandle))
		ThePlayer->GetWorldTimerManager().ClearTimer(PreAltFireTimeHandle);

}

// Pre Alt Fire
void AWeapon::PreAltFire()
{
	if (!ThePlayer)return;

	// Bp Pre Alternative Fire
	BP_PreAltFire();

	// If not shooting , stop everything
	if (!bShooting)
	{
		ThePlayer->GetWorldTimerManager().ClearTimer(PreAltFireTimeHandle);
		return;
	}


	// Currently Equiping this weapon
	if (ThePlayer->ArmsAnimInstance && ThePlayer->ArmsAnimInstance->IsAnimState(EAnimState::Equip))
	{
		// Try after a small delay
		FTimerHandle MyHandle;
		ThePlayer->GetWorldTimerManager().SetTimer(MyHandle, this, &AWeapon::PreAltFire, 0.2, false);
		return;
	}

	// Wrong Player Anim State
	if (!CanShoot())return;

	// Seprate Ammo check
	if (bAltFireSeparateAmmo)
	{
		
		if (!AlternativeFire.CanFire())
		{
			// BP no ammo for Alt fire
			BP_NoAmmo();
			return;
		}
	}
	// Unified ammo Check
	else if (AlternativeFire.FireCost > 0 && AlternativeFire.FireCost > MainFire.CurrentAmmo)
	{
			BP_NoAmmo();
			return;
	}


	// The real Alt fire event
	AltFire();

	// Use Alt Fire Ammo
	if (bUseAmmo)UseAltFireAmmo();


	// Set Player Anim State
	ThePlayer->ServerSetAnimID(EAnimState::Fire);

	// Decrease move speed when shooting
	ThePlayer->ResetMoveSpeed();


	///	Stop Fire Anim
	FTimerHandle MyHandle;
	ThePlayer->GetWorldTimerManager().SetTimer(MyHandle, this, &AWeapon::StopFireAnim, 0.1, false);
}



// Equip Start
void AWeapon::EquipStart()
{
	if (!ThePlayer)return;

	// Bp Equip Start
	BP_Equip_Start();


	// Attach First person Mesh
	if (Mesh1P)	{
		AttachRootComponentTo(ThePlayer->Mesh1P, FName("WeaponSocket"));
		Mesh1P->RelativeLocation = (Mesh1P->RelativeLocation);
		Mesh1P->RelativeRotation = (Mesh1P->RelativeRotation);
	}

	// Attach Third Person Mesh
	if (Mesh3P)	{
		
		Mesh3P->AttachTo(ThePlayer->GetMesh(), FName("WeaponSocket"));
		Mesh3P->RelativeLocation = (Mesh3P->RelativeLocation);
		Mesh3P->RelativeRotation = (Mesh3P->RelativeRotation);
		Mesh3P->SetOwnerNoSee(true);
		Mesh3P->bOwnerNoSee = true;
	}

	// Load Weapon Data from inventory
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

	// Delay equip end
	FTimerHandle myHandle;
	ThePlayer->GetWorldTimerManager().SetTimer(myHandle, this, &AWeapon::EquipEnd, EquipTime, false);

}



// Stop Fire Anim after some time
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

// Equip Ended
void AWeapon::EquipEnd()
{
	// BP equip Ended
	BP_Equip_End();

	// Save Inventory
	if (ThePlayer && ThePlayer->TheInventory) 
		ThePlayer->TheInventory->SaveInventory();
}

// Unequip Start
void AWeapon::UnEquipStart()
{
	// Save the weapon stats
	SaveCurrentWeaponStats();

	// BP UnEquip Start
	BP_Unequip_Start();

	//delay unequip End
	FTimerHandle myHandle;
	ThePlayer->GetWorldTimerManager().SetTimer(myHandle, this, &AWeapon::UnEquipEnd, EquipTime, false);
}

// Unequip End
void AWeapon::UnEquipEnd()
{
	// BP unequip End
	BP_Unequip_End();

	// Save Inventory
	if (ThePlayer && ThePlayer->TheInventory) ThePlayer->TheInventory->SaveInventory();

	// Destroy the Weapon Actor
	Destroy();
}

// Reload Start
void AWeapon::ReloadWeaponStart()
{
	if (!ThePlayer)return;

	// BP reload Start
	BP_Reload_Start();

	// Set reload State in weapon adn player
	bReloading = true;
	ThePlayer->ServerSetAnimID(EAnimState::Reload);

	// Delay reload End
	FTimerHandle myHandle;
	ThePlayer->GetWorldTimerManager().SetTimer(myHandle, this, &AWeapon::ReloadWeaponEnd, ReloadTime, false);
}

// reload Ended
void AWeapon::ReloadWeaponEnd()
{
	// Bp Reload Ended
	BP_Reload_End();

	// Substract clip number
	MainFire.ClipNumber--;

	// Set Current Ammo to clip size
	MainFire.CurrentAmmo = MainFire.ClipSize;

	// Stop Fire State in weapon and Player
	bReloading = false;
	ThePlayer->ServerSetAnimID(EAnimState::Idle_Run);
}

// Can Weapon Reload ?
bool AWeapon::CanReload()
{
	if (bReloading || !ThePlayer || !MainFire.CanReload())
	{
		return false;
	}
	return true;
}

// Can Weapon and Player Fire
bool AWeapon::CanShoot()
{
	if (!ThePlayer || !ThePlayer->CanShoot())return false;

	return true;
}


// Save Weapon Data
void AWeapon::SaveCurrentWeaponStats()
{
	if (!ThePlayer || !ThePlayer->TheInventory)return;

	// Find Weapon data in item list
	for (int32 i = 0; i < ThePlayer->TheInventory->Items.Num(); i++)
	{
		
		if (ThePlayer->TheInventory->Items.IsValidIndex(i) && ThePlayer->TheInventory->Items[i].Archetype->GetDefaultObject()->GetClass() == GetClass())
		{
			// Set The Inventory item data to this weapon data
			ThePlayer->TheInventory->Items[i].MainFireStats = MainFire;
			ThePlayer->TheInventory->Items[i].AltFireStats = AlternativeFire;
			return;
		}
	}
}

// Use Main Fire Ammo
void AWeapon::UseMainFireAmmo()
{
	// BP Ammo Used
	BP_Ammo_Used();

	// Substract Fire Cost
	MainFire.CurrentAmmo -= MainFire.FireCost;
}

// use Alt Fire Ammo
void  AWeapon::UseAltFireAmmo()
{
	// BP Alt Ammo Used
	BP_AltAmmo_Used();

	// Check which ammo to substract from
	if (bAltFireSeparateAmmo)AlternativeFire.CurrentAmmo -= AlternativeFire.FireCost;
	else MainFire.CurrentAmmo -= AlternativeFire.FireCost;
}

// Add Ammo from weapon pointer
void AWeapon::AddAmmo(AWeapon* weapAmmo)
{
	if (weapAmmo)
	{
		MainFire.AddAmmo(weapAmmo->MainFire.CurrentAmmo, weapAmmo->MainFire.ClipNumber);
	}
}

// Add Ammo as a number
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