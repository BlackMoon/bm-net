// frame.cpp
#include "stdafx.h"
#include "frame.h"

extern char					msg[MAX_PATH];
extern SIZE					szParam;		
extern stockline			_stockline;
// CMainFrame
const CRect CMainFrame::s_rcDefault(0, 0, 800, 600);									 	
const TCHAR CMainFrame::s_filter[] = {"Битмап Windows (*.bmp)|*.bmp|Формат JPEG Interchange (*.jpg)|*.jpg|Portatible Network Graphics (*.png)|*.png|"};

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)
CMainFrame::CMainFrame()
{	
	m_bfirst = 1; 	
	m_bsplit = m_bstock = 0;
	m_pcomboP = m_pcomboD = 0;
	m_pstockDlg = 0;
	memset(&m_bpanel, 1, 3);
	pgraph = &app.m_graph;		
}
BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{	
	if (!m_split.CreateStatic(this, 2, 1)) return 0;		// create splitter window
	
	if (!m_split.CreateView(0, 0, RUNTIME_CLASS(C3dView), CSize(200, 100), pContext) ||
		!m_split.CreateView(1, 0, RUNTIME_CLASS(CInfoView), CSize(200, 50), pContext))
	{
		m_split.DestroyWindow();
		return 0;
	}
	m_p3dview = (C3dView*)m_split.GetPane(0, 0);
	m_pinfoview = (CInfoView*)m_split.GetPane(1, 0);
	m_bsplit = 1;	
	
	return 1; // return success
}
BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if (cs.hMenu)
	{
		::DestroyMenu(cs.hMenu);
		cs.hMenu = 0;
	}
	cs.style &= ~FWS_ADDTOTITLE;
	return CFrameWnd::PreCreateWindow(cs);	
}
void CMainFrame::ActivateFrame(int nCmdShow)
{
	RECT rc;
    HKEY hKey;
	DWORD dwType, dwSize;
	TCHAR subKey[_MAX_PATH];
	BOOL bIconic = 0,
		 bMaximized = 0;
    UINT flags;    

    if (m_bfirst) 
	{        
		m_bfirst = 0;
		wsprintf(subKey, "Software\\%s\\%s\\frame", app.m_pszRegistryKey, app.m_pszProfileName); 	
		
		if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_CURRENT_USER, subKey, 0, KEY_READ, &hKey)) 
		{
			dwType = REG_DWORD;
			dwSize = 4;
			RegQueryValueEx(hKey, "icon", 0, &dwType, (LPBYTE)&bIconic, &dwSize);	
			RegQueryValueEx(hKey, "max", 0, &dwType, (LPBYTE)&bMaximized, &dwSize);	

			dwType = REG_SZ;
			dwSize = _MAX_PATH;
			RegQueryValueEx(hKey, "rect", 0, &dwType, (LPBYTE)subKey, &dwSize);  					
			
			sscanf_s(subKey, "%04d %04d %04d %04d", &rc.left, &rc.top, &rc.right, &rc.bottom);
		}
		if (IsRectEmpty(&rc)) rc = s_rcDefault;        
        
        if (bIconic) 
		{
            nCmdShow = SW_SHOWMINNOACTIVE;
            if (bMaximized) flags = WPF_RESTORETOMAXIMIZED;			
            else flags = WPF_SETMINPOSITION;            
        }
        else 
		{
            if (bMaximized) 
			{
                nCmdShow = SW_SHOWMAXIMIZED;
                flags = WPF_RESTORETOMAXIMIZED;
            }
            else 
			{
                nCmdShow = SW_NORMAL;
                flags = WPF_SETMINPOSITION;
            }
        }
		
		WINDOWPLACEMENT wndpl;    
        wndpl.length = 44;
        wndpl.showCmd = nCmdShow;
        wndpl.flags = flags;
		wndpl.ptMinPosition = CPoint(0, 0);
        wndpl.ptMaxPosition = 
			CPoint(-::GetSystemMetrics(SM_CXBORDER), -::GetSystemMetrics(SM_CYBORDER));
        wndpl.rcNormalPosition = rc;        		
        SetWindowPlacement(&wndpl);			
    }
	CFrameWnd::ActivateFrame(nCmdShow);		
}
void CMainFrame::back()
{	
	pgraph->BackView();	
}
void CMainFrame::bottom()
{	
	pgraph->BottomView();
}
void CMainFrame::cell()
{		
	VARIANT_BOOL bcell = pgraph->m_show.bshowCell;
	pgraph->m_show.bshowCell = !bcell;
}
void CMainFrame::comboAddData(LPCTSTR lpszString, LPVOID lpData, bool broxar)
{
	char line[32];
	memset(line, 0, 32);
	strncpy_s(line, 32, lpszString, 31);	

	CComboBox* pcombo = broxar ? m_pcomboD : m_pcomboP;	
	
	UCHAR count = (UCHAR)pcombo->SendMessage(CB_GETCOUNT);		
	pcombo->SendMessage(CB_ADDSTRING, 0, (LPARAM)line);
	
	pcombo->SendMessage(CB_SETITEMDATA, count, (LPARAM)lpData);
	pcombo->SendMessage(CB_SETITEMHEIGHT, count, MAKELONG(ITEM_H, 0)); 
}
void CMainFrame::comboClearData()
{
	m_pcomboP->SendMessage(CB_RESETCONTENT);		
	m_pcomboP->EnableWindow(0);

	m_pcomboD->SendMessage(CB_RESETCONTENT);
	m_pcomboD->EnableWindow(0);
}
void CMainFrame::dxsave(LPCTSTR pDestFile, D3DXIMAGE_FILEFORMAT DestFormat)
{		
	HBITMAP hbm = m_pinfoview->getSnapShot();
	LPDIRECT3DSURFACE9 lpscene = m_p3dview->getScene(hbm);				
	DeleteObject(hbm);			
		
	D3DXSaveSurfaceToFile(pDestFile, DestFormat, lpscene, 0, 0);
	SAFE_RELEASE(lpscene);			
}
void CMainFrame::first()
{	
	m_pcomboD->SendMessage(CB_SETCURSEL, 0, 0);	
	SendMessage(WM_COMMAND, MAKELONG(ID_DATE, CBN_SELCHANGE), (LPARAM)m_pcomboD->m_hWnd);	
}
void CMainFrame::front()
{	
	pgraph->FrontView();
}
void CMainFrame::infoUpdate()
{
	m_pinfoview->InvalidateRect(0);
	m_split.InvalidateRect(0);
}
void CMainFrame::last()
{	
	int count = (int)m_pcomboD->SendMessage(CB_GETCOUNT, 0, 0);		
	m_pcomboD->SendMessage(CB_SETCURSEL, --count, 0);	
	SendMessage(WM_COMMAND, MAKELONG(ID_DATE, CBN_SELCHANGE), (LPARAM)m_pcomboD->m_hWnd);	
}
void CMainFrame::left()
{	
	pgraph->LeftView();
}
void CMainFrame::next()
{	
	int sel = (int)m_pcomboD->SendMessage(CB_GETCURSEL, 0, 0);	
	m_pcomboD->SendMessage(CB_SETCURSEL, ++sel, 0);	
	SendMessage(WM_COMMAND, MAKELONG(ID_DATE, CBN_SELCHANGE), (LPARAM)m_pcomboD->m_hWnd);		
}
void CMainFrame::panel1()
{
	m_bpanel.btop = !m_bpanel.btop;
	ShowControlBar(&m_topbar, m_bpanel.btop, 0);	
	recalcSplit();
}
void CMainFrame::panel2()
{
	m_bpanel.bleft = !m_bpanel.bleft;
	ShowControlBar(&m_leftbar, m_bpanel.bleft, 0);
}
void CMainFrame::panel3()
{
	m_bpanel.binfo = !m_bpanel.binfo;
	ShowControlBar(&m_infobar, m_bpanel.binfo, 0);	
	recalcSplit();
}
void CMainFrame::prev()
{	
	int sel = (int)m_pcomboD->SendMessage(CB_GETCURSEL, 0, 0);	
	m_pcomboD->SendMessage(CB_SETCURSEL, --sel, 0);	
	SendMessage(WM_COMMAND, MAKELONG(ID_DATE, CBN_SELCHANGE), (LPARAM)m_pcomboD->m_hWnd);	
}
void CMainFrame::recalcSplit()
{
	if (m_bsplit)	
	{
		RECT rc;
		GetClientRect(&rc);
	
		USHORT hOffset = 136;
		if (!m_bpanel.btop) hOffset -= 32;
		if (!m_bpanel.binfo) hOffset -= 28;	
			
		m_split.SetRowInfo(0, rc.bottom - rc.top - hOffset, 10);
		m_split.SetRowInfo(1, hOffset, 10);
		m_split.RecalcLayout();
	}		
}
void CMainFrame::reset()
{	
	pgraph->ResetView();
}
void CMainFrame::right()
{	
	pgraph->RightView();
}
void CMainFrame::setParam()
{
	m_pcomboP->SendMessage(CB_INSERTSTRING, 0, (LPARAM)_T("Глубина"));
	m_pcomboP->SendMessage(CB_SETCURSEL, 0, 0);
	m_pcomboP->EnableWindow(1);
	m_pcomboD->SendMessage(CB_SETCURSEL, 0, 0);
}
void CMainFrame::showStock(bool brel)
{
	if (m_pstockDlg)
	{
		int nCmdShow = SW_HIDE;
		if (brel)
		{
			m_bstock = 0;
			m_pstockDlg->reload();			
		}
		else			
			if (m_bstock) nCmdShow = SW_SHOW;
	
		m_pstockDlg->ShowWindow(nCmdShow);	
	}
}
void CMainFrame::top()
{	
	pgraph->TopView();
}
void CMainFrame::well()
{		
	VARIANT_BOOL bwell = pgraph->m_show.bshowWell;
	pgraph->m_show.bshowWell = !bwell;		
}
BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_CBN_SELCHANGE(ID_PARAM, OnParamSelchange)   			
	ON_CBN_SELCHANGE(ID_DATE, OnDataSelchange)   
	ON_COMMAND(ID_BACK_VIEW, back)		
	ON_COMMAND(ID_BOTTOM_VIEW, bottom)			
	ON_COMMAND(ID_EDIT_COPY, OnFileCopy)	
	ON_COMMAND(ID_SAVE, OnFileSave)	
	ON_COMMAND(ID_FIRST, first)
	ON_COMMAND(ID_FRONT_VIEW, front)
	ON_COMMAND(ID_CELL, cell)		
	ON_COMMAND(ID_LAST, last)
	ON_COMMAND(ID_LEFT_VIEW, left)	
	ON_COMMAND(ID_NEXT, next)
	ON_COMMAND(ID_PANEL1, panel1)
	ON_COMMAND(ID_PANEL2, panel2)
	ON_COMMAND(ID_PANEL3, panel3)
	ON_COMMAND(ID_PREV, prev)
	ON_COMMAND(ID_RIGHT_VIEW, right)		
	ON_COMMAND(ID_RESET_VIEW, reset)
	ON_COMMAND(ID_STOCK, OnStock)
	ON_COMMAND(ID_TOP_VIEW, top)			
	ON_COMMAND(ID_WELL, well)			
	ON_MESSAGE(ID_STOCK_CLOSED, OnStockClosed)
	ON_UPDATE_COMMAND_UI(ID_CELL, OnUpdateCell) 	
	ON_UPDATE_COMMAND_UI(ID_FIRST, OnUpdatePrev) 	
	ON_UPDATE_COMMAND_UI(ID_LAST, OnUpdateNext) 	
	ON_UPDATE_COMMAND_UI(ID_NEXT, OnUpdateNext) 	
	ON_UPDATE_COMMAND_UI(ID_PREV, OnUpdatePrev) 	
	ON_UPDATE_COMMAND_UI(ID_STOCK, OnUpdateStock) 	
	ON_UPDATE_COMMAND_UI(ID_WELL, OnUpdateWell) 
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_MEASUREITEM()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()
int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1) return -1;
	// main toolbar
	if (!m_mainbar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | 
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||  
		!m_mainbar.LoadToolBar(IDR_MAINBAR))
	{
		TRACE0("Failed to create mainbar\n");
		return -1;      // fail to create
	}	
	m_mainbar.setTrueColor(IDB_MAIN);
	m_mainbar.setTrueColor(IDB_MAIN);		
	// top toolbar
	if (!m_topbar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) || 
		!m_topbar.LoadToolBar(IDR_TOPBAR))
	{
		TRACE0("Failed to create topbar\n");
		return -1;      // fail to create
	}		
	m_pcomboP = (CODCombo*)m_topbar.InsertControl(RUNTIME_CLASS(CODCombo), _T(""), 
		CRect(0, 0, 200, 200), ID_PARAM, WS_VSCROLL | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | CBS_HASSTRINGS);					
	m_pcomboP->EnableWindow(0);
	
	m_pcomboD = (CODCombo*)m_topbar.InsertControl(RUNTIME_CLASS(CODCombo), _T(""), 
		CRect(0, 0, 140, 140), ID_DATE, WS_VSCROLL | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | CBS_HASSTRINGS);								
	m_pcomboD->EnableWindow(0);

	m_topbar.setTrueColor(IDB_TOP);
	// create infobar
	if (!m_infobar.Create(this))
	{
		TRACE0("Failed to create infobar\n");
		return -1;		// fail to create
	}
	// left toolbar
	if (!m_leftbar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_LEFT
		| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) || 
		!m_leftbar.LoadToolBar(IDR_LEFTBAR))
	{
		TRACE0("Failed to create leftbar\n");
		return -1;      // fail to create
	}	
	m_leftbar.setTrueColor(IDB_LEFT);	
	// xpopup
	if (!m_xpopup.CreatePopupMenu())	
	{
		TRACE0("Failed to create xpopup\n");
		return -1;      // fail to create
	}
	m_xpopup.AppendMenu(MF_CHECKED, ID_PANEL1, 0, _T("Панель параметров"));
	m_xpopup.AppendMenu(0, ID_PANEL2, 1, _T("Панель вида"));
	m_xpopup.AppendMenu(0, ID_PANEL3, 2, _T("Панель фильтра"));		
	m_xpopup.setTrueColor(IDB_XMENU);	
	// xmenu
	if  (!m_xmenu.CreatePopupMenu())			
	{
		TRACE0("Failed to create xmenu\n");
		return -1;      // fail to create
	}
	m_xmenu.AppendMenu(MF_POPUP, (UINT_PTR)m_xpopup.m_hMenu, 0, _T("Панели"));
	m_xmenu.AppendMenu(MF_SEPARATOR);
	m_xmenu.AppendMenu(0, ID_EDIT_COPY, 6, _T("Копировать"));
	m_xmenu.AppendMenu(0, ID_SAVE, 7, _T("Сохранить"));
	m_xmenu.AppendMenu(0, ID_FILE_PRINT, 8, _T("Печать"));
	m_xmenu.AppendMenu(MF_SEPARATOR);
	m_xmenu.AppendMenu(0, ID_OPTION, 9, _T("Настройки"));	
	m_xmenu.setTrueColor(IDB_XMENU);	
	
	return 0; // return success	
}
LRESULT CMainFrame::OnStockClosed(WPARAM, LPARAM)
{
	m_bstock = 0;
	return 0L;
}
void CMainFrame::OnContextMenu(CWnd* pWnd, CPoint point)
{
	RECT rc0, rc1;
	m_p3dview->GetWindowRect(&rc0);
	m_pinfoview->GetWindowRect(&rc1);
	rc0.bottom = rc1.bottom;

	if (PtInRect(&rc0, point)) m_xmenu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
	Default();
}
void CMainFrame::OnDataSelchange()
{
	int cy = szParam.cy;
	szParam.cy = (int)m_pcomboD->SendMessage(CB_GETCURSEL, 0, 0);	  					
	UpdateWindow();			

	if FAILED(pgraph->updateDeviceObject(UG_READ))
	{
		MessageBox(msg, app.m_pszAppName, MB_ICONWARNING);		
		szParam.cy = cy;		
		m_pcomboD->SendMessage(CB_SETCURSEL, cy, 0);	 
	}
}
void CMainFrame::OnDestroy()
{	
    BOOL bIconic, bMaximized;
    WINDOWPLACEMENT wndpl;
    wndpl.length = 44;
    GetWindowPlacement(&wndpl);

    if (wndpl.showCmd == SW_SHOWNORMAL) 
	{
        bIconic = 0;
        bMaximized = 0;
    }
    else if (wndpl.showCmd == SW_SHOWMAXIMIZED) 
	{
        bIconic = 0;
        bMaximized = 1;
    } 
    else if (wndpl.showCmd == SW_SHOWMINIMIZED) 
	{
        bIconic = 1;
        if (wndpl.flags) bMaximized = 1;        
        else bMaximized = 0;        
    }
    // save to registry
	HKEY hKey;
    TCHAR subKey[_MAX_PATH];	
	
	wsprintf(subKey, "Software\\%s\\%s\\frame", app.m_pszRegistryKey, app.m_pszProfileName); 	
	if (ERROR_SUCCESS != RegOpenKeyEx(HKEY_CURRENT_USER, subKey, 0, KEY_WRITE, &hKey))	
		RegCreateKey(HKEY_CURRENT_USER, subKey, &hKey); 	

	RegSetValueEx(hKey, "icon", 0, REG_DWORD, (LPBYTE)&bIconic, 4); 		
	RegSetValueEx(hKey, "max", 0, REG_DWORD, (LPBYTE)&bMaximized, 4); 		
	sprintf_s(subKey, MAX_PATH, "%04d %04d %04d %04d", wndpl.rcNormalPosition.left, 
		wndpl.rcNormalPosition.top, wndpl.rcNormalPosition.right, wndpl.rcNormalPosition.bottom);
	RegSetValueEx(hKey, "rect", 0, REG_SZ, (LPBYTE)subKey, _MAX_PATH); 	
	RegCloseKey(hKey);		

	SAFE_DELETE(m_pcomboP);
	SAFE_DELETE(m_pcomboD);
	SAFE_DELETE(m_pstockDlg);

	CFrameWnd::OnDestroy();	
}
void CMainFrame::OnFileCopy()
{
	BeginWaitCursor();

	HBITMAP hbm, hbm1 = m_pinfoview->getSnapShot();
	LPDIRECT3DSURFACE9 lpscene = m_p3dview->getScene(hbm1);
	DeleteObject(hbm1);

	D3DSURFACE_DESC desc;
	lpscene->GetDesc(&desc);	

	HDC hdc, hdc1;
	lpscene->GetDC(&hdc);
	hdc1 = CreateCompatibleDC(hdc);	
	hbm = CreateCompatibleBitmap(hdc, desc.Width, desc.Height); 
	
	hbm1 = (HBITMAP)SelectObject(hdc1, hbm);	
	BitBlt(hdc1, 0, 0, desc.Width, desc.Height, hdc, 0, 0, SRCCOPY);
	hbm = (HBITMAP)SelectObject(hdc1, hbm1);
		
	DeleteObject(hbm1);			
	DeleteDC(hdc1);
	// clipboard
	if (OpenClipboard())
	{
		EmptyClipboard();			
		SetClipboardData(CF_BITMAP, hbm);
		CloseClipboard();
	}
	DeleteObject(hbm);

	lpscene->ReleaseDC(hdc);
	SAFE_RELEASE(lpscene);
	
	EndWaitCursor();
}
void CMainFrame::OnFileSave()
{
	CFilterDlg dlg(0, "bmp", 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, s_filter, 0);	
	if (IDOK == dlg.DoModal()) 
	{
		BeginWaitCursor();
		dxsave(dlg.m_ofn.lpstrFile, dlg.dxFormat);		
		EndWaitCursor();
	}
}
void CMainFrame::OnParamSelchange()
{
	int cx = szParam.cx;
	szParam.cx = (int)m_pcomboP->SendMessage(CB_GETCURSEL, 0, 0);	  		
	LPVOID pData = (LPVOID)m_pcomboP->SendMessage(CB_GETITEMDATA, szParam.cx, 0);			
	
	if (pData) 
	{		
		szParam.cy = (int)m_pcomboD->SendMessage(CB_GETCURSEL, 0, 0);		
		m_pcomboD->EnableWindow(1);
	}
	else 
	{	
		szParam.cy = -1;							// roxar not available		
		m_pcomboD->EnableWindow(0);
	}				
	UpdateWindow();			
	
	if FAILED(pgraph->updateDeviceObject(UG_READ))
	{
		MessageBox(msg, app.m_pszAppName, MB_ICONWARNING);		
		m_pcomboP->SendMessage(CB_SETCURSEL, cx, 0);	 
		szParam.cx = cx;				
	}
}
void CMainFrame::OnMeasureItem(int nUDCtl, LPMEASUREITEMSTRUCT lpMIS)
{
	HMENU hMenu = AfxGetThreadState()->m_hTrackingMenu;
	CMenu* pMenu = CMenu::FromHandle(hMenu);
	pMenu->MeasureItem(lpMIS);	
}
void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
	CFrameWnd::OnSize(nType, cx, cy);	
	if (nType != SIZE_MINIMIZED) recalcSplit();		
}
void CMainFrame::OnStock()
{
	if (!m_pstockDlg) 
	{		
		m_pstockDlg = new stockDlg();
		m_pstockDlg->Create(IDD_STOCK, this);
		
		RECT rc;
		m_p3dview->GetWindowRect(&rc);		
		m_pstockDlg->setWindow(rc);
	}
	m_bstock = !m_bstock;
	showStock();		
}
void CMainFrame::OnUpdateCell(CCmdUI* pCmdUI)
{		
	pCmdUI->Enable(pgraph->m_binit);
	pCmdUI->SetCheck(pgraph->m_show.bshowCell);
}
void CMainFrame::OnUpdateNext(CCmdUI* pCmdUI)
{		
	BOOL bOn = m_pcomboD->IsWindowEnabled();
	int nsel = (int)m_pcomboD->SendMessage(CB_GETCURSEL, 0, 0);	  		
	bOn &= (++nsel != (int)m_pcomboD->SendMessage(CB_GETCOUNT, 0, 0));
	
	pCmdUI->Enable(bOn);
}
void CMainFrame::OnUpdatePrev(CCmdUI* pCmdUI)
{		
	BOOL bOn = m_pcomboD->IsWindowEnabled();
	int nsel = (int)m_pcomboD->SendMessage(CB_GETCURSEL, 0, 0);	  	
	bOn &= (nsel != 0);
	
	pCmdUI->Enable(bOn);
}
void CMainFrame::OnUpdateStock(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(_stockline.bvalid);
	pCmdUI->SetCheck(m_bstock);
}
void CMainFrame::OnUpdateWell(CCmdUI* pCmdUI)
{
	if (pgraph->m_nwells > 0)
	{
		pCmdUI->Enable(1);
		pCmdUI->SetCheck(pgraph->m_show.bshowWell);
	}
	else pCmdUI->Enable(0);
}