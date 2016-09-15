// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#pragma once
#include <SdkDdkVer.h>
#undef  NTDDI_VERSION
#define NTDDI_VERSION NTDDI_VISTA
#undef  _WIN32_WINNT
#define _WIN32_WINNT _WIN32_WINNT_VISTA

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
//#define OEMRESOURCE		//For 'LoadOEMCursor' function

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include "resource.h"
#include "common.h"
#include "Config.h"
#include "Debug.h"
#include "Log.h"
