// Copyright 2015 Vagen Ayrapetyan

#include "Rade.h"
#include "Weapon/RifleWeapon.h"
#include "Character/RadePlayer.h"


void ARifleWeapon::Fire()
{
	Super::Fire();

	if (Mesh1P && ThePlayer && ThePlayer->Controller)
	{
		/// Get Camera Location and Rotation
		FVector CamLoc;
		FRotator CamRot;
		ThePlayer->Controller->GetPlayerViewPoint(CamLoc, CamRot); 

		// Get Mesh Fire Socket Location and calculatre Fire end point
		const FVector StartTrace = GetFireSocketTransform().GetLocation();;
		const FVector Direction = CamRot.Vector();
		const FVector EndTrace = StartTrace + Direction *MainFire.FireDistance;


		// Set trace values and perform trace to retrieve hit info
		FCollisionQueryParams TraceParams(FName(TEXT("WeaponTrace")), true, this);
		TraceParams.bTraceAsyncScene = true;
		TraceParams.bReturnPhysicalMaterial = true;
		TraceParams.AddIgnoredActor(ThePlayer);

		// Trace Hit
		FHitResult Hit(ForceInit);
		if (GetWorld()->LineTraceSingleByChannel(Hit, StartTrace, EndTrace, ECC_WorldStatic, TraceParams))
		{

			// Check if player character
			ARadeCharacter* EnemyPlayer = Cast<ARadeCharacter>(Hit.GetActor());
			if (EnemyPlayer)
			{
				// Hit Enemy
				BP_HitEnemy(Hit);

				// Apply Damage
				UGameplayStatics::ApplyDamage(EnemyPlayer, MainFire.FireDamage, ThePlayer->Controller, Cast<AActor>(this), UDamageType::StaticClass());
			}
		}
	}
	
}





