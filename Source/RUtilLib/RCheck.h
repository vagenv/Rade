#pragma once

#include "RLog.h"

#define R_IS_NET_ADMIN (GetOwner ()->HasAuthority ())

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

