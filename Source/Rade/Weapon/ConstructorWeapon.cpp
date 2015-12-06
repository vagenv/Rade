// Copyright 2015 Vagen Ayrapetyan

#include "Rade.h"
#include "RadeGameMode.h"

#include "Weapon/ConstructorWeapon.h"

#include "Character/RadePlayer.h"

#include "Custom/LevelBlockConstructor.h"
#include "Custom/LevelBlock.h"

#include "Components/LineBatchComponent.h"
#include "DrawDebugHelpers.h"

AConstructorWeapon::AConstructorWeapon(const FObjectInitializer& PCIP)
	: Super(PCIP)
{
	bAltFireEnabled = true;
	bUseAmmo = false;
}

void AConstructorWeapon::BeginPlay()
{
	Super::BeginPlay();

	// Find Level Block Constructor
	if (GetWorld() && GetWorld()->GetAuthGameMode() && GetWorld()->GetAuthGameMode<ARadeGameMode>())
	{
		TheLevelBlockConstructor = GetWorld()->GetAuthGameMode<ARadeGameMode>()->TheLevelBlockConstructor;
	}
}

// Get Linebatcher pointer
static ULineBatchComponent* GetDebugLineBatcher(const UWorld* InWorld, bool bPersistentLines, float LifeTime, bool bDepthIsForeground)
{
	return (InWorld ? (bDepthIsForeground ? InWorld->ForegroundLineBatcher : ((bPersistentLines || (LifeTime > 0.f)) ? InWorld->PersistentLineBatcher : InWorld->LineBatcher)) : NULL);
}


// Fire Event
void AConstructorWeapon::Fire()
{
	Super::Fire();

	// If level block constructor pointer empty ,try to find it again
	if (TheLevelBlockConstructor == NULL)
	{
		if (GetWorld() && GetWorld()->GetAuthGameMode() && GetWorld()->GetAuthGameMode<ARadeGameMode>())
		{
			TheLevelBlockConstructor = GetWorld()->GetAuthGameMode<ARadeGameMode>()->TheLevelBlockConstructor;
		}
	}

	if (TheLevelBlockConstructor && Mesh1P && ThePlayer && ThePlayer->Controller)
	{
		// Get Camera Location and Rotation
		FVector CamLoc;
		FRotator CamRot;
		ThePlayer->Controller->GetPlayerViewPoint(CamLoc, CamRot); 


		// Calculate the the end point where player is looking at
		FVector StartTrace = Mesh1P->GetSocketLocation(TEXT("FireSocket"));
		FVector Direction = CamRot.Vector();
		FVector EndTrace = StartTrace + Direction *MainFire.FireDistance;

		// Add New Block To Constructor
		if (TheLevelBlockConstructor->AddNewBlock(BlockArchetype, EndTrace, this))
			UseMainFireAmmo();
	}
}

// Alt Fire
void AConstructorWeapon::AltFire()
{
	Super::AltFire();

	// If level block constructor pointer empty ,try to find it again
	if (GetWorld() && GetWorld()->GetAuthGameMode() && GetWorld()->GetAuthGameMode<ARadeGameMode>())
	{
		TheLevelBlockConstructor = GetWorld()->GetAuthGameMode<ARadeGameMode>()->TheLevelBlockConstructor;
	}

	if (TheLevelBlockConstructor && Mesh1P && ThePlayer->Controller)
	{
		// Get Camera location and rotation
		FVector CamLoc;
		FRotator CamRot;
		ThePlayer->Controller->GetPlayerViewPoint(CamLoc, CamRot);

		// Calculate the the end point where player is looking at
		FVector StartTrace = Mesh1P->GetSocketLocation(TEXT("FireSocket"));
		FVector Direction = CamRot.Vector();
		FVector EndTrace = StartTrace + Direction *MainFire.FireDistance;

		// Destroy block that is being looked at
		if (TheLevelBlockConstructor->DestroyBlock(EndTrace, this)) 
			MainFire.AddAmmo(MainFire.FireCost,0);
	}
}

// Draw Box Update
void AConstructorWeapon::DrawBox()
{
	// Get pointer to player
	if (!ThePlayer && GetOwner()  && Cast<ARadePlayer>(GetOwner()))
	{
		ThePlayer = Cast<ARadePlayer>(GetOwner());
	}

	if (ThePlayer && ThePlayer->Controller && Mesh1P)
	{
		// Get Camera location and rotation
		FVector CamLoc;
		FRotator CamRot;
		ThePlayer->Controller->GetPlayerViewPoint(CamLoc, CamRot); 

		// Calculate the the end point where player is looking at
		FVector StartTrace = Mesh1P->GetSocketLocation(TEXT("FireSocket"));
		FVector Direction = CamRot.Vector();
		FVector EndTrace = StartTrace + Direction *MainFire.FireDistance;


		// Flooring
		EndTrace.X = 100 * round(EndTrace.X / 100);
		EndTrace.Y = 100 * round(EndTrace.Y / 100);
		EndTrace.Z = 100 * round(EndTrace.Z / 100);


		// Draw lines from 2 points in 3 axis
		ULineBatchComponent* const LineBatcher = GetDebugLineBatcher(GetWorld(), false, BoxDrawUpdate,false);
		if (LineBatcher != NULL)
		{
			float LineLifeTime = (BoxDrawUpdate > 0.f) ? BoxDrawUpdate : LineBatcher->DefaultLifeTime;
			uint8 DepthPriority = 0;
	
			LineBatcher->DrawLine(EndTrace + FVector(BoxDrawSize, BoxDrawSize, BoxDrawSize), EndTrace + FVector(BoxDrawSize, -BoxDrawSize, BoxDrawSize), DrawBoxColor, DepthPriority, DrawBoxThickness, LineLifeTime);
			LineBatcher->DrawLine(EndTrace + FVector(BoxDrawSize, -BoxDrawSize, BoxDrawSize), EndTrace + FVector(-BoxDrawSize, -BoxDrawSize, BoxDrawSize), DrawBoxColor, DepthPriority, DrawBoxThickness, LineLifeTime);
			LineBatcher->DrawLine(EndTrace + FVector(-BoxDrawSize, -BoxDrawSize, BoxDrawSize), EndTrace + FVector(-BoxDrawSize, BoxDrawSize, BoxDrawSize), DrawBoxColor, DepthPriority, DrawBoxThickness, LineLifeTime);
			LineBatcher->DrawLine(EndTrace + FVector(-BoxDrawSize, BoxDrawSize, BoxDrawSize), EndTrace + FVector(BoxDrawSize, BoxDrawSize, BoxDrawSize), DrawBoxColor, DepthPriority, DrawBoxThickness, LineLifeTime);

			LineBatcher->DrawLine(EndTrace + FVector(BoxDrawSize, BoxDrawSize, -BoxDrawSize), EndTrace + FVector(BoxDrawSize, -BoxDrawSize, -BoxDrawSize), DrawBoxColor, DepthPriority, DrawBoxThickness, LineLifeTime);
			LineBatcher->DrawLine(EndTrace + FVector(BoxDrawSize, -BoxDrawSize, -BoxDrawSize), EndTrace + FVector(-BoxDrawSize, -BoxDrawSize, -BoxDrawSize), DrawBoxColor, DepthPriority, DrawBoxThickness, LineLifeTime);
			LineBatcher->DrawLine(EndTrace + FVector(-BoxDrawSize, -BoxDrawSize, -BoxDrawSize), EndTrace + FVector(-BoxDrawSize, BoxDrawSize, -BoxDrawSize), DrawBoxColor, DepthPriority, DrawBoxThickness, LineLifeTime);
			LineBatcher->DrawLine(EndTrace + FVector(-BoxDrawSize, BoxDrawSize, -BoxDrawSize), EndTrace + FVector(BoxDrawSize, BoxDrawSize, -BoxDrawSize), DrawBoxColor, DepthPriority, DrawBoxThickness, LineLifeTime);

			LineBatcher->DrawLine(EndTrace + FVector(BoxDrawSize, BoxDrawSize, BoxDrawSize), EndTrace + FVector(BoxDrawSize, BoxDrawSize, -BoxDrawSize), DrawBoxColor, DepthPriority, DrawBoxThickness, LineLifeTime);
			LineBatcher->DrawLine(EndTrace + FVector(BoxDrawSize, -BoxDrawSize, BoxDrawSize), EndTrace + FVector(BoxDrawSize, -BoxDrawSize, -BoxDrawSize), DrawBoxColor, DepthPriority, DrawBoxThickness, LineLifeTime);
			LineBatcher->DrawLine(EndTrace + FVector(-BoxDrawSize, -BoxDrawSize, BoxDrawSize), EndTrace + FVector(-BoxDrawSize, -BoxDrawSize, -BoxDrawSize), DrawBoxColor, DepthPriority, DrawBoxThickness, LineLifeTime);
			LineBatcher->DrawLine(EndTrace + FVector(-BoxDrawSize, BoxDrawSize, BoxDrawSize), EndTrace + FVector(-BoxDrawSize, BoxDrawSize, -BoxDrawSize), DrawBoxColor, DepthPriority, DrawBoxThickness, LineLifeTime);
		}

	}
}



// Ammo restore
void AConstructorWeapon::AmmoRestore(){
	MainFire.AddAmmo(AmmoRestoreValue, 0);
}

// Equip End
void AConstructorWeapon::EquipEnd()
{
	Super::EquipEnd();

	// Get Constructor reference
	if (!TheLevelBlockConstructor && GetWorld() && GetWorld()->GetAuthGameMode() && GetWorld()->GetAuthGameMode<ARadeGameMode>())
	{
		TheLevelBlockConstructor = GetWorld()->GetAuthGameMode<ARadeGameMode>()->TheLevelBlockConstructor;
	}

	// Start Drawing Box
	if (ThePlayer)
	{
		if (ThePlayer->ThePC)
		{
			// If you are server
			GetWorldTimerManager().SetTimer(DrawCubeHandle, this, &AConstructorWeapon::DrawBox, BoxDrawUpdate, true);
			if (bRestoreAmmo && AmmoRestoreTime>0)GetWorldTimerManager().SetTimer(AmmoRestoreHandle, this, &AConstructorWeapon::AmmoRestore, AmmoRestoreTime, true);
		}
		else 
		{
			// if you are client
			Client_EnableDrawBox();
		}
	}

}

// Unequip Start
void AConstructorWeapon::UnEquipStart()
{
	if (ThePlayer)
	{
		// End Drawing Box
		if (ThePlayer->ThePC)
		{
			// On Server
			GetWorldTimerManager().ClearTimer(DrawCubeHandle);
			GetWorldTimerManager().ClearTimer(AmmoRestoreHandle);
		}
		else 
		{
			// On Client
			Client_DisableDrawBox();
		}
	}

	Super::UnEquipStart();
}


// Enable Draw Box on client 
void AConstructorWeapon::Client_EnableDrawBox_Implementation()
{
	GetWorldTimerManager().SetTimer(DrawCubeHandle, this, &AConstructorWeapon::DrawBox, BoxDrawUpdate, true);
	if (AmmoRestoreTime>0)GetWorldTimerManager().SetTimer(AmmoRestoreHandle, this, &AConstructorWeapon::AmmoRestore, AmmoRestoreTime, true);
}

// Disable Drwa box on client
void AConstructorWeapon::Client_DisableDrawBox_Implementation()
{
	GetWorldTimerManager().ClearTimer(DrawCubeHandle);
	GetWorldTimerManager().ClearTimer(AmmoRestoreHandle);
}