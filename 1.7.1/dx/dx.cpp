// dx.cpp
#include "stdafx.h"
#include <afxdllx.h>

static AFX_EXTENSION_MODULE dxDLL = {0, 0};
extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{	
	UNREFERENCED_PARAMETER(lpReserved);
	if (dwReason == DLL_PROCESS_ATTACH)
	{		
		if (!AfxInitExtensionModule(dxDLL, hInstance)) return 0;
		new CDynLinkLibrary(dxDLL);
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{	
		AfxTermExtensionModule(dxDLL);
	}
	return 1;   // ok
}