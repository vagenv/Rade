// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "Serialization/BufferArchive.h"
#include "Serialization/MemoryReader.h"

#include "RSaveInterface.generated.h"

class UWorld;
class URSaveMgr;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
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
		virtual void Init_Save (UObject* WorldContextObject, const FString &SaveId);

	// Must be implemented
	virtual void OnSave (FBufferArchive &SaveData) = 0;
	virtual void OnLoad (FMemoryReader  &LoadData) = 0;

	// Internal events
   UFUNCTION()
      virtual void OnSave_Internal ();

   UFUNCTION()
      virtual void OnLoad_Internal ();

private:

	// Values for internal tracking
	FString    ObjectSaveId;
	UWorld 	 *World   = nullptr;
	URSaveMgr *SaveMgr = nullptr;
};

