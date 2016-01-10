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
	bRestoreAmmo = false;
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




/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///								 Fire  Events



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
		FVector StartTrace = GetFireSocketTransform().GetLocation();
		FVector Direction = CamRot.Vector();
		FVector EndTrace = StartTrace + Direction *MainFire.FireDistance;

		// Add New Block To Constructor
		if (TheLevelBlockConstructor->AddNewBlock(BlockArchetype, EndTrace, this))
		{
			UseMainFireAmmo();
			BP_BlockSpawned();
		}
			
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
		FVector StartTrace = GetFireSocketTransform().GetLocation();
		FVector Direction = CamRot.Vector();
		FVector EndTrace = StartTrace + Direction *MainFire.FireDistance;

		// Destroy block that is being looked at
		if (TheLevelBlockConstructor->DestroyBlock(EndTrace, this)) 
		{
			MainFire.AddAmmo(MainFire.FireCost, 0);
			BP_BlockDestroyed();
		}
		
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///								 Additional Events

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
		TheLevelBlockConstructor = GetWorld()->GetAuthGameMode<ARadeGameMode>()->TheLevelBlockConstructor;


	if (bRestoreAmmo && AmmoRestoreTime>0)
		GetWorldTimerManager().SetTimer(AmmoRestoreHandle, this, &AConstructorWeapon::AmmoRestore, AmmoRestoreTime, true);

	// Start Drawing Box
	if (ThePlayer)
	{
		// If you are server
		if (ThePlayer->ThePC)GetWorldTimerManager().SetTimer(DrawCubeHandle, this, &AConstructorWeapon::DrawBox, BoxDrawUpdate, true);

		// if you are client
		else Client_EnableDrawBox();
	}

}

// Unequip Start
void AConstructorWeapon::UnEquipStart()
{
	// End Drawing Box
	if (ThePlayer)
	{	
		// On Server
		if (ThePlayer->ThePC)
		{
			GetWorldTimerManager().ClearTimer(DrawCubeHandle);
			GetWorldTimerManager().ClearTimer(AmmoRestoreHandle);
		}
		// On Client
		else Client_DisableDrawBox();
	}

	Super::UnEquipStart();
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///								Draw Box Events


// Get Linebatcher pointer
static ULineBatchComponent* GetDebugLineBatcher(const UWorld* InWorld, bool bPersistentLines, float LifeTime, bool bDepthIsForeground)
{
	return (InWorld ? (bDepthIsForeground ? InWorld->ForegroundLineBatcher : ((bPersistentLines || (LifeTime > 0.f)) ? InWorld->PersistentLineBatcher : InWorld->LineBatcher)) : NULL);
}


// Draw Box Update
void AConstructorWeapon::DrawBox()
{
	// Get pointer to player
	if (!ThePlayer && GetOwner() && Cast<ARadePlayer>(GetOwner()))
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
		FVector StartTrace = GetFireSocketTransform().GetLocation();
		FVector Direction = CamRot.Vector();
		FVector EndTrace = StartTrace + Direction *MainFire.FireDistance;


		// Flooring
		EndTrace.X = 100 * round(EndTrace.X / 100);
		EndTrace.Y = 100 * round(EndTrace.Y / 100);
		EndTrace.Z = 100 * round(EndTrace.Z / 100);


		// Draw lines from 2 points in 3 axis
		ULineBatchComponent* const LineBatcher = GetDebugLineBatcher(GetWorld(), false, BoxDrawUpdate, false);
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

// Enable Draw Box on client 
void AConstructorWeapon::Client_EnableDrawBox_Implementation(){
	GetWorldTimerManager().SetTimer(DrawCubeHandle, this, &AConstructorWeapon::DrawBox, BoxDrawUpdate, true);
}

// Disable Drwa box on client
void AConstructorWeapon::Client_DisableDrawBox_Implementation(){
	GetWorldTimerManager().ClearTimer(DrawCubeHandle);
}

