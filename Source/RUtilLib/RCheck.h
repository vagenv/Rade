// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "RLog.h"

#define R_IS_VALID_WORLD (IsValid (GetWorld ()) && !GetWorld ()->bIsTearingDown)


#define R_IS_NET_ADMIN (IsValid (GetOwner ()) && GetOwner ()->HasAuthority ())

#define R_RETURN_IF_NOT_ADMIN                                     \
   if (!R_IS_NET_ADMIN) {                                         \
      R_LOG ("Client has no authority to perform this action.");  \
      return;                                                     \
   }                                                              \

#define R_RETURN_IF_NOT_ADMIN_BOOL                                \
   if (!R_IS_NET_ADMIN) {                                         \
      R_LOG ("Client has no authority to perform this action.");  \
      return false;                                               \
   }                                                              \

#define R_RETURN_IF_NOT_ADMIN_NULL                                \
   if (!R_IS_NET_ADMIN) {                                         \
      R_LOG ("Client has no authority to perform this action.");  \
      return nullptr;                                             \
   }                                                              \

