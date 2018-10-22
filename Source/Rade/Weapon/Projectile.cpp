// Copyright 2015-2017 Vagen Ayrapetyan

#include "Weapon/Projectile.h"
#include "Rade.h"
#include "Character/RadeCharacter.h"


AProjectile::AProjectile(const class FObjectInitializer& PCIP)
	: Super(PCIP)
{

	// Set Mesh Component
	Mesh = PCIP.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("ProjectileMesh"));
   Mesh->BodyInstance.SetCollisionProfileName("Projectile");	
	Mesh->bReceivesDecals = false;
	Mesh->CastShadow = false;
   SetRootComponent (Mesh);


	// Set Collision Component
	CollisionComp = PCIP.CreateDefaultSubobject<USphereComponent>(this, TEXT("SphereComp"));
	CollisionComp->MoveIgnoreActors.Add(this);
	CollisionComp->InitSphereRadius(60.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Trigger");	
	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &AProjectile::OnBeginOverlap);
   CollisionComp->SetupAttachment(Mesh);


	//		Projectile Movement
	ProjectileMovement = PCIP.CreateDefaultSubobject<UProjectileMovementComponent>(this, TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = Mesh;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;


	// Replicate
	bReplicates = true;
	bReplicateMovement = true;

	// Set default radial Damage Curve
	FRichCurve* RadialDamageCurveData = RadialDamageCurve.GetRichCurve();
	if (RadialDamageCurveData)
	{
		RadialDamageCurveData->AddKey(0, 90);
		RadialDamageCurveData->AddKey(380, 40);
		RadialDamageCurveData->AddKey(900, 0);
	}

	// Set default Radial Impulse Curve
	FRichCurve* RadialImpulseCurveData = RadialImpulseCurve.GetRichCurve();
	if (RadialImpulseCurveData)
	{
		RadialImpulseCurveData->AddKey(0, 15000);
		RadialImpulseCurveData->AddKey(900, 9000);
	}

}


void AProjectile::BeginPlay()
{
	if (Role < ROLE_Authority)
		return;

	// Set Mobility
	Mesh->SetMobility(EComponentMobility::Movable);

	const UWorld* TheWorld = GetWorld();
	if (TheWorld)
	{
		// Set Death Delay
		FTimerHandle DeathHandle;
		TheWorld->GetTimerManager().SetTimer(DeathHandle, this, &AProjectile::Explode, LifeTime, false);
	
		// Set Enabled Delay
		FTimerHandle EnableDelayhandle;
		TheWorld->GetTimerManager().SetTimer(EnableDelayhandle, this, &AProjectile::EnableProjectile, EnableDelay, false);
	}
	
}

// Can Explode
void AProjectile::EnableProjectile(){
	bCanExplode = true;
}

// Hit Something
void AProjectile::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, 
                                 AActor* OtherActor, 
                                 UPrimitiveComponent* OtherComp, 
                                 int32 OtherBodyIndex, 
                                 bool bFromSweep, 
                                 const FHitResult & SweepResult)
{
	if (bCanExplode && 
       OtherActor != NULL && 
       OtherActor != this && 
       OtherComp != Mesh)
	{
      print ("Hit something");
	   if (bCanExplode && (OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL) && OtherComp != Mesh)
	   {
		   // Hit Something
		   BP_Hit(OtherComp, SweepResult);

		   // Boooooooom
		   Explode();	
	   }
   }
}

// Actual Projectile Explosion
void AProjectile::Explode()
{
	BP_Explode();
	
	const FVector Loc = GetActorLocation();
	for (TActorIterator<AActor> aItr(GetWorld()); aItr; ++aItr)
	{
		const float distance = GetDistanceTo(*aItr);
		if (distance<AffectArea && aItr && aItr->GetRootComponent() && aItr->GetRootComponent()->Mobility == EComponentMobility::Movable)
		{

			FVector dir = aItr->GetActorLocation() - Loc;
			dir.Normalize();
			ARadeCharacter* TheChar = Cast<ARadeCharacter>(*aItr);

			//If Rade Character, apply damage
			if (TheChar)
			{
				// BP Explosion Hit Enemy
				BP_Explode_HitEnemy(TheChar, RadialDamageCurve.GetRichCurve()->Eval(distance));

				// Apply Damage to Character
				UGameplayStatics::ApplyDamage(TheChar, RadialDamageCurve.GetRichCurve()->Eval(distance), NULL, this, ExplosionDamageType);
			}

			// Apply impulse on physics actors
			if (aItr->GetRootComponent()->IsSimulatingPhysics() && Cast<UPrimitiveComponent>(aItr->GetRootComponent()))
			{
				Cast<UPrimitiveComponent>(aItr->GetRootComponent())->AddImpulse(dir*RadialImpulseCurve.GetRichCurve()->Eval(distance));

			}
		}
	}

	Destroy();
}

// Apply Velocity to Projectile
void AProjectile::InitVelocity(const FVector& ShootDirection)
{

	// Set Movement velocity
	if (ProjectileMovement) 
	{
		ProjectileMovement->SetVelocityInLocalSpace(ShootDirection);
	}

}