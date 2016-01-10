// Copyright 2015 Vagen Ayrapetyan

#include "Rade.h"
#include "RadeDamageVolume.h"
#include "Character/RadeCharacter.h"



// Actual Damage Event
void ARadeDamageVolume::ApplyDamageToCharacters()
{
	// Apply Damage to all Actor inside volume 
	for (ARadeCharacter* DmgCharacter : DamageCharacterList)
	{
		if (DmgCharacter)
		{
			// If Actor is Dead Remove from damage list
			if (DmgCharacter->bDead)
				DamageCharacterList.Remove(DmgCharacter);
		
			// Apply Damage To Character
			else UGameplayStatics::ApplyDamage(DmgCharacter, ContinuousDamage, NULL, this, VolumeDamageType);
		}
	}
}


// Actor Entered Damage Volume
void ARadeDamageVolume::ActorEnteredVolume(AActor* Other)
{
	
	Super::ActorEnteredVolume(Other);

	// If the actor entered is Rade CHaracter , Add him to Damage List
	if (Cast<ARadeCharacter>(Other) && !Cast<ARadeCharacter>(Other)->bDead)
	{
		DamageCharacterList.Add(Cast<ARadeCharacter>(Other));

		// Apply Initial Damage to actor
		UGameplayStatics::ApplyDamage(Other, InitialDamage, NULL, this, VolumeDamageType);
	}

	// If timer is not active and damage list is not emppty , start timer
	if (!GetWorldTimerManager().IsTimerActive(DamageRateHandle) && DamageCharacterList.Num()>0)
		GetWorldTimerManager().SetTimer(DamageRateHandle,this,&ARadeDamageVolume::ApplyDamageToCharacters,DamageRate, true);

}

// Someone Left the volume
void ARadeDamageVolume::ActorLeavingVolume(AActor* Other)
{
	Super::ActorLeavingVolume(Other);

	// If the actor is of Rade Character type remove from damage list
	if (Cast<ARadeCharacter>(Other))
		DamageCharacterList.Remove(Cast<ARadeCharacter>(Other));

	// If Timer is Active and Damage list is empty, stop timer
	if (GetWorldTimerManager().IsTimerActive(DamageRateHandle) && DamageCharacterList.Num()<1)
		GetWorldTimerManager().ClearTimer(DamageRateHandle);
}