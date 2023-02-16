// Copyright 2015-2023 Vagen Ayrapetyan

#include "RStatusEffect.h"
#include "RUtilLib/RLog.h"

FRPassiveStatusEffect FRPassiveStatusEffect::operator + (const FRPassiveStatusEffect &obj) const
{
   FRPassiveStatusEffect res;
   res.Scale = Scale;
   res.Target = Target;
   res.Value = Value + obj.Value;
   return res;
}

ARActiveStatusEffect::ARActiveStatusEffect ()
{

}

void ARActiveStatusEffect::Start (AActor * Causer_, AActor* Target_)
{
   if (isRunning) return;

   isRunning = true;
   Causer = Causer_;
   Target = Target_;
   Started ();
}

void ARActiveStatusEffect::Started ()
{
   BP_Started ();
   GetWorld ()->GetTimerManager ().SetTimer (TimerToEnd, this, &ARActiveStatusEffect::Ended, Duration, false);
   OnStart.Broadcast ();
}

void ARActiveStatusEffect::Ended ()
{
   BP_Ended ();
   OnEnd.Broadcast ();

   Destroy ();
}

