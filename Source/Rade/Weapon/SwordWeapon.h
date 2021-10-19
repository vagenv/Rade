// Copyright 2015-2021 Vagen Ayrapetyan

#pragma once

#include "Weapon.h"
#include "Components/BoxComponent.h"
#include "SwordWeapon.generated.h"

// Any Kind of Melee/Sword Weapon with which can Slash
UCLASS()
class RADE_API ASwordWeapon : public AWeapon
{
   GENERATED_BODY()
public:

   ASwordWeapon(const class FObjectInitializer& PCIP);

   virtual void BeginPlay()override;

   // Hit Actors
   UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sword")
      TArray<class ARadeCharacter*>HitActors;

   //  First Person Collision BOX
   UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
      UBoxComponent* Mesh1P_MeleeAttackCollisionBox;

   //  Third Person Collision BOX
   UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
      UBoxComponent* Mesh3P_MeleeAttackCollisionBox;


   // Box Overlap Event
   UFUNCTION()
      virtual void OnSwordWeaponBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent,
                                                class AActor* OtherActor,
                                                class UPrimitiveComponent* OtherComp,
                                                int32 OtherBodyIndex, 
                                                bool bFromSweep,
                                                const FHitResult &SweepResult);

   // Is Slashing/Tracing/Cutting
   bool bTracingMeleeAttack;

   //  Start Attack Tracing
   virtual void StartMeleeAttackTrace();

   // Stop Attack Tracing
   virtual void EndMeleeAttackTrace();
};
