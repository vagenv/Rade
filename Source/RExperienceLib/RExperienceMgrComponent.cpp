// Copyright 2015-2023 Vagen Ayrapetyan

#include "RExperienceMgrComponent.h"
#include "RUtilLib/RLog.h"

void URExperienceMgrComponent::BeginPlay ()
{
   Super::BeginPlay ();
}

void URExperienceMgrComponent::EndPlay (const EEndPlayReason::Type EndPlayReason)
{
   Super::EndPlay (EndPlayReason);
}

void URExperienceMgrComponent::AddExperiencePoints (float ExpPoint)
{
   ExperiencePoints += (int)ExpPoint;

   R_LOG_PRINTF ("[%s] Experience: %llu", *GetOwner ()->GetName (), ExperiencePoints);
}

int64 URExperienceMgrComponent::GetExperiencePoints () const
{
   return ExperiencePoints;
}

