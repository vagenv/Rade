// Copyright 2015 Vagen Ayrapetyan

#pragma once

#include "GameFramework/PhysicsVolume.h"
#include "RadeDamageVolume.generated.h"

//  Custom Rade Damage Volume.
UCLASS()
class RADE_API ARadeDamageVolume : public APhysicsVolume
{
	GENERATED_BODY()
	
protected:

	// Character Entered Volume
	virtual void ActorEnteredVolume(class AActor* Other) override;

	// Character Left Volume
	virtual void ActorLeavingVolume(class AActor* Other);

	// Damage Handle 
	FTimerHandle DamageRateHandle;

	// Actual Damage Event
	virtual void ApplyDamageToCharacters();

public:

	// Initial Damage , upon entering the volume.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rade Volume")
		float InitialDamage = 50;

	// Continues damage to all Rade Character who are Inside Volume
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rade Volume")
		float ContinuousDamage = 10;

	// Damage Rate, Delay before next damage event
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rade Volume")
		float DamageRate = 0.5f;

	// Damage Type
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rade Volume")
		TSubclassOf<class UDamageType> VolumeDamageType;

	// Current Rade Characters Inside Volume
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rade Volume")
		TArray<class ARadeCharacter*> DamageCharacterList;

};
