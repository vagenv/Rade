// Copyright 2015 Vagen Ayrapetyan

#include "Rade.h"
#include "RadeGameMode.h"

#include "Weapon/ConstructorWeapon.h"

#include "Character/RadeCharacter.h"

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

	if (GetWorld() && GetWorld()->GetAuthGameMode() && GetWorld()->GetAuthGameMode<ARadeGameMode>())
	{
		TheLevelBlockConstructor = GetWorld()->GetAuthGameMode<ARadeGameMode>()->TheLevelBlockConstructor;
	}
}

static ULineBatchComponent* GetDebugLineBatcher(const UWorld* InWorld, bool bPersistentLines, float LifeTime, bool bDepthIsForeground)
{
	return (InWorld ? (bDepthIsForeground ? InWorld->ForegroundLineBatcher : ((bPersistentLines || (LifeTime > 0.f)) ? InWorld->PersistentLineBatcher : InWorld->LineBatcher)) : NULL);
}




void AConstructorWeapon::Fire()
{
	Super::Fire();

	if (TheLevelBlockConstructor == NULL)
	{
		if (GetWorld() && GetWorld()->GetAuthGameMode() && GetWorld()->GetAuthGameMode<ARadeGameMode>())
		{
			TheLevelBlockConstructor = GetWorld()->GetAuthGameMode<ARadeGameMode>()->TheLevelBlockConstructor;
		}
	}

	if (TheLevelBlockConstructor)
	{

		FVector CamLoc;
		FRotator CamRot;
		ThePlayer->Controller->GetPlayerViewPoint(CamLoc, CamRot); // Get the camera position and rotation
		FVector StartTrace = Mesh1P->GetSocketLocation(TEXT("FireSocket"));
		//	CamLoc; // trace start is the camera location
		FVector Direction = CamRot.Vector();
		FVector EndTrace = StartTrace + Direction *MainFire.FireDistance;

		if (TheLevelBlockConstructor->AddNewBlock(BlockArchetype, EndTrace, this)) UseMainFireAmmo();
	}
}

void AConstructorWeapon::AltFire()
{
	Super::AltFire();

	if (GetWorld() && GetWorld()->GetAuthGameMode() && GetWorld()->GetAuthGameMode<ARadeGameMode>())
	{
		TheLevelBlockConstructor = GetWorld()->GetAuthGameMode<ARadeGameMode>()->TheLevelBlockConstructor;
	}

	if (TheLevelBlockConstructor)
	{
		FVector CamLoc;
		FRotator CamRot;
		ThePlayer->Controller->GetPlayerViewPoint(CamLoc, CamRot); // Get the camera position and rotation
		FVector StartTrace = Mesh1P->GetSocketLocation(TEXT("FireSocket"));
		FVector Direction = CamRot.Vector();
		FVector EndTrace = StartTrace + Direction *MainFire.FireDistance;

		if (TheLevelBlockConstructor->DestroyBlock(EndTrace, this)) 
		{
			MainFire.AddAmmo(MainFire.FireCost,0);
		}


	}
}

void AConstructorWeapon::DrawBox()
{


	if (!ThePlayer && GetOwner()  && Cast<ARadeCharacter>(GetOwner()))
	{
		ThePlayer = Cast<ARadeCharacter>(GetOwner());
	}

	if (ThePlayer && ThePlayer->Controller)
	{
		FVector CamLoc;
		FRotator CamRot;
		ThePlayer->Controller->GetPlayerViewPoint(CamLoc, CamRot); // Get the camera position and rotation
		FVector StartTrace = Mesh1P->GetSocketLocation(TEXT("FireSocket"));
		//	CamLoc; // trace start is the camera location
		FVector Direction = CamRot.Vector();
		FVector EndTrace = StartTrace + Direction *MainFire.FireDistance;


		// Flooring
		EndTrace.X = 100 * round(EndTrace.X / 100);
		EndTrace.Y = 100 * round(EndTrace.Y / 100);
		EndTrace.Z = 100 * round(EndTrace.Z / 100);


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




void AConstructorWeapon::AmmoRestore(){
	MainFire.AddAmmo(AmmoRestoreValue, 0);
}


void AConstructorWeapon::EquipEnd()
{
	Super::EquipEnd();

	if (!TheLevelBlockConstructor && GetWorld() && GetWorld()->GetAuthGameMode() && GetWorld()->GetAuthGameMode<ARadeGameMode>())
	{
		TheLevelBlockConstructor = GetWorld()->GetAuthGameMode<ARadeGameMode>()->TheLevelBlockConstructor;
	}

	if (ThePlayer)
	{
		if (ThePlayer->ThePC)
		{
			GetWorldTimerManager().SetTimer(DrawCubeHandle, this, &AConstructorWeapon::DrawBox, BoxDrawUpdate, true);
			if (bRestoreAmmo && AmmoRestoreTime>0)GetWorldTimerManager().SetTimer(AmmoRestoreHandle, this, &AConstructorWeapon::AmmoRestore, AmmoRestoreTime, true);
		}
		else 
		{
			Client_EnableDrawBox();
		}
	}

}

void AConstructorWeapon::UnEquipStart()
{
	if (ThePlayer)
	{
		if (ThePlayer->ThePC)
		{
			GetWorldTimerManager().ClearTimer(DrawCubeHandle);
			GetWorldTimerManager().ClearTimer(AmmoRestoreHandle);
		}
		else 
		{
			Client_DisableDrawBox();
		}
	}
	Super::UnEquipStart();
}



void AConstructorWeapon::Client_EnableDrawBox_Implementation()
{
	GetWorldTimerManager().SetTimer(DrawCubeHandle, this, &AConstructorWeapon::DrawBox, BoxDrawUpdate, true);
	if (AmmoRestoreTime>0)GetWorldTimerManager().SetTimer(AmmoRestoreHandle, this, &AConstructorWeapon::AmmoRestore, AmmoRestoreTime, true);
}

void AConstructorWeapon::Client_DisableDrawBox_Implementation()
{
	GetWorldTimerManager().ClearTimer(DrawCubeHandle);
	GetWorldTimerManager().ClearTimer(AmmoRestoreHandle);
}