// dbloader.h : main header file for the dbloader application
//
#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include "afxdao.h"


// CdbApp:
// See dbloader.cpp for the implementation of this class
//

class CdbApp : public CWinApp
{
public:
	CdbApp();
private:	

// Overrides
public:
	BOOL InitInstance();
	int ExitInstance();

// Implementation

public:
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CdbApp theApp;