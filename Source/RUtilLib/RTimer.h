// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "Engine.h"

#define RTIMER_START(TimerHandle, Object, ObjectFunc, Rate, Loop)              \
      {																								 \
         UWorld *TimerWorld = URUtil::GetWorld (Object);								 \
         if (TimerWorld && ensure (!TimerHandle.IsValid ())) {					    \
            TimerWorld->GetTimerManager ().SetTimer (TimerHandle,					 \
                                                     Object, ObjectFunc,		 \
                                                     Rate,							 \
                                                     Loop);							 \
         }																							 \
      };																								 \


#define RTIMER_STOP(TimerHandle, Object)                                       \
      {																								 \
         UWorld *TimerWorld = URUtil::GetWorld (Object);								 \
         if (TimerWorld && TimerHandle.IsValid ()) {		         				 \
            TimerWorld->GetTimerManager ().ClearTimer (TimerHandle);     		 \
         }																					 		 \
      };		                                                                   \


