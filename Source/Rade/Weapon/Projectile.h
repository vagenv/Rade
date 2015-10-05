// Copyright 2015 Vagen Ayrapetyan

#pragma once

#include "GameFramework/Actor.h"
#include "Projectile.generated.h"


UCLASS(config = Game)
class RADE_API AProjectile : public AActor
{
	GENERATED_BODY()

public:

	AProjectile(const class FObjectInitializer& PCIP);


	void BeginPlay()override;

	bool bCanExplode = false;
	void EnableProjectile();


	// weapon mesh: 1st person view //
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		UStaticMeshComponent* Mesh;

		/** Sphere collision component */
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
		USphereComponent* CollisionComp;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
		class UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
		float LifeTime=5;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
		float EnableDelay = 0.3f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
		float AffectArea = 800;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
		TSubclassOf<class UDamageType> ExplosionDamageType;
	UPROPERTY(EditAnywhere, Category = "Projectile")
		FRuntimeFloatCurve RadialDamageCurve;
	UPROPERTY(EditAnywhere, Category = "Projectile")
		FRuntimeFloatCurve RadialImpulseCurve;



	/** called when projectile hits something */
	UFUNCTION()
		void OnHit(AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	void InitVelocity(const FVector& ShootDirection);


	virtual void Explode();
	UFUNCTION(BlueprintImplementableEvent, Category = "Projectile")
		void BP_Explode();

	UFUNCTION(BlueprintImplementableEvent, Category = "Projectile")
		void BP_Hit(UPrimitiveComponent* OtherComp,const FHitResult& Hit);

	
};
