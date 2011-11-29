/// ext3.cpp
#include "stdafx.h"
#include <afxdllx.h>

static AFX_EXTENSION_MODULE ext3DLL = {0, 0};

extern "C" int APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{	
	UNREFERENCED_PARAMETER(lpReserved);

	if (dwReason == DLL_PROCESS_ATTACH)
	{	
		if (!AfxInitExtensionModule(ext3DLL, hInstance)) return 0;
		new CDynLinkLibrary(ext3DLL);
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		AfxTermExtensionModule(ext3DLL);
	}
	return 1;   // ok
}