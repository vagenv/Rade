// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#ifdef WIN32
#  define V_DIR_SLASH '\\'
#endif

#ifdef LINUX
#  define V_DIR_SLASH '/'
#endif

#define __FILENAME__ (strrchr(__FILE__, V_DIR_SLASH) ? strrchr(__FILE__, V_DIR_SLASH) + 1 : __FILE__)

