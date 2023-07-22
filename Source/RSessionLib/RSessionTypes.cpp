// Copyright 2015-2023 Vagen Ayrapetyan

#include "RSessionTypes.h"
#include "RUtilLib/RLog.h"

// ============================================================================
//          Available session information
// ============================================================================

FRAvaiableSessionsData::FRAvaiableSessionsData ()
{
}

FRAvaiableSessionsData::FRAvaiableSessionsData (const FOnlineSessionSearchResult &SessionData_)
{
   SessionData     = FOnlineSessionSearchResult (SessionData_);
   Hostname        = SessionData_.Session.OwningUserName;
   Ping            = SessionData_.PingInMs;
   ConnectionsMax  = SessionData_.Session.SessionSettings.NumPublicConnections;
   ConnectionsBusy = ConnectionsMax - SessionData_.Session.NumOpenPublicConnections;
}

