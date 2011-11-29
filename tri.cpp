// tri.cpp
#include "stdafx.h"
#include "doc.h"
#include "hlapi_c.h"
#include "view.h"
#include "tri.h"
#include "htmlhelp.h" 

static const CLSID			clsid =	{0x1024D545, 0xC833, 0x40A5, 
									{0x92, 0x81, 0x95, 0x1, 0x7A, 0x38, 0x49, 0x73}};
const GUID CDECL BASED_CODE _tlid = {0xBAC9CDCE, 0xFBE5, 0x4B4D, 
									{0x8A, 0x91, 0xB8, 0xAC, 0x61, 0xFB, 0xEA, 0x9D}};
const WORD					_wVerMajor = 1, _wVerMinor = 0;

Byte						RefKey[8] = {72, 65, 82, 68, 76, 79, 67, 75};
Byte						VerKey[8] = {0xE9, 0x8B, 0x14, 0x08, 0x3E, 0x7E, 0xA1, 0x14};
UINT						lx, ly;

extern bool					bmove;		
extern char					dec, _log[MAX_PATH], msg[MAX_PATH];
extern BOOL					bread;
extern LPDIRECT3DDEVICE9	lpd3d;

TCHAR getSystemDecimalSeparator()
{
    TCHAR buffer[5];    
	if (::GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SDECIMAL | LOCALE_USE_CP_ACP, buffer, 4) > 0) 
		return buffer[0];
    if (::GetLocaleInfo(LOCALE_SYSTEM_DEFAULT, LOCALE_SDECIMAL | LOCALE_USE_CP_ACP, buffer, 4) > 0) 
		return buffer[0];

    return (TCHAR)_T(".");
}
// CtriApp
CtriApp app;
CtriApp::CtriApp()
{
	m_pFrame = 0;
	lpszPath = new TCHAR[_MAX_PATH];
	memset(lpszPath, 0, _MAX_PATH);		
	dec = getSystemDecimalSeparator();	
}
CtriApp::~CtriApp()
{
	SAFE_DELETE_ARRAY(lpszPath);
	SAFE_DELETE_ARRAY(stg._c_set.colors);	
}
bool CtriApp::hardlock()
{		
	Word LoginStatus = HL_LOGIN(MODAD, LOCAL_DEVICE, RefKey, VerKey);
	if (LoginStatus != STATUS_OK)
	{
		memset(msg, 0, _MAX_PATH);
		sprintf_s(msg, MAX_PATH, "%s (Error: %d)", HL_ERRMSG(LoginStatus, 0, 0, 0), LoginStatus);			
		writelog(_log, msg);		
		::MessageBox(0, msg, m_pszAppName, MB_ICONSTOP);				
		return 0;	
	}	
	HL_LOGOUT();
	return 1;
}
BOOL CtriApp::InitInstance()
{
	CWinApp::InitInstance();
	AfxEnableControlContainer();	
	AfxOleInit();	
	// log file	
	memset(_log, 0, MAX_PATH);
	strcpy_s(_log, MAX_PATH, __argv[0]);
	PathRenameExtension(_log, ".log");	
	// hardlock key
//	if (!hardlock()) return 0;	
	// registry
	SetRegistryKey(_T("bm"));
	bool breg = loadRegSettings();	
	HDC hDC = ::GetDC(0);
	lx = ::GetDeviceCaps(hDC, LOGPIXELSX);
	ly = ::GetDeviceCaps(hDC, LOGPIXELSY);
	::ReleaseDC(0, hDC);
	// Register document templates		
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(C3dDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(C3dView));
	AddDocTemplate(pDocTemplate);
	m_server.ConnectTemplate(clsid, pDocTemplate, 1);
	// Parse command line for standard shell commands, DDE, file open	
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	// first time
	if (!breg)	
	{
		OnChangeOption();
		cmdInfo.m_nShellCommand = CCommandLineInfo::FileNew;	
	}
	else 
	{
		if (stg._g_set.bauto) 
		{
			cmdInfo.m_nShellCommand = CCommandLineInfo::FileOpen;	
			cmdInfo.m_strFileName = lpszPath;
		}
	}
	// Automation		
	if (cmdInfo.m_bRunEmbedded || cmdInfo.m_bRunAutomated)
	{
		COleTemplateServer::RegisterAll();		
		return 1;
	}
	else if (cmdInfo.m_nShellCommand == CCommandLineInfo::AppUnregister)
	{
		UnregisterShellFileTypes();
		m_server.UpdateRegistry(OAT_DISPATCH_OBJECT, 0, 0, 0);
		AfxOleUnregisterTypeLib(_tlid, _wVerMajor, _wVerMinor);
	}	
	else
	{
		m_server.UpdateRegistry(OAT_DISPATCH_OBJECT);
		COleObjectFactory::UpdateRegistryAll();
		AfxOleRegisterTypeLib(AfxGetInstanceHandle(), _tlid);
	}			
	if (!ProcessShellCommand(cmdInfo)) return 0;
	if (!lpd3d) return 0;	
	// The one and only window has been initialized, so show and update it		
	m_pFrame->ShowWindow(SW_SHOW);
	m_pFrame->UpdateWindow();			

	return 1;
}
int CtriApp::ExitInstance()
{
	saveRegSettings();				
	return CWinApp::ExitInstance();
}
bool CtriApp::loadRegSettings()
{	
	HKEY hKey;
	DWORD dwType, dwSize;
	char subKey[MAX_PATH];
	// common
	sprintf_s(subKey, MAX_PATH, "Software\\%s\\%s", m_pszRegistryKey, m_pszProfileName); 	
	if (ERROR_SUCCESS != RegOpenKeyEx(HKEY_CURRENT_USER, subKey, 0, KEY_READ, &hKey)) return 0; 		
	
	dwType = REG_DWORD;
	dwSize = 4;
	RegQueryValueEx(hKey, "auto", 0, &dwType, (LPBYTE)&stg._g_set.bauto, &dwSize);		
	
	dwType = REG_SZ;
	dwSize = _MAX_PATH;
	RegQueryValueEx(hKey, "path", 0, &dwType, (LPBYTE)lpszPath, &dwSize);  		
	
	dwSize = 16;
	RegQueryValueEx(hKey, "zcoef", 0, &dwType, (LPBYTE)subKey, &dwSize);	
	RegCloseKey(hKey);		
	
	sscanf_s(subKey, "%f", &stg._g_set.zcoef);	
	
	sprintf_s(subKey, MAX_PATH, "Software\\%s\\%s\\legend", m_pszRegistryKey, m_pszProfileName); 	
	if (ERROR_SUCCESS != RegOpenKeyEx(HKEY_CURRENT_USER, subKey, 0, KEY_READ, &hKey)) return 1;

	dwType = REG_BINARY;	
	RegQueryValueEx(hKey, "legend", 0, &dwType, 0, &dwSize);  			
	if (dwSize > 0)
	{
		stg._c_set._size = dwSize >> 2;
		stg._c_set.setArr();		
		RegQueryValueEx(hKey, "legend", 0, &dwType, (LPBYTE)stg._c_set.colors, &dwSize);  		
	}
	
	dwType = REG_DWORD;
	dwSize = 4;	
	RegQueryValueEx(hKey, "gray", 0, &dwType, (LPBYTE)&stg._c_set.bgray, &dwSize);  		
	RegQueryValueEx(hKey, "inv", 0, &dwType, (LPBYTE)&stg._c_set.binv, &dwSize);  		
	RegCloseKey(hKey);	
	// colors
	sprintf_s(subKey, MAX_PATH, "Software\\%s\\%s\\colors", m_pszRegistryKey, m_pszProfileName); 
	if (ERROR_SUCCESS != RegOpenKeyEx(HKEY_CURRENT_USER, subKey, 0, KEY_READ, &hKey)) return 1;		
	
	RegQueryValueEx(hKey, "background", 0, &dwType, (LPBYTE)&stg._g_set.dw_bkgnd, &dwSize);  		
	RegQueryValueEx(hKey, "cell", 0, &dwType, (LPBYTE)&stg._g_set.dw_cell, &dwSize);  	
	RegQueryValueEx(hKey, "lfont", 0, &dwType, (LPBYTE)&stg._g_set.cl_lfont, &dwSize);  				
	// well
	wsprintf(subKey, "Software\\%s\\%s\\well", m_pszRegistryKey, m_pszProfileName); 
	if (ERROR_SUCCESS != RegOpenKeyEx(HKEY_CURRENT_USER, subKey, 0, KEY_READ, &hKey)) return 1;		

	dwType = REG_DWORD;
	dwSize = 4;
	RegQueryValueEx(hKey, "main", 0, &dwType, (LPBYTE)&stg._w_set.dw_main, &dwSize);  		
	RegQueryValueEx(hKey, "sel", 0, &dwType, (LPBYTE)&stg._w_set.dw_sel, &dwSize);  		
	RegQueryValueEx(hKey, "wfont", 0, &dwType, (LPBYTE)&stg._w_set.dw_font, &dwSize); 				
	RegQueryValueEx(hKey, "wh", 0, &dwType, (LPBYTE)&stg._w_set.wh, &dwSize); 				
	RegQueryValueEx(hKey, "wi", 0, &dwType, (LPBYTE)&stg._w_set.wi, &dwSize); 				
	RegQueryValueEx(hKey, "wr1", 0, &dwType, (LPBYTE)&stg._w_set.wr1, &dwSize); 			
	RegQueryValueEx(hKey, "wr2", 0, &dwType, (LPBYTE)&stg._w_set.wr2, &dwSize); 	
	// font_desc	
	dwType = REG_BINARY;		
	RegQueryValueEx(hKey, "ds", 0, &dwType, 0, &dwSize);  		
	RegQueryValueEx(hKey, "ds", 0, &dwType, (LPBYTE)&stg._w_set.ds, &dwSize);  		

	char value[8];
	dwType = REG_SZ;
	dwSize = 8;	
	RegQueryValueEx(hKey, "wxo", 0, &dwType, (LPBYTE)value, &dwSize);  	
	sscanf_s(value, "%d", &stg._w_set.wxo); 
	RegQueryValueEx(hKey, "wyo", 0, &dwType, (LPBYTE)value, &dwSize);  	
	sscanf_s(value, "%d", &stg._w_set.wyo); 
	RegCloseKey(hKey);
	
	return 1;
}
BOOL CtriApp::OnIdle(LONG lCount)
{
	if (m_pFrame && (!m_pFrame->IsIconic())) 
		m_pFrame->m_p3dview->InvalidateRect(0, 0);	

	return CWinApp::OnIdle(lCount);
}
BOOL CtriApp::ProcessShellCommand(CCommandLineInfo& rCmdInfo)
{
	BOOL bResult = 1;
	switch (rCmdInfo.m_nShellCommand)
	{
	case CCommandLineInfo::FileNew:
		OnFileNew();
		if (m_pFrame == 0) bResult = 0;		
		break;
	case CCommandLineInfo::FileOpen:
		OnFileNew();		
		if (m_pFrame == 0) bResult = 0;	
		else OpenDocumentFile(rCmdInfo.m_strFileName);			
		break;
	default:bResult = CWinApp::ProcessShellCommand(rCmdInfo);
	}	
	return bResult;
}
CDocument* CtriApp::OpenDocumentFile(LPCTSTR lpszFileName)
{
	bmove = 0;
	CDocument* pdoc = CWinApp::OpenDocumentFile(lpszFileName);	
	return pdoc;
}
void CtriApp::main()
{
	m_pFrame = (CMainFrame*)m_pMainWnd;	
}
void CtriApp::OnFileNew()
{
	if (!OnCmdMsg(ID_FILE_NEW, 0, 0, 0)) CWinApp::OnFileNew();	
	main();
}
void CtriApp::saveRegColors()
{
	HKEY hKey;
    TCHAR subKey[_MAX_PATH];	
	// legend
	wsprintf(subKey, "Software\\%s\\%s\\legend", m_pszRegistryKey, m_pszProfileName); 	
	if (ERROR_SUCCESS != RegOpenKeyEx(HKEY_CURRENT_USER, subKey, 0, KEY_WRITE, &hKey))	
		RegCreateKey(HKEY_CURRENT_USER, subKey, &hKey); 	
	
	RegSetValueEx(hKey, "gray", 0, REG_DWORD, (LPBYTE)&stg._c_set.bgray, 4); 		
	RegSetValueEx(hKey, "inv", 0, REG_DWORD, (LPBYTE)&stg._c_set.binv, 4); 		
	RegSetValueEx(hKey, "legend", 0, REG_BINARY, (LPBYTE)stg._c_set.colors, 
		stg._c_set._size << 2); 		
	RegCloseKey(hKey);
}
void CtriApp::saveRegSettings()
{
	HKEY hKey;
    TCHAR subKey[_MAX_PATH];
	// common
	wsprintf(subKey, "Software\\%s\\%s", m_pszRegistryKey, m_pszProfileName); 	
	if (ERROR_SUCCESS != RegOpenKeyEx(HKEY_CURRENT_USER, subKey, 0, KEY_WRITE, &hKey))	
		RegCreateKey(HKEY_CURRENT_USER, subKey, &hKey); 	

	RegSetValueEx(hKey, "auto", 0, REG_DWORD, (LPBYTE)&stg._g_set.bauto, 4); 		
	RegSetValueEx(hKey, "path", 0, REG_SZ, (LPBYTE)lpszPath, _MAX_PATH); 		
	sprintf_s(subKey, MAX_PATH, "%f", stg._g_set.zcoef);
	RegSetValueEx(hKey, "zcoef", 0, REG_SZ, (LPBYTE)subKey, 16); 	
	RegCloseKey(hKey);
	// legend
	saveRegColors();
	// colors
	wsprintf(subKey, "Software\\%s\\%s\\colors", m_pszRegistryKey, m_pszProfileName); 	
	if (ERROR_SUCCESS != RegOpenKeyEx(HKEY_CURRENT_USER, subKey, 0, KEY_WRITE, &hKey))	
		RegCreateKey(HKEY_CURRENT_USER, subKey, &hKey); 	
	
	RegSetValueEx(hKey, "background", 0, REG_DWORD, (LPBYTE)&stg._g_set.dw_bkgnd, 4); 		
	RegSetValueEx(hKey, "cell", 0, REG_DWORD, (LPBYTE)&stg._g_set.dw_cell, 4); 		
	RegSetValueEx(hKey, "lfont", 0, REG_DWORD, (LPBYTE)&stg._g_set.cl_lfont, 4); 			
	RegCloseKey(hKey);
	// well
	wsprintf(subKey, "Software\\%s\\%s\\well", m_pszRegistryKey, m_pszProfileName); 	
	if (ERROR_SUCCESS != RegOpenKeyEx(HKEY_CURRENT_USER, subKey, 0, KEY_WRITE, &hKey))	
		RegCreateKey(HKEY_CURRENT_USER, subKey, &hKey); 	
	
	RegSetValueEx(hKey, "ds", 0, REG_BINARY, (LPBYTE)&stg._w_set.ds, 56); 
	RegSetValueEx(hKey, "main", 0, REG_DWORD, (LPBYTE)&stg._w_set.dw_main, 4); 		
	RegSetValueEx(hKey, "sel", 0, REG_DWORD, (LPBYTE)&stg._w_set.dw_sel, 4); 			
	RegSetValueEx(hKey, "wfont", 0, REG_DWORD, (LPBYTE)&stg._w_set.dw_font, 4); 	
	RegSetValueEx(hKey, "wh", 0, REG_DWORD, (LPBYTE)&stg._w_set.wh, 4); 	
	RegSetValueEx(hKey, "wi", 0, REG_DWORD, (LPBYTE)&stg._w_set.wi, 4); 	
	RegSetValueEx(hKey, "wr1", 0, REG_DWORD, (LPBYTE)&stg._w_set.wr1, 4); 
	RegSetValueEx(hKey, "wr2", 0, REG_DWORD, (LPBYTE)&stg._w_set.wr2, 4);	
	// well offset
	TCHAR value[6];
	wsprintf(value, "%d", stg._w_set.wxo); 	
	RegSetValueEx(hKey, "wxo", 0, REG_SZ, (LPBYTE)value, 6); 		
	wsprintf(value, "%d", stg._w_set.wyo); 	
	RegSetValueEx(hKey, "wyo", 0, REG_SZ, (LPBYTE)value, 6); 	
	RegCloseKey(hKey);		
}
BEGIN_MESSAGE_MAP(CtriApp, CWinApp)		
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)		
	ON_COMMAND(ID_HELP_FINDER, OnHelp)
	ON_COMMAND(ID_OPTION, OnChangeOption)	
	ON_COMMAND(ID_STAT, OnStat)			
	ON_UPDATE_COMMAND_UI(ID_STAT, OnUpdateStat)	
END_MESSAGE_MAP()
void CtriApp::OnChangeOption()
{
	highColorSheet sheet(IDS_OPTION);
	sheet.DoModal();
}
void CtriApp::OnFileOpen()
{
	CXFileDlg dlg(1);
		
	if (IDOK == dlg.DoModal()) 
	{
		bread = dlg.m_ofn.Flags & OFN_READONLY;		
		m_pFrame->UpdateWindow();		
		OpenDocumentFile(dlg.m_ofn.lpstrFile);	
		m_pFrame->showStock(1);
	}
}
void CtriApp::OnHelp()
{
	char hPath[MAX_PATH];	
	strcpy_s(hPath, MAX_PATH, m_pszHelpFilePath);
	PathRenameExtension(hPath, ".chm");
	::HtmlHelp(0, hPath, HH_DISPLAY_TOPIC, 0);
}
void CtriApp::OnStat()
{
	statDlg stat;
	stat.DoModal();
}
void CtriApp::OnUpdateStat(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_graph.m_inilines.GetSize() > 0);
}