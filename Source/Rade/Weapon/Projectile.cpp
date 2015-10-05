// Copyright 2015 Vagen Ayrapetyan

#include "Rade.h"

#include "Weapon/Projectile.h"
#include "Character/BaseCharacter.h"

AProjectile::AProjectile(const class FObjectInitializer& PCIP)
	: Super(PCIP)
{

	//RootPoint = PCIP.CreateDefaultSubobject<USceneComponent>(this, TEXT("RootPoint"));

	//		The Collision Component 
	CollisionComp = PCIP.CreateDefaultSubobject<USphereComponent>(this, TEXT("SphereComp"));
	CollisionComp->MoveIgnoreActors.Add(this);
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");	
	CollisionComp->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);	
	RootComponent = CollisionComp;



	Mesh = PCIP.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("ProjectileMesh"));
	Mesh->bReceivesDecals = false;
	Mesh->CastShadow = false;
	Mesh->AttachParent = CollisionComp;

	//		Projectile Movement
	ProjectileMovement = PCIP.CreateDefaultSubobject<UProjectileMovementComponent>(this, TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	bReplicates = true;


	FRichCurve* RadialDamageCurveData = RadialDamageCurve.GetRichCurve();
	RadialDamageCurveData->AddKey(0, 90);
	RadialDamageCurveData->AddKey(380, 40);
	RadialDamageCurveData->AddKey(900, 0);
	

	FRichCurve* RadialImpulseCurveData = RadialImpulseCurve.GetRichCurve();
	RadialImpulseCurveData->AddKey(0, 15000);
	RadialImpulseCurveData->AddKey(900, 9000);


	
}

void AProjectile::BeginPlay()
{
	if (Role < ROLE_Authority)
		return;

	Mesh->SetMobility(EComponentMobility::Movable);
	const UWorld* theWorld = GetWorld();
	if (theWorld)
	{
		//printg("Timer started");
		FTimerHandle DeathHandle;
		theWorld->GetTimerManager().SetTimer(DeathHandle, this, &AProjectile::Explode, LifeTime,false);
	
		FTimerHandle EnableDelayhandle;
		theWorld->GetTimerManager().SetTimer(EnableDelayhandle, this, &AProjectile::EnableProjectile, EnableDelay, false);
	}
	
}
void AProjectile::EnableProjectile()
{
	bCanExplode = true;
}
void AProjectile::OnHit(AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{

	if (!bCanExplode)return;

	if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL) && OtherComp!=Mesh)
	{
		Explode();	
	}
	else 
	{
		BP_Hit(OtherComp,Hit);
	}
}

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

			FRichCurve* RadialDamageCurveData = RadialDamageCurve.GetRichCurve();
			FRichCurve* RadialImpulseCurveData = RadialImpulseCurve.GetRichCurve();


			ABaseCharacter* theChar = Cast<ABaseCharacter>(*aItr);

			//If Player apply damage
			if (theChar && RadialDamageCurveData)
			{
				//printr("Apply Damage");
				UGameplayStatics::ApplyDamage(theChar, RadialDamageCurveData->Eval(distance), NULL, this, ExplosionDamageType);
			}
			
			if (RadialImpulseCurveData && aItr->GetRootComponent()->IsSimulatingPhysics() && Cast<UPrimitiveComponent>(aItr->GetRootComponent()))
			{
				Cast<UPrimitiveComponent>(aItr->GetRootComponent())->AddImpulse(dir*RadialImpulseCurveData->Eval(distance));
				
			}
		}

	}




	Destroy();
}
void AProjectile::InitVelocity(const FVector& ShootDirection)
{

	if (ProjectileMovement)
	{
		//print("Init Velocity");
		// set the projectile's velocity to the desired direction
		ProjectileMovement->Velocity = ShootDirection * ProjectileMovement->InitialSpeed;
	}
}