// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "Serialization/BufferArchive.h"
#include "Serialization/MemoryReader.h"

#include "RSaveInterface.generated.h"

class UWorld;
class URSaveGame;
class URWorldSaveMgr;


// This class does not need to be modified.
UINTERFACE(MinimalAPI, NotBlueprintable)
class URSaveInterface : public UInterface
{
	GENERATED_BODY()
};

class RSAVELIB_API IRSaveInterface
{
	GENERATED_BODY()

protected:

	// Must be called on BeginPlay
	UFUNCTION()
		virtual void Init_Save (const UObject* WorldContextObject, const FString &SaveId_);

	// --- Must be implemented
	virtual void OnSave (FBufferArchive &SaveData) = 0;
	virtual void OnLoad (FMemoryReader  &LoadData) = 0;

	// Internal events
   UFUNCTION()
      virtual void OnSave_Internal (URSaveGame* SaveGame);

   UFUNCTION()
      virtual void OnLoad_Internal (URSaveGame* SaveGame);

private:

	// Values for internal tracking
	FString  	          SaveId;
	const UWorld*         World   = nullptr;
			URWorldSaveMgr* SaveMgr = nullptr;
};

