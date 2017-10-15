// HookDll.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "keyHelper.h"

KEYHELPER_API LONGLONG WINAPI version()
{
    return 0x201506271130; ///< version is date. e.g. 2015-06-27 11:30
}

