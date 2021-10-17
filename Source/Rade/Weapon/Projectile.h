// Copyright 2015-2017 Vagen Ayrapetyan

#pragma once

#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "Projectile.generated.h"


// Custom Projectile Type
UCLASS(config = Game)
class RADE_API AProjectile : public AActor
{
	GENERATED_BODY()

public:

	AProjectile(const class FObjectInitializer& PCIP);

	void BeginPlay()override;

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//							Base Components

	// Projectile Mesh
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		UStaticMeshComponent* Mesh;

	// Collision Component
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
		USphereComponent* CollisionComp;

	// Movement Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
		class UProjectileMovementComponent* ProjectileMovement;


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//							Base Properties

	// Life time of projectile
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
		float LifeTime=5;

	// Projectile Enabled after delay - Not to explode near player
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
		float EnableDelay = 0.1f;

	// Projectile Affect Area
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
		float AffectArea = 800;

	// Projectile Explosion Damage Type
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
		TSubclassOf<class UDamageType> ExplosionDamageType;

	// Radial Damage Curve
	UPROPERTY(EditAnywhere, Category = "Projectile")
		FRuntimeFloatCurve RadialDamageCurve;

	// Radial Impulse Curve
	UPROPERTY(EditAnywhere, Category = "Projectile")
		FRuntimeFloatCurve RadialImpulseCurve;

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//							Additional  Properties and Events

	// Can The projectile Explode?
	bool bCanExplode = false;

	// Enable Projectile Explosion
	void EnableProjectile();
	
   // Player Entered
	UFUNCTION()
      void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent,
                          class AActor* OtherActor,
							     class UPrimitiveComponent* OtherComp,
                          int32 OtherBodyIndex,
								  bool bFromSweep,
								  const FHitResult& SweepResult);

	// Apply Velocity 
	void InitVelocity(const FVector& ShootDirection);

	// Explode 
	virtual void Explode();

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//							Blueprint Events


	// BP explode 
	UFUNCTION(BlueprintImplementableEvent, Category = "Projectile")
		void BP_Explode();

	// BP Hit
	UFUNCTION(BlueprintImplementableEvent, Category = "Projectile")
		void BP_Hit(UPrimitiveComponent* OtherComp, const FHitResult& Hit);

	// Explosion hit Enemy
	UFUNCTION(BlueprintImplementableEvent, Category = "Projectile")
		void BP_Explode_HitEnemy(class ARadeCharacter* TheCharacter,float HitDamage);
};
