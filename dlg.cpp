// dlg.cpp
#include "stdafx.h"
#include "dlg.h"

UCHAR						npage = 0;
extern char					_log[MAX_PATH], msg[MAX_PATH];
extern UINT					lx, ly, size;
extern SIZE					szParam;
extern SPACE				space;
extern stockline			_stockline;
extern xyzDim				_xyzDim;

extern bool					charFLOAT(UINT nChar);
extern void					resetFilter();

SIZE getDim(const char* line)
{	
	SIZE sz;
	const char* pdest = strstr(line, "-");		
	
	if (pdest) 
	{
		sscanf_s(line, "%u", &sz.cx);
		if (sscanf_s(++pdest, "%u", &sz.cy) != 1) sz.cy = sz.cx;
	}
	else sz.cx = sz.cy = atoi(line);		
	
	sz.cx--;			
	return sz;
}
void fontSave(D3DXFONT_DESC* pds, PLOGFONT plf, bool bsave)
{	
	if (bsave)										// save plf data into pds
	{
		lstrcpy(pds->FaceName, plf->lfFaceName);
		pds->CharSet = plf->lfCharSet;		
		pds->Height = -MulDiv(plf->lfHeight, ly, 72);			
		pds->Italic = plf->lfItalic;
		pds->Quality = plf->lfQuality;
		pds->Weight = plf->lfWeight;
		pds->Width = plf->lfWidth;
		pds->OutputPrecision = plf->lfOutPrecision;
		pds->PitchAndFamily = plf->lfPitchAndFamily;	
	}
	else
	{
		lstrcpy(plf->lfFaceName, pds->FaceName);
		plf->lfCharSet = pds->CharSet;		
		plf->lfHeight = -MulDiv(pds->Height, 72, ly);
		plf->lfItalic = pds->Italic;		
		plf->lfQuality = pds->Quality;
		plf->lfWeight = pds->Weight;
		plf->lfWidth = pds->Width;
		plf->lfOutPrecision = pds->OutputPrecision;
		plf->lfPitchAndFamily = pds->PitchAndFamily;
	}
}
// CFilterDlg
CFilterDlg::CFilterDlg(BOOL bOpenFileDialog, LPCTSTR lpszDefExt, LPCTSTR lpszFileName, 
					   DWORD dwFlags, LPCTSTR lpszFilter, CWnd* pParentWnd) : 
	CFileDialog(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, pParentWnd)
{
	dxFormat = D3DXIFF_BMP; 
}
void CFilterDlg::OnTypeChange()
{		
	TCHAR _ext[5], line[MAX_PATH];
	
	GetParent()->SendMessage(CDM_GETSPEC, (WPARAM)MAX_PATH, (LPARAM)line);	
	if (lstrlen(line) > 0)
	{			
		switch (m_ofn.nFilterIndex)
		{
			case 1:
			{
				lstrcpy(_ext, ".bmp");
				break;
			}
			case 2:
			{
				lstrcpy(_ext, ".jpg");
				break;
			}
			case 3:
			{
				lstrcpy(_ext, ".png");
				break;
			}
		}		
		PathRenameExtension(line, _ext);
		GetParent()->SendMessage(CDM_SETCONTROLTEXT, (WPARAM)0x480, (LPARAM)line);		
	}
	if (m_ofn.nFilterIndex < 3) m_ofn.nFilterIndex--;	
	dxFormat = (D3DXIMAGE_FILEFORMAT)m_ofn.nFilterIndex;					
}
// CXFileDlg
CXFileDlg::CXFileDlg(BOOL bOpenFileDialog, LPCTSTR lpszDefExt, LPCTSTR lpszFileName,
		DWORD dwFlags, LPCTSTR lpszFilter, CWnd* pParentWnd) :
		CFileDialog(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, pParentWnd)
{
}
BOOL CXFileDlg::OnInitDialog()
{
	BOOL bRet = CFileDialog::OnInitDialog();	
	if (bRet) 
	{
		CWnd* pwnd = GetParent();		
		pwnd->SendMessage(CDM_SETCONTROLTEXT, (WPARAM)IDOK, (LPARAM)"OK");
		pwnd->SendMessage(CDM_SETCONTROLTEXT, (WPARAM)0x410, (LPARAM)"создать палитру");
	}
	return bRet;
}
// CXFontDlg
CXFontDlg::CXFontDlg(LPLOGFONT lplfInitial, DWORD dwFlags, CDC* pdcPrinter, CWnd* pParentWnd)
	: CFontDialog(lplfInitial, dwFlags, pdcPrinter, pParentWnd)  
{
}
BOOL CXFontDlg::OnInitDialog()
{
	BOOL bRet = CFontDialog::OnInitDialog();	
	if (bRet) 
	{
		::EnableWindow(GetDlgItem(0x0410)->GetSafeHwnd(), 0);
		::EnableWindow(GetDlgItem (0x0411)->GetSafeHwnd(), 0);		
	}
	return bRet;
}
// CInfoBar
CInfoBar::CInfoBar()
{
	pgraph = &app.m_graph;
}
BOOL CInfoBar::Create(CWnd* pParentWnd)
{
	if (!CDialogBar::Create(pParentWnd, IDD_INFOBAR, CBRS_ALIGN_BOTTOM, AFX_IDW_TOOLBAR)) 
	{	
		TRACE0("Falied to create dialogbar\n");
		return 0;		
	}
	UpdateData(0);
	return 1;	
}
BOOL CInfoBar::PreTranslateMessage(MSG* pMsg)
{
	CSize sz; 
	char line[10];		

	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) 
	{
		try
		{				
			if (pMsg->hwnd == m_editX.m_hWnd) 
			{
				m_editX.GetWindowText(line, 10);	
				// size
				sz = getDim(line);
				if (sz.cx >= sz.cy)
				{
					memset(msg, 0, MAX_PATH);
					strcpy_s(msg, MAX_PATH, "i min > i max");
					throw msg;
				}
				if (sz != _xyzDim.xDim)
				{
					if ((sz.cx >= 0) && (sz.cy <= space.nX - 1)) _xyzDim.xDim = sz;
					else 
					{
						memset(msg, 0, MAX_PATH);
						sprintf_s(msg, MAX_PATH, "i выходит за границы (%u, %u)", 1, space.nX - 1);
						throw msg;
					}
					if FAILED(pgraph->updateDeviceObject())
					{				
						writelog(_log, msg);
						throw msg;
					}
				}
			}			
			if (pMsg->hwnd == m_editY.m_hWnd) 
			{
				m_editY.GetWindowText(line, 10);			
				// size
				sz = getDim(line);
				if (sz.cx > sz.cy)
				{
					memset(msg, 0, MAX_PATH);
					strcpy_s(msg, MAX_PATH, "j min > j max");
					throw msg;
				}
				if (sz != _xyzDim.yDim)
				{
					if ((sz.cx >= 0) && (sz.cy <= space.nY - 1)) _xyzDim.yDim = sz;
					else 
					{
						memset(msg, 0, MAX_PATH);
						sprintf_s(msg, MAX_PATH, "j выходит за границы (%u, %u)", 1, space.nY - 1);
						throw msg;
					}
					if FAILED(pgraph->updateDeviceObject()) throw msg;					
				}
			}			
			if (pMsg->hwnd == m_editZ.m_hWnd) 
			{
				m_editZ.GetWindowText(line, 10);
				// size
				sz = getDim(line);
				if (sz.cx > sz.cy)
				{
					memset(msg, 0, MAX_PATH);
					strcpy_s(msg, MAX_PATH, "k min > k max");
					throw msg;
				}
				if (sz != _xyzDim.zDim)
				{
					if ((sz.cx >= 0) && (sz.cy <= space.nZ)) _xyzDim.zDim = sz;
					else 
					{
						memset(msg, 0, MAX_PATH);
						sprintf_s(msg, MAX_PATH, "k выходит за границы (%u, %u)", 1, space.nZ);
						throw msg;
					}
					if FAILED(pgraph->updateDeviceObject(UG_CREATE | UG_READ)) throw msg;					
				}
			}												
		}
		catch (char*)
		{			
			MessageBox(msg, app.m_pszAppName, MB_ICONWARNING);
		}		
		setFilter();		
	}		
	return 0;
}
void CInfoBar::DoDataExchange(CDataExchange* pDX)
{	
	CDialogBar::DoDataExchange(pDX);		

	DDX_Control(pDX, IDC_PROGRESS, m_progress);	
	DDX_Control(pDX, IDE_XDIM, m_editX);
	DDX_Control(pDX, IDE_YDIM, m_editY);
	DDX_Control(pDX, IDE_ZDIM, m_editZ);
}
void CInfoBar::setFilter()
{	
	char xDim[10], yDim[10], zDim[10];
	
	if (_xyzDim.xDim.cx + 1 == _xyzDim.xDim.cy) sprintf_s(xDim, 10, "%u", _xyzDim.xDim.cx + 1);
	else sprintf_s(xDim, 10, "%u-%u", _xyzDim.xDim.cx + 1, _xyzDim.xDim.cy);

	if (_xyzDim.yDim.cx + 1 == _xyzDim.yDim.cy) sprintf_s(yDim, 10, "%u", _xyzDim.yDim.cx + 1);
	else sprintf_s(yDim, 10, "%u-%u", _xyzDim.yDim.cx + 1, _xyzDim.yDim.cy);
	
	if (_xyzDim.zDim.cx + 1 == _xyzDim.zDim.cy) sprintf_s(zDim, 10, "%u", _xyzDim.zDim.cx + 1);
	else sprintf_s(zDim, 10, "%u-%u", _xyzDim.zDim.cx + 1, _xyzDim.zDim.cy);
	
	m_editX.SetWindowText(xDim);
	m_editY.SetWindowText(yDim);
	m_editZ.SetWindowText(zDim);
}
BEGIN_MESSAGE_MAP(CInfoBar, CDialogBar)
    ON_BN_CLICKED(IDB_RESET, OnClickReset)
	ON_BN_CLICKED(IDB_XSMALL, OnXSmaller)
	ON_BN_CLICKED(IDB_XBIG, OnXBigger)
	ON_BN_CLICKED(IDB_YSMALL, OnYSmaller)
	ON_BN_CLICKED(IDB_YBIG, OnYBigger)
	ON_BN_CLICKED(IDB_ZSMALL, OnZSmaller)
	ON_BN_CLICKED(IDB_ZBIG, OnZBigger)	  
	ON_UPDATE_COMMAND_UI(IDB_RESET, OnUpdateReset)
	ON_UPDATE_COMMAND_UI(IDB_XSMALL, OnUpdateXSmall)
	ON_UPDATE_COMMAND_UI(IDB_XBIG, OnUpdateXBig)
	ON_UPDATE_COMMAND_UI(IDB_YSMALL, OnUpdateYSmall)
	ON_UPDATE_COMMAND_UI(IDB_YBIG, OnUpdateYBig)
	ON_UPDATE_COMMAND_UI(IDB_ZSMALL, OnUpdateZSmall)
	ON_UPDATE_COMMAND_UI(IDB_ZBIG, OnUpdateZBig)
END_MESSAGE_MAP()
void CInfoBar::OnClickReset()
{	
	::resetFilter();
	pgraph->updateDeviceObject(UG_CREATE | UG_READ);
	setFilter();
}
void CInfoBar::OnXSmaller()
{	
	_xyzDim.xDim.cx--;
	_xyzDim.xDim.cy--;
	pgraph->updateDeviceObject();	
	setFilter();
}
void CInfoBar::OnXBigger()
{	
	_xyzDim.xDim.cx++;
	_xyzDim.xDim.cy++;
	pgraph->updateDeviceObject();	
	setFilter();
}
void CInfoBar::OnYSmaller()
{	
	_xyzDim.yDim.cx--;
	_xyzDim.yDim.cy--;
	pgraph->updateDeviceObject();	
	setFilter();
}
void CInfoBar::OnYBigger()
{	
	_xyzDim.yDim.cx++;
	_xyzDim.yDim.cy++;
	pgraph->updateDeviceObject();	
	setFilter();
}
void CInfoBar::OnZSmaller()
{	
	_xyzDim.zDim.cx--;
	_xyzDim.zDim.cy--;
	pgraph->updateDeviceObject(UG_CREATE | UG_READ);	
	setFilter();
}
void CInfoBar::OnZBigger()
{	
	_xyzDim.zDim.cx++;
	_xyzDim.zDim.cy++;
	pgraph->updateDeviceObject(UG_CREATE | UG_READ);	
	setFilter();
}
void CInfoBar::OnUpdateReset(CCmdUI* pCmdUI)
{	
	bool bret = ((_xyzDim.xDim.cx == 0) && (_xyzDim.yDim.cx == 0)
		&& (_xyzDim.zDim.cx == 0) && (_xyzDim.xDim.cy + 1 == space.nX)
		&& (_xyzDim.yDim.cy + 1 == space.nY) && (_xyzDim.zDim.cy == space.nZ));
	pCmdUI->Enable(!bret);	
}
void CInfoBar::OnUpdateXSmall(CCmdUI* pCmdUI)
{	
	pCmdUI->Enable(_xyzDim.xDim.cx != 0);
}
void CInfoBar::OnUpdateXBig(CCmdUI* pCmdUI)
{	
	pCmdUI->Enable(_xyzDim.xDim.cy + 1 != space.nX);
}
void CInfoBar::OnUpdateYSmall(CCmdUI* pCmdUI)
{	
	pCmdUI->Enable(_xyzDim.yDim.cx != 0);
}
void CInfoBar::OnUpdateYBig(CCmdUI* pCmdUI)
{	
	pCmdUI->Enable(_xyzDim.yDim.cy + 1 != space.nY);
}
void CInfoBar::OnUpdateZSmall(CCmdUI* pCmdUI)
{	
	pCmdUI->Enable(_xyzDim.zDim.cx != 0);
}
void CInfoBar::OnUpdateZBig(CCmdUI* pCmdUI)
{	
	pCmdUI->Enable(_xyzDim.zDim.cy != space.nZ);
}
// legendDlg
legendDlg::legendDlg(DWORD* dw, UCHAR size) : CDialog(legendDlg::IDD)
{
	m_size = size;
	m_dw = new DWORD[m_size];
	memcpy(m_dw, dw, m_size << 2);
}
legendDlg::~legendDlg()
{
	SAFE_DELETE_ARRAY(m_dw);
}
BOOL legendDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	m_list.m_dw = m_dw;
	m_list.createMenu();
	m_list.setArr(m_size);		
	
	return 1;
}
void legendDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDL_LEGEND, m_list);
}
BEGIN_MESSAGE_MAP(legendDlg, CDialog)
	ON_WM_DESTROY()
END_MESSAGE_MAP()
void legendDlg::OnDestroy()
{
	m_dw = m_list.m_dw;
	m_size = (int)m_list.SendMessage(LB_GETCOUNT);
	CDialog::OnDestroy();
}
// xPage
xPage::xPage(UINT nIDTemplate, UINT nIDCaption) : CPropertyPage(nIDTemplate, nIDCaption)
{
	parent = 0;
	m_psp.dwFlags |= PSP_USEHICON;
}
BOOL xPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();
	parent = GetParent();
	
	return 1;
}
// generalPage
generalPage::generalPage() : xPage(generalPage::IDD) 
{		
	_c_set = app.stg._c_set;
	_c_set.setArr();
	memcpy(_c_set.colors, app.stg._c_set.colors, app.stg._c_set._size << 2); 
	memcpy(&_g_set, &app.stg._g_set, 20);
}
generalPage::~generalPage()
{
	SAFE_DELETE_ARRAY(_c_set.colors);
}
BOOL generalPage::OnInitDialog()
{
	xPage::OnInitDialog();
	
	BYTE  r, g, b;
	r = (BYTE)((_g_set.dw_bkgnd >> 16) & 0xff),
	g = (BYTE)((_g_set.dw_bkgnd >> 8) & 0xff),
	b = (BYTE)(_g_set.dw_bkgnd & 0xff);	
	m_bkgnd.m_color = RGB(r, g, b);

	r = (BYTE)((_g_set.dw_cell >> 0x10) & 0xff),
	g = (BYTE)((_g_set.dw_cell >> 0x8) & 0xff),
	b = (BYTE)(_g_set.dw_cell & 0xff);
	m_cell.m_color = RGB(r, g, b);
	m_lfont.m_color = _g_set.cl_lfont;	
	
	m_lstatic.setColors(_c_set.colors, _c_set._size);
	m_lstatic.setGray(_c_set.bgray);
	m_lstatic.setInv(_c_set.binv);
	UpdateData(0);
	
	return 1;
}
void generalPage::DoDataExchange(CDataExchange* pDX)
{
	ASSERT(pDX);
	xPage::DoDataExchange(pDX);
	
	DDX_Check(pDX, IDC_AUTO, _g_set.bauto);
	DDX_Check(pDX, IDC_GRAY, _c_set.bgray);
	DDX_Check(pDX, IDC_INV, _c_set.binv);
	
	DDX_Control(pDX, IDB_BKGND, m_bkgnd);
	DDX_Control(pDX, IDB_CELL, m_cell);
	DDX_Control(pDX, IDB_LFONT, m_lfont);
	DDX_Control(pDX, IDS_LEGEND, m_lstatic);

	DDX_Control(pDX, IDE_ZCOEF, m_editZ);
	DDX_Text(pDX, IDE_ZCOEF, _g_set.zcoef);
	DDV_MinMaxFloat(pDX, _g_set.zcoef, 1.0f, 512.0f); 
	
	m_editZ.SetLimitText(6);
}
void generalPage::getData()
{
	BYTE r, g, b;

	r = (BYTE)(m_bkgnd.m_color & 0xff);
	g = (BYTE)((m_bkgnd.m_color >> 0x8) & 0xff);
	b = (BYTE)((m_bkgnd.m_color >> 0x10) & 0xff);	
	_g_set.dw_bkgnd = D3DCOLOR_XRGB(r, g, b);
	
	r = (BYTE)(m_cell.m_color & 0xff);
	g = (BYTE)((m_cell.m_color >> 0x8) & 0xff);
	b = (BYTE)((m_cell.m_color >> 0x10) & 0xff);
	_g_set.dw_cell = D3DCOLOR_XRGB(r, g, b);
	_g_set.cl_lfont = m_lfont.m_color;	
}
BEGIN_MESSAGE_MAP(generalPage, xPage)
	ON_BN_CLICKED(IDC_AUTO, OnClickedAuto)
	ON_BN_CLICKED(IDB_BKGND, OnClickedBkgnd)
	ON_BN_CLICKED(IDB_CELL, OnClickedCell)
	ON_BN_CLICKED(IDC_GRAY, OnClickedGray)
	ON_BN_CLICKED(IDC_INV, OnClickedInv)
	ON_BN_CLICKED(IDB_LEGEND, OnClickedLegend)
	ON_BN_CLICKED(IDB_LFONT, OnClickedLFont)
	ON_EN_CHANGE(IDE_ZCOEF, OnChangeZ)
END_MESSAGE_MAP()
void generalPage::OnChangeZ()
{
	parent->PostMessage(ID_GENERAL_CHANGED, UO_ZCOEF);
}
void generalPage::OnClickedAuto()
{
	parent->PostMessage(ID_GENERAL_CHANGED, UO_GENERAL);
}
void generalPage::OnClickedBkgnd()
{
	parent->PostMessage(ID_INFO_CHANGED);
}
void generalPage::OnClickedCell()
{
	parent->PostMessage(ID_GENERAL_CHANGED, UO_SKEL);
}
void generalPage::OnClickedGray()
{
	UpdateData(1);
	m_lstatic.setGray(_c_set.bgray);	
	m_lstatic.InvalidateRect(0);
	parent->PostMessage(ID_LEGEND_CHANGED);
}
void generalPage::OnClickedInv()
{
	UpdateData(1);	
	m_lstatic.setInv(_c_set.binv);
	m_lstatic.InvalidateRect(0);
	parent->PostMessage(ID_LEGEND_CHANGED);
}
void generalPage::OnClickedLegend()
{
	legendDlg lDlg(_c_set.colors, _c_set._size);	
	
	if (IDOK == lDlg.DoModal())
	{
		if (memcmp(_c_set.colors, lDlg.m_dw, _c_set._size << 2) != 0)
		{			
			_c_set._size = lDlg.m_size;
			_c_set.setArr();
			memcpy(_c_set.colors, lDlg.m_dw, lDlg.m_size << 2);
			m_lstatic.setColors(_c_set.colors, lDlg.m_size);
			m_lstatic.InvalidateRect(0);
			parent->PostMessage(ID_LEGEND_CHANGED);
		}
	}
}
void generalPage::OnClickedLFont()
{
	parent->PostMessage(ID_INFO_CHANGED);
}
// wellPage
wellPage::wellPage() : xPage(wellPage::IDD) 
{	
	_w_set = app.stg._w_set;
}
wellPage::~wellPage()
{	
}
BOOL wellPage::OnInitDialog()
{
	xPage::OnInitDialog();
	
	BYTE  r, g, b;
	r = (BYTE)((_w_set.dw_main >> 0x10) & 0xff),
	g = (BYTE)((_w_set.dw_main >> 0x8) & 0xff),
	b = (BYTE)(_w_set.dw_main & 0xff);	
	m_wmain.m_color = RGB(r, g, b);
	
	r = (BYTE)((_w_set.dw_sel >> 0x10) & 0xff),
	g = (BYTE)((_w_set.dw_sel >> 0x8) & 0xff),
	b = (BYTE)(_w_set.dw_sel & 0xff);
	m_wsel.m_color = RGB(r, g, b);

	r = (BYTE)((_w_set.dw_font >> 0x10) & 0xff),
	g = (BYTE)((_w_set.dw_font >> 0x8) & 0xff),
	b = (BYTE)(_w_set.dw_font & 0xff);
	m_wfont.m_color = RGB(r, g, b);
	
	fontSave(&_w_set.ds, &m_wfont.m_lf, 0);	
	
	m_editH.SetLimitText(4);
	m_editR1.SetLimitText(4);
	m_editR2.SetLimitText(4);
	m_editXO.SetLimitText(5);
	m_editYO.SetLimitText(5);

	return 1;
}
void wellPage::DoDataExchange(CDataExchange* pDX)
{	
	xPage::DoDataExchange(pDX);	

	DDX_Control(pDX, IDB_WFONT, m_wfont);
	DDX_Control(pDX, IDB_WMAIN, m_wmain);
	DDX_Control(pDX, IDB_WSEL, m_wsel);
	
	DDX_Control(pDX, IDE_H, m_editH);			
	DDX_Control(pDX, IDE_I, m_editI);			
	DDX_Control(pDX, IDE_R1, m_editR1);
	DDX_Control(pDX, IDE_R2, m_editR2);
	DDX_Control(pDX, IDE_XO, m_editXO);
	DDX_Control(pDX, IDE_YO, m_editYO);	
	
	DDX_Text(pDX, IDE_H, _w_set.wh);		
	DDX_Text(pDX, IDE_I, _w_set.wi);		
	DDX_Text(pDX, IDE_R1, _w_set.wr1);	
	DDX_Text(pDX, IDE_R2, _w_set.wr2);	
	DDX_Text(pDX, IDE_XO, _w_set.wxo);	
	DDX_Text(pDX, IDE_YO, _w_set.wyo);	

	DDV_MinMaxInt(pDX, _w_set.wh, 1, 1024); 
	DDV_MinMaxInt(pDX, _w_set.wi, 0, 128); 
	DDV_MinMaxInt(pDX, _w_set.wr1, 1, 1024); 
	DDV_MinMaxInt(pDX, _w_set.wr2, 1, 1024); 
	DDV_MinMaxInt(pDX, _w_set.wxo, -0xff, 0xff); 
	DDV_MinMaxInt(pDX, _w_set.wyo, -0xff, 0xff); 
}
void wellPage::getData()
{	
	BYTE r, g, b;

	r = GetRValue(m_wmain.m_color);
	g = GetGValue(m_wmain.m_color);
	b = GetBValue(m_wmain.m_color);
	_w_set.dw_main = D3DCOLOR_XRGB(r, g, b);
	
	r = GetRValue(m_wsel.m_color);
	g = GetGValue(m_wsel.m_color);
	b = GetBValue(m_wsel.m_color);
	_w_set.dw_sel = D3DCOLOR_XRGB(r, g, b);
	
	r = GetRValue(m_wfont.m_color);
	g = GetGValue(m_wfont.m_color);
	b = GetBValue(m_wfont.m_color);
	_w_set.dw_font = D3DCOLOR_XRGB(r, g, b);

	fontSave(&_w_set.ds, &m_wfont.m_lf, 1);		
}
BEGIN_MESSAGE_MAP(wellPage, CPropertyPage)
	ON_BN_CLICKED(IDB_WFONT, OnClickedWFont)
	ON_BN_CLICKED(IDB_WMAIN, OnClickedWMain)
	ON_BN_CLICKED(IDB_WSEL, OnClickedWSel)
	ON_EN_CHANGE(IDE_H, OnChangeWell)
	ON_EN_CHANGE(IDE_I, OnChangeWell)
	ON_EN_CHANGE(IDE_R1, OnChangeWell)
	ON_EN_CHANGE(IDE_R2, OnChangeWell)
	ON_EN_CHANGE(IDE_XO, OnChangeWell)
	ON_EN_CHANGE(IDE_YO, OnChangeWell)
END_MESSAGE_MAP()
void wellPage::OnChangeWell()
{
	parent->PostMessage(ID_WELL_CHANGED, UO_WELL);
}
void wellPage::OnClickedWFont()
{
	if (m_wfont.m_bchanged) parent->PostMessage(ID_WELL_CHANGED, UO_WFONT);
}
void wellPage::OnClickedWMain()
{
	if (m_wmain.m_bchanged) OnChangeWell();
}
void wellPage::OnClickedWSel()
{
	if (m_wsel.m_bchanged) OnChangeWell();
}	
// highColorSheet
highColorSheet::highColorSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
: CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{	
	m_psh.dwFlags |= PSH_NOAPPLYNOW;	
	m_psh.nStartPage = npage;
	
	m_gpage.Construct(IDD_GENERAL);	
	AddPage(&m_gpage);		
	m_wpage.Construct(IDD_WELL);	
	AddPage(&m_wpage);	
	
	u_opt = UO_NONE;
}
BOOL highColorSheet::OnInitDialog()
{	
	CPropertySheet::OnInitDialog();		
	// sheet	
	RECT r0, r1;
	GetWindowRect(&r0);	
	r0.right -= 40;    
	r0.bottom += 20;
	MoveWindow(&r0);
	// tabctrl
	CTabCtrl* pTab = GetTabControl();
    pTab->GetWindowRect(&r0);
    ScreenToClient(&r0);
    r0.right -= 40;    
    pTab->MoveWindow(&r0);
	// page
	CPropertyPage* pPage = STATIC_DOWNCAST(CPropertyPage,
			CWnd::FromHandle((HWND)::SendMessage(m_hWnd, PSM_GETCURRENTPAGEHWND, 0, 0)));
    pPage->GetWindowRect(&r0);
    ScreenToClient(&r0);
    r0.right -= 40;    
    pPage->MoveWindow(&r0);
	// buttons
	CWnd *pcancel = GetDlgItem(IDCANCEL),
		 *pok = GetDlgItem(IDOK);	
	
	pok->GetWindowRect(&r0);
	pcancel->GetWindowRect(&r1);
	ScreenToClient(&r0);
	ScreenToClient(&r1);
	OffsetRect(&r0, -40, 20);	
	OffsetRect(&r1, -40, 20);	

	pcancel->MoveWindow(&r1);
	pok->MoveWindow(&r0);


	m_tstatic.SetFont(GetFont());	
	
	return 0;	
}
INT_PTR highColorSheet::DoModal()
{
	INT_PTR nres = CPropertySheet::DoModal();
	if (nres == IDOK) 
	{	// general & info & skel & zcoef	
		if (u_opt & UO_GENERAL)
		{
			m_gpage.getData();
			app.stg._g_set = m_gpage._g_set;			
		}	
		if (u_opt & UO_INFO) 
		{
			m_gpage.getData();
			app.stg._g_set = m_gpage._g_set;			
			app.m_pFrame->infoUpdate();
		}
		if (u_opt & UO_SKEL) 
		{
			m_gpage.getData();
			app.stg._g_set = m_gpage._g_set;			
			app.m_graph.updateDeviceObject(UG_SKEL);			
		}
		if (u_opt & UO_ZCOEF) 
		{
			m_gpage.getData();
			app.stg._g_set = m_gpage._g_set;			
			app.m_graph.updateDeviceWell(0);			
			app.m_graph.updateFonts();
			app.m_graph.updateDeviceObject(UG_SKEL | UG_ZCOEF);			
		}					
		if (u_opt & UO_LEGEND)
		{
			app.stg._c_set = m_gpage._c_set;
			app.stg._c_set.setArr();
			memcpy(app.stg._c_set.colors, m_gpage._c_set.colors, m_gpage._c_set._size << 2);				
			app.saveRegColors();
		}
		// well
		if (u_opt & UO_WELL) 
		{
			m_wpage.getData();		
			app.stg._w_set = m_wpage._w_set;						
			app.m_graph.updateDeviceWell(0);			
			app.m_graph.updateFonts();
		}
		if (u_opt & UO_WFONT) 
		{
			m_wpage.getData();		
			app.stg._w_set = m_wpage._w_set;			
			app.m_pFrame->m_p3dview->updateFont(UF_CREATE | UF_RELEASE);
			app.m_graph.updateFonts();
		}			
	}
	return nres;
}
BEGIN_MESSAGE_MAP(highColorSheet, CPropertySheet)
	ON_MESSAGE(ID_GENERAL_CHANGED, OnGeneralChanged)	
	ON_MESSAGE(ID_LEGEND_CHANGED, OnLegendChanged)	
	ON_MESSAGE(ID_INFO_CHANGED, OnInfoChanged)	
	ON_MESSAGE(ID_WELL_CHANGED, OnWellChanged)		
	ON_WM_CREATE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()
int highColorSheet::OnCreate(LPCREATESTRUCT lpCreateStruct)
{	
	if (CPropertySheet::OnCreate(lpCreateStruct) == -1) return -1;		
	// icon static
	if (!m_istatic.Create(_T(""), WS_CHILD | SS_ICON | SS_CENTERIMAGE, 
		CRect(12, 350, 28, 366), this))
	{
		TRACE0("Failed to create istatic\n");
		return -1;
	}		    
	HICON hIcon = (HICON)::LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_OPTIONS), IMAGE_ICON, 16, 16, LR_VGACOLOR);
	SendMessage(WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
	hIcon = (HICON)::LoadImage(afxCurrentInstanceHandle, MAKEINTRESOURCE(IDI_EXCLAIM), IMAGE_ICON, 16, 16, LR_VGACOLOR);	
	m_istatic.SendMessage(STM_SETICON, (WPARAM)hIcon);	
	// text static
	if (!m_tstatic.Create(_T("Требуется создать палитру заново"), WS_CHILD | SS_LEFT, 
		CRect(36, 352, 330, 368), this))
	{
		TRACE0("Failed to create tstatic\n");
		return -1;
	}		
	return 0;
}
void highColorSheet::OnDestroy()
{
	npage = GetActiveIndex();
	CPropertySheet::OnDestroy();
}
LRESULT highColorSheet::OnGeneralChanged(WPARAM wParam, LPARAM)
{		
	u_opt |= wParam;	
	return 1;
}
LRESULT highColorSheet::OnInfoChanged(WPARAM wParam, LPARAM)
{	
	u_opt |= UO_INFO;
	return 1;
}
LRESULT highColorSheet::OnLegendChanged(WPARAM wParam, LPARAM)
{		
	m_istatic.ShowWindow(SW_SHOW);		
	m_tstatic.ShowWindow(SW_SHOW);
	u_opt |= UO_LEGEND;
	return 1;
}
LRESULT highColorSheet::OnWellChanged(WPARAM wParam, LPARAM)
{
	u_opt |= wParam;
	return 1;
}
// toolDlg
toolDlg::toolDlg(UINT nIDTemplate, CWnd* pParentWnd) : CDialog(nIDTemplate, pParentWnd)
{	
}
toolDlg::~toolDlg()
{
}
bool toolDlg::getDims()
{
	bool bres = 1;

	CSize csz; 
	CString line;	
	try
	{
		m_editX.GetWindowText(line);
		if (line.Right(1) == "-") m_editX.PostMessage(WM_KEYDOWN, VK_BACK);				
		// size
		csz = getDim(line);
		if (csz.cx > csz.cy)
		{			
			memset(msg, 0, MAX_PATH);
			strcpy_s(msg, MAX_PATH, "i min > i max");				
			throw msg;
		}
		if (csz != toolDim.xDim)
		{
			if ((csz.cx >= 0) && (csz.cy <= space.nX - 1)) toolDim.xDim = csz;
			else 
			{				
				memset(msg, 0, MAX_PATH);
				sprintf_s(msg, MAX_PATH, "i выходит за границы (%u, %u)", 1, space.nX - 1);
				throw msg;
			}
		}
		m_editY.GetWindowText(line);
		if (line.Right(1) == "-") m_editZ.PostMessage(WM_KEYDOWN, VK_BACK);				
		// size
		csz = getDim(line);
		if (csz.cx > csz.cy)
		{			
			memset(msg, 0, MAX_PATH);
			strcpy_s(msg, MAX_PATH, "j min > j max");				
			throw msg;
		}
		if (csz != toolDim.yDim)
		{
			if ((csz.cx >= 0) && (csz.cy <= space.nY - 1)) toolDim.yDim = csz;
			else 
			{				
				memset(msg, 0, MAX_PATH);
				sprintf_s(msg, MAX_PATH, "j выходит за границы (%u, %u)", 1, space.nY - 1);
				throw msg;
			}
		}
		m_editZ.GetWindowText(line);
		if (line.Right(1) == "-") m_editZ.PostMessage(WM_KEYDOWN, VK_BACK);				
		// size
		csz = getDim(line);
		if (csz.cx > csz.cy)
		{			
			memset(msg, 0, MAX_PATH);
			strcpy_s(msg, MAX_PATH, "k min > k max");				
			throw msg;
		}
		if (csz != toolDim.zDim)
		{
			if ((csz.cx >= 0) && (csz.cy <= space.nZ)) toolDim.zDim = csz;
			else 
			{				
				memset(msg, 0, MAX_PATH);
				sprintf_s(msg, MAX_PATH, "k выходит за границы (%u, %u)", 1, space.nZ);
				throw msg;
			}
		}
	}
	catch (char*)
	{
		bres = 0; 
	}
	return bres;
}
BOOL toolDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	resetFilter();
	setFilter();	
	return 1;
}
void toolDlg::CopyCell()
{
	long col1, col2, row1, row2; 
	m_vsflex.GetSelection(&row1, &col1, &row2, &col2);	
	
	COleVariant vclip = m_vsflex.get_Cell(0, COleVariant(0L), COleVariant(col1),
                                             COleVariant(0L), COleVariant(col2));		
	
	CString sclip = m_vsflex.get_Clip();
	sclip.Insert(0, "\n");		
	sclip.Insert(0, (CString)vclip.bstrVal);	
	// clipboard
	if (OpenClipboard())
	{
		EmptyClipboard();

		HGLOBAL hglobal = GlobalAlloc(GMEM_MOVEABLE, sclip.GetLength() + 1);	 
		if (hglobal)
		{
			LPSTR lptext = (LPSTR)GlobalLock(hglobal);
			lstrcpy(lptext, sclip.GetBuffer(0));
			GlobalUnlock(hglobal);
			SetClipboardData(CF_TEXT, hglobal);
		}	
		CloseClipboard();
	}
}
void toolDlg::flexReload()
{
	m_vsflex.put_Rows(2);
	m_vsflex.Clear(COleVariant(1L), COleVariant(0L));
}
void toolDlg::resetFilter()
{
	memset(&toolDim, 0, 24);	
	toolDim.xDim.cy = space.nX - 1;
	toolDim.yDim.cy = space.nY - 1;
	toolDim.zDim.cy = space.nZ;	
}
void toolDlg::SelAll()
{
	m_vsflex.SetFocus();
	m_vsflex.Select(1L, 0L, COleVariant(m_rows - 1L), COleVariant(m_cols - 1L));
}
void toolDlg::setFilter()
{
	char xDim[10], yDim[10], zDim[10];
	
	if (toolDim.xDim.cx + 1 == toolDim.xDim.cy) sprintf_s(xDim, 10, "%u", toolDim.xDim.cx + 1);
	else sprintf_s(xDim, 10, "%u-%u", toolDim.xDim.cx + 1, toolDim.xDim.cy);

	if (toolDim.yDim.cx + 1 == toolDim.yDim.cy) sprintf_s(yDim, 10, "%u", toolDim.yDim.cx + 1);
	else sprintf_s(yDim, 10, "%u-%u", toolDim.yDim.cx + 1, toolDim.yDim.cy);
	
	if (toolDim.zDim.cx + 1 == toolDim.zDim.cy) sprintf_s(zDim, 10, "%u", toolDim.zDim.cx + 1);
	else sprintf_s(zDim, 10, "%u-%u", toolDim.zDim.cx + 1, toolDim.zDim.cy);
	
	m_editX.SetWindowText(xDim);
	m_editY.SetWindowText(yDim);
	m_editZ.SetWindowText(zDim);
}
BEGIN_MESSAGE_MAP(toolDlg, CDialog)
	ON_WM_CREATE()
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(toolDlg, CDialog)	
	ON_EVENT(toolDlg, IDX_VSFLEX, DISPID_MOUSEUP, OnMouseUpFlex, VTS_I2 VTS_I2 VTS_R4 VTS_R4)	
END_EVENTSINK_MAP()
int toolDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1) return -1;

	m_xmenu.CreatePopupMenu();				
	m_xmenu.itemWidth = 100;
	m_xmenu.setTrueColor(IDB_XMENU);

	return 0; 
}
void toolDlg::OnMouseUpFlex(short Button, short Shift, float X, float Y)
{	
	if (Button == 2) 
	{		
		RECT rc;
		m_vsflex.GetWindowRect(&rc);
		ScreenToClient(&rc);

		POINT pos;
		pos.x = (int)(X * lx / 1440); 
		pos.y = (int)(Y * ly / 1440);
		ClientToScreen(&pos);
		pos.x += rc.left;
		pos.y += rc.top;

		m_xmenu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pos.x, pos.y, this);
	}
}
// statDlg
statDlg::statDlg() : toolDlg(statDlg::IDD)
{		
	bactive = bdiv = bread = 0;		
	m_cy = m_row = sz.cx = 0;	
	sz.cy = -1;	
	// read actn
	m_total = (space.nX - 1) * (space.nY - 1) * space.nZ;
	pbarr = new bool[m_total];
	memset(pbarr, 1, m_total);
	
	FILE* stream; 
	fopen_s(&stream, app.m_graph._actn, "rb");
	if (stream) 
	{			
		fread(pbarr, 1, m_total, stream);		
		fclose(stream);
		bread = 1;
	}			
	m_fstep = 0.0f;		
}
statDlg::~statDlg()
{		
	SAFE_DELETE_ARRAY(pbarr);
}
bool statDlg::analise(const char* filename)
{	
	bool bres = 1; 		 	

	BeginWaitCursor();
	try
	{
		FILE *stream;
		fopen_s(&stream, filename, "rb");
		if (!stream) throw 0;	

		float fvalue;		
		int i, iz, jy, kx,
		    offset, dsize = (int)stats.size();			 	     
		stat _stat;	
		
		float* pfarr = new float[size];
				
		if (bactive)								// active cells
		{			
			for (iz = toolDim.zDim.cx; iz < toolDim.zDim.cy; iz++)
			{
				offset = iz * size;
				fseek(stream, ++offset << 3, SEEK_SET);				
				if (fread(pfarr, 4, size, stream) != size) throw 0;

				for (jy = toolDim.yDim.cx; jy < toolDim.yDim.cy; jy++)
				{							
					for (kx = toolDim.xDim.cx; kx < toolDim.xDim.cy; kx++)
					{
						fvalue = pfarr[jy * space.nX + kx];				

						i = (space.nX - 1) * (iz * (space.nY - 1) + jy) + kx;
						if (!pbarr[i]) continue;
								
						for (i = 0; i < dsize; i++)
						{
							// fisrt value - total min
							if ((fvalue >= m_fmin - FLT_EPSILON) && (fvalue <= m_fmin + FLT_EPSILON))
							{
								stats[0].count++;
								m_faver += fvalue;
								m_total++;
								break;
							}
							_stat = stats[i];
							if ((fvalue > _stat.x0) && (fvalue <= _stat.x1 + FLT_EPSILON)) 
							{
								stats[i].count++;
								m_faver += fvalue;
								m_total++;
								break;
							}
						}
					}				
				}				
			}				
			m_progr.StepIt();								
		}
		else
		{			
			for (iz = toolDim.zDim.cx; iz < toolDim.zDim.cy; iz++)
			{
				offset = iz * size;
				fseek(stream, ++offset << 3, SEEK_SET);				
				if (fread(pfarr, 4, size, stream) != size) throw 0;

				for (jy = toolDim.yDim.cx; jy < toolDim.yDim.cy; jy++)
				{							
					for (kx = toolDim.xDim.cx; kx < toolDim.xDim.cy; kx++)
					{
						fvalue = pfarr[jy * space.nX + kx];		
						
						for (i = 0; i < dsize; i++)
						{									
							// fisrt value - total min
							if ((fvalue >= m_fmin - FLT_EPSILON) && (fvalue <= m_fmin + FLT_EPSILON))
							{
								stats[0].count++;
								m_faver += fvalue;
								m_total++;
								break;
							}
							_stat = stats[i];
							if ((fvalue > _stat.x0) && (fvalue <= _stat.x1 + FLT_EPSILON)) 
							{
								stats[i].count++;
								m_faver += fvalue;
								m_total++;
								break;
							}
						}							
					}				
				}			
				m_progr.StepIt();			
			}					
		}				
		SAFE_DELETE_ARRAY(pfarr);	
		fclose(stream);
	}
	catch (int)
	{		
		memset(msg, 0, MAX_PATH);
		sprintf_s(msg, MAX_PATH, "Не удалось открыть %s.", filename);		
		bres = 0;		
	}
	EndWaitCursor();
	return bres;
}
bool statDlg::analiseDiv(const char* filename)
{
	bool bres = 1; 		 	

	BeginWaitCursor();
	try
	{
		FILE *stream0, *stream1;
		fopen_s(&stream0, filename, "rb"),
		fopen_s(&stream1, filename, "rb");

		if (!stream0) throw 0;

		float fvalue0, fvalue1;		
		int i, iz, jy, kx, 
		    offset0, offset1,
		    dsize = (int)stats.size();			 	
		stat _stat;							

		float *pfarr0 = new float[size],
		      *pfarr1 = new float[size];

		if (bactive)								// active cells
		{			
			for (iz = toolDim.zDim.cx; iz < toolDim.zDim.cy; iz++)
			{
				offset0 = (2 + iz * size + sz.cy * (space.cube() + 1)) << 1;
				fseek(stream0, offset0 << 2, SEEK_SET);	
				if (fread(pfarr0, 4, size, stream0) != size) throw 0;
				
				offset1 = (2 + iz * size + m_cy * (space.cube() + 1)) << 1;
				fseek(stream1, offset1 << 2, SEEK_SET);	
				if (fread(pfarr1, 4, size, stream1) != size) throw 1;

				for (jy = toolDim.yDim.cx; jy < toolDim.yDim.cy; jy++)
				{							
					for (kx = toolDim.xDim.cx; kx < toolDim.xDim.cy; kx++)
					{
						fvalue0 = pfarr0[jy * space.nX + kx];
						fvalue1 = pfarr1[jy * space.nX + kx];					

						i = (space.nX - 1) * (iz * (space.nY - 1) + jy) + kx;
						if (!pbarr[i]) continue;

						fvalue0 = (fvalue1 != 0.0f) ? fvalue0 / fvalue1 : 1.0f;
								
						for (i = 0; i < dsize; i++)
						{
							// fisrt value - total min
							if ((fvalue0 >= m_fmin - FLT_EPSILON) && (fvalue0 <= m_fmin + FLT_EPSILON))
							{
								stats[0].count++;
								m_total++;
								m_faver += fvalue0;
								break;
							}
							_stat = stats[i];
							if ((fvalue0 > _stat.x0) && (fvalue0 <= _stat.x1 + FLT_EPSILON)) 
							{
								stats[i].count++;
								m_faver += fvalue0;
								m_total++;
								break;
							}
						}
					}								
				}
				m_progr.StepIt();		
			}													
		}
		else
		{			
			for (iz = toolDim.zDim.cx; iz < toolDim.zDim.cy; iz++)
			{
				offset0 = (2 + iz * size + sz.cy * (space.cube() + 1)) << 1;
				fseek(stream0, offset0 << 2, SEEK_SET);	
				if (fread(pfarr0, 4, size, stream0) != size) throw 0;
				
				offset1 = (2 + iz * size + m_cy * (space.cube() + 1)) << 1;
				fseek(stream1, offset1 << 2, SEEK_SET);	
				if (fread(pfarr1, 4, size, stream1) != size) throw 1;

				for (jy = toolDim.yDim.cx; jy < toolDim.yDim.cy; jy++)
				{							
					for (kx = toolDim.xDim.cx; kx < toolDim.xDim.cy; kx++)
					{
						fvalue0 = pfarr0[jy * space.nX + kx];
						fvalue1 = pfarr1[jy * space.nX + kx];					

						fvalue0 = (fvalue1 != 0.0f) ? fvalue0 / fvalue1 : 1.0f;
						
						for (i = 0; i < dsize; i++)
						{									
							// fisrt value - total min
							if ((fvalue0 >= m_fmin - FLT_EPSILON) && (fvalue0 <= m_fmin + FLT_EPSILON))
							{
								stats[0].count++;
								m_faver += fvalue0;
								m_total++;
								break;
							}
							_stat = stats[i];
							if ((fvalue0 > _stat.x0) && (fvalue0 <= _stat.x1 + FLT_EPSILON)) 
							{
								stats[i].count++;
								m_faver += fvalue0;
								m_total++;
								break;
							}
						}				
					}
				}			
				m_progr.StepIt();			
			}					
		}
		SAFE_DELETE_ARRAY(pfarr0);
	    SAFE_DELETE_ARRAY(pfarr1);

		fclose(stream0);
	    fclose(stream1);
	}
	catch (int)
	{
		memset(msg, 0, MAX_PATH);
        sprintf_s(msg, MAX_PATH, "Не удалось открыть %s.", filename);		
		bres = 0;		
	}
	EndWaitCursor();
	return bres;
}
bool statDlg::analiseRox(const char* filename)
{
	bool bres = 1; 		 	

	BeginWaitCursor();
	try
	{
		FILE *stream; 
		fopen_s(&stream, filename, "rb");	     
	    if (!stream) throw 0;

		float fvalue;
		int i, iz, jy, kx, offset, 
		    dsize = (int)stats.size();			 	
		stat _stat;							

		float *pfarr = new float[size];		  

		if (bactive)								// active cells
		{			
			for (iz = toolDim.zDim.cx; iz < toolDim.zDim.cy; iz++)
			{
				offset = (2 + iz * size + sz.cy * (space.cube() + 1)) << 1;
				fseek(stream, offset << 2, SEEK_SET);	
				if (fread(pfarr, 4, size, stream) != size) throw 0;				

				for (jy = toolDim.yDim.cx; jy < toolDim.yDim.cy; jy++)
				{							
					for (kx = toolDim.xDim.cx; kx < toolDim.xDim.cy; kx++)
					{
						fvalue = pfarr[jy * space.nX + kx];						

						i = (space.nX - 1) * (iz * (space.nY - 1) + jy) + kx;
						if (!pbarr[i]) continue;						
								
						for (i = 0; i < dsize; i++)
						{
							// fisrt value - total min
							if ((fvalue >= m_fmin - FLT_EPSILON) && (fvalue <= m_fmin + FLT_EPSILON))
							{
								stats[0].count++;
								m_faver += fvalue;
								m_total++;
								break;
							}
							_stat = stats[i];
							if ((fvalue > _stat.x0) && (fvalue <= _stat.x1 + FLT_EPSILON)) 
							{
								stats[i].count++;
								m_faver += fvalue;
								m_total++;
								break;
							}
						}
					}								
				}
				m_progr.StepIt();		
			}													
		}
		else
		{			
			for (iz = toolDim.zDim.cx; iz < toolDim.zDim.cy; iz++)
			{
				offset = (2 + iz * size + sz.cy * (space.cube() + 1)) << 1;
				fseek(stream, offset << 2, SEEK_SET);	
				if (fread(pfarr, 4, size, stream) != size) throw 0;				

				for (jy = toolDim.yDim.cx; jy < toolDim.yDim.cy; jy++)
				{							
					for (kx = toolDim.xDim.cx; kx < toolDim.xDim.cy; kx++)
					{
						fvalue = pfarr[jy * space.nX + kx];				
						
						for (i = 0; i < dsize; i++)
						{									
							// fisrt value - total min
							if ((fvalue >= m_fmin - FLT_EPSILON) && (fvalue <= m_fmin + FLT_EPSILON))
							{
								stats[0].count++;
								m_faver += fvalue;
								m_total++;
								break;
							}
							_stat = stats[i];
							if ((fvalue > _stat.x0) && (fvalue <= _stat.x1 + FLT_EPSILON)) 
							{
								stats[i].count++;
								m_faver += fvalue;
								m_total++;
								break;
							}
						}				
					}
				}			
				m_progr.StepIt();			
			}					
		}
		SAFE_DELETE_ARRAY(pfarr);	

	    fclose(stream);	
	}
	catch (int)
	{
		memset(msg, 0, MAX_PATH);
		sprintf_s(msg, MAX_PATH, "Не удалось открыть %s.", filename);		
		bres = 0;		
	}
	EndWaitCursor();
	return bres;
}
BOOL statDlg::OnInitDialog()
{
	toolDlg::OnInitDialog();

	CComboBox *pcomboP = app.m_pFrame->m_pcomboP, 
		      *pcomboD = app.m_pFrame->m_pcomboD;
	
	TCHAR line[32];
	int i,
	    size0 = (int)pcomboP->SendMessage(CB_GETCOUNT),
	    size1 = (int)pcomboD->SendMessage(CB_GETCOUNT);
	LPVOID lpData;	
	// params	
	for (i = 1; i < size0; i++)
	{
		memset(line, 0, 32);
		pcomboP->SendMessage(CB_GETLBTEXT, i, (LPARAM)line);
		lpData = (LPVOID)pcomboP->SendMessage(CB_GETITEMDATA, i, 0);
		
		m_comboP.SendMessage(CB_ADDSTRING, 0, (LPARAM)line);		
		m_comboP.SendMessage(CB_SETITEMDATA, i - 1, (LPARAM)lpData);				
	}	
	// dates
	for (i = 0; i < size1; i++)
	{
		memset(line, 0, 32);
		pcomboD->SendMessage(CB_GETLBTEXT, i, (LPARAM)line);
		
		m_comboD0.SendMessage(CB_ADDSTRING, 0, (LPARAM)line);		
		m_comboD1.SendMessage(CB_ADDSTRING, 0, (LPARAM)line);						
	}		
	m_comboP.SendMessage(CB_SETCURSEL, 0, 0);	
	m_comboD0.SendMessage(CB_SETCURSEL, 0, 0);
	m_comboD1.SendMessage(CB_SETCURSEL, 0, 0);		

	bool bdata = ((LPVOID)m_comboP.SendMessage(CB_GETITEMDATA, 0, 0) != 0);									
	sz.cy = bdata ? (int)m_comboP.SendMessage(CB_GETCURSEL, 0, 0) : - 1;			

	minmax();	
	fillNullRow();	
	m_progr.SetRange(0, space.nZ); 
	UpdateData(0);
	
	m_comboD0.EnableWindow(bdata);
	GetDlgItem(IDC_ACTIVE)->EnableWindow(bread);		
	GetDlgItem(IDC_DIV)->EnableWindow(bdata);		
	
	return 1;
}
void statDlg::AddRow()
{		
	m_vsflex.AddItem(m_vsflex.get_TextMatrix(m_row, 1), COleVariant(m_row + 1L));		
	m_vsflex.Select(m_row + 1L, 1L, COleVariant(m_row + 1L), COleVariant(1L));
}
void statDlg::RemoveRow()
{
	m_vsflex.RemoveItem(COleVariant(m_row));
}
void statDlg::DoDataExchange(CDataExchange* pDX)
{
	toolDlg::DoDataExchange(pDX);	
	
	DDX_Control(pDX, IDC_PARAM, m_comboP);	
	DDX_Control(pDX, IDC_DATE0, m_comboD0);	
	DDX_Control(pDX, IDC_DATE1, m_comboD1);	

	DDX_Control(pDX, IDC_PROGR, m_progr);
	DDX_Control(pDX, IDX_VSFLEX, m_vsflex);
	DDX_Control(pDX, IDE_STEP, m_estep);
	
	DDX_Control(pDX, IDE_XDIM, m_editX);	
	DDX_Control(pDX, IDE_YDIM, m_editY);	
	DDX_Control(pDX, IDE_ZDIM, m_editZ);	

	DDX_Text(pDX, IDE_STEP, m_fstep);
	DDX_Text(pDX, IDS_MIN, m_fmin);	
	DDX_Text(pDX, IDS_MAX, m_fmax);	
	DDX_Text(pDX, IDS_AVER2, m_faver);	
	DDX_Text(pDX, IDS_NUMBER, m_total);	
	DDX_Text(pDX, IDS_PERCENT, m_fprc);	
}
void statDlg::fillNullRow()
{		
	flexReload();	
	m_vsflex.put_Cell(0, COleVariant(1L), COleVariant(0L), COleVariant(1L), COleVariant(0L),
						 COleVariant(m_fmin));	
}
void statDlg::minmax()
{	
	if (sz.cy != -1)
	{
		iniwline* piniwline = (iniwline*)app.m_graph.m_inilines[sz.cx];
		m_fmin = piniwline->m_flimits[0][sz.cy];	
		m_fmax = piniwline->m_flimits[1][sz.cy];	
	}
	else
	{
		legend* plegend = app.m_graph.m_inilines[sz.cx]->plegend;	
		m_fmin = plegend->m_fmin;	
		m_fmax = plegend->m_fmax;
	}

	m_total = 0;
	m_faver = m_fprc = 0.0f;
}
void statDlg::SaveCell()
{
	CFileDialog dlg(0, "*.txt", 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "Текстовые файлы (*.txt)|*.txt|", 0); 
	
	if (IDOK == dlg.DoModal())	
	{		
		m_vsflex.put_Rows(m_rows + 1);		
		m_vsflex.put_Cell(0, COleVariant(m_rows), COleVariant(2L), COleVariant(m_rows), COleVariant(2L),
						 COleVariant(m_total));	
		m_vsflex.put_Cell(0, COleVariant(m_rows), COleVariant(3L), COleVariant(m_rows), COleVariant(3L),
						 COleVariant(m_fprc));	
		m_vsflex.SaveGrid(dlg.m_ofn.lpstrFile, 4, COleVariant(1L, VT_BOOL));
		m_vsflex.put_Rows(m_rows);
	}
}
BEGIN_MESSAGE_MAP(statDlg, toolDlg)	
	ON_BN_CLICKED(IDC_ACTIVE, OnClickedActive)
	ON_BN_CLICKED(IDB_ANALISE, OnClickedAnalise)	
	ON_BN_CLICKED(IDC_DIV, OnClickedDiv)   	
	ON_BN_CLICKED(IDB_FILL, OnClickedFill)   	
	ON_BN_CLICKED(IDB_RESET, OnClickedReset)   	
	ON_BN_CLICKED(IDB_RESTRICT, OnClickedRestrict)   	
	ON_BN_CLICKED(IDC_TURN, OnClickedTurn)  		
	ON_CBN_SELCHANGE(IDC_DATE0, OnDate0Selchange)   			
	ON_CBN_SELCHANGE(IDC_DATE1, OnDate1Selchange)   			
	ON_CBN_SELCHANGE(IDC_PARAM, OnParamSelchange)   					
	ON_COMMAND(ID_ADDROW, AddRow)
	ON_COMMAND(ID_COPYCELL, CopyCell)
	ON_COMMAND(ID_SAVECELL, SaveCell)
	ON_COMMAND(ID_SELALL, SelAll)
	ON_COMMAND(ID_REMOVEROW, RemoveRow)	
	ON_WM_CREATE()	
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(statDlg, toolDlg)	
	ON_EVENT(statDlg, IDX_VSFLEX, 3, OnEnterCellFlex, VTS_NONE)
	ON_EVENT(statDlg, IDX_VSFLEX, 25, OnKeyPressEditFlex, VTS_I4 VTS_I4 VTS_PI2)					
	ON_EVENT(statDlg, IDX_VSFLEX, DISPID_MOUSEUP, OnMouseUpFlex, VTS_I2 VTS_I2 VTS_R4 VTS_R4)			
END_EVENTSINK_MAP()
int statDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (toolDlg::OnCreate(lpCreateStruct) == -1) return -1;
	HICON hIcon = (HICON)::LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_STAT), IMAGE_ICON, 16, 16, 0);
	SendMessage(WM_SETICON, ICON_SMALL, (LPARAM)hIcon);	
	
	m_xmenu.AppendMenu(MF_ENABLED, ID_ADDROW, 3, _T("Добавить ряд"));
	m_xmenu.AppendMenu(MF_ENABLED, ID_REMOVEROW, 4, _T("Удалить"));
	m_xmenu.AppendMenu(MF_SEPARATOR);
	m_xmenu.AppendMenu(MF_ENABLED, ID_SELALL, 5, _T("Выделить все"));
	m_xmenu.AppendMenu(MF_ENABLED, ID_COPYCELL, 6, _T("Копировать"));
	m_xmenu.AppendMenu(MF_SEPARATOR);
	m_xmenu.AppendMenu(MF_ENABLED, ID_SAVECELL, 7, _T("Сохранить"));	

	return 0; // return success	
}
void statDlg::OnClickedActive()
{
	bactive = !bactive;
}
void statDlg::OnClickedAnalise()
{	
	try
	{
		if (!getDims()) throw msg;
		
		long i = 1, 		 
			 size = m_vsflex.get_Rows();		
	
		stat _stat;			
		for (i; i < size; i++)
		{				
			if (sscanf_s(m_vsflex.get_TextMatrix(i, 0), "%f", &_stat.x0) == -1)
			{
				memset(msg, 0, MAX_PATH);
				sprintf_s(msg, MAX_PATH, "Ошибка в значении, ячейка (%u, %u)", i, 0);
				throw msg;
			}			
			if (sscanf_s(m_vsflex.get_TextMatrix(i, 1), "%f", &_stat.x1) == -1)
			{
				memset(msg, 0, MAX_PATH);
				sprintf_s(msg, MAX_PATH, "Ошибка в значении, ячейка (%u, %u)", i, 1);
				throw msg;
			}

			stats.push_back(_stat);
		}		
		m_total = 0;
		m_faver = m_fprc = 0.0f;		
		
		m_progr.SetPos(0);
		m_progr.ShowWindow(SW_SHOW);

		CString filename = app.m_graph.m_inilines[sz.cx]->fext;				
		if (bdiv)
		{
			if (!analiseDiv(filename)) throw msg;			
		}
		else
		{
			if (sz.cy != -1) 
			{
				if (!analiseRox(filename)) throw msg;
			}
			else
			{
				if (!analise(filename)) throw msg;			
			}
		}				 
		// fill results			
		float fprc;		
		for (i = 0; i < size - 1; i++) 
		{
			_stat = stats[i];		
			m_vsflex.put_Cell(0, COleVariant(i + 1), COleVariant(2L), COleVariant(i + 1), COleVariant(2L),
								 COleVariant(_stat.count));

			fprc = (m_total != 0) ? 100.0f * _stat.count / m_total : 0.0f;
			m_vsflex.put_Cell(0, COleVariant(i + 1), COleVariant(3L), COleVariant(i + 1), COleVariant(3L),
								 COleVariant(fprc));
			m_fprc += fprc;
		}			
		
		if (m_total != 0) m_faver /= m_total;		
		m_faver = (int)(m_faver * 1000.0f) / 1000.0f;
		
		UpdateData(0);
		
		GetDlgItem(IDS_AVER1)->ShowWindow(SW_SHOW);
		GetDlgItem(IDS_AVER2)->ShowWindow(SW_SHOW);
		GetDlgItem(IDS_TOTAL)->ShowWindow(SW_SHOW);
		GetDlgItem(IDS_NUMBER)->ShowWindow(SW_SHOW);
		GetDlgItem(IDS_PERCENT)->ShowWindow(SW_SHOW);		
	}
	catch (char*)
	{
		MessageBox(msg, app.m_pszAppName, MB_ICONWARNING);
	}
	stats.clear();		
	m_progr.ShowWindow(SW_HIDE);
}
void statDlg::OnClickedDiv()
{
	bdiv = !bdiv;

	int nCmdShow = bdiv ? SW_SHOW : SW_HIDE;
	m_comboD1.ShowWindow(nCmdShow);	
}
void statDlg::OnClickedFill()
{
	UpdateData(1);
	if (m_fstep == 0.0f)
	{
		MessageBox("шаг должен быть больше 0!", app.m_pszAppName, MB_ICONWARNING);		
		return;
	}	
	if ((m_fstep > fabs(m_fmax - m_fmin))) 
	{		
		m_vsflex.put_Rows(2);		
		m_vsflex.put_Cell(0, COleVariant(1L), COleVariant(1L),  COleVariant(1L), COleVariant(1L),
						     COleVariant(m_fmax));
	}
	else
	{			
		int num,
			div0 = (int)ROUND(m_fmin / m_fstep) + 1,								
		    div1 = (int)(m_fmax / m_fstep - 1);			
		
		if (div0 * m_fstep > m_fmin + m_fstep + FLT_EPSILON) div0--;		
		num = div1 - div0 + 2;			
		
		float fvalue = (div1 + 1) * (float)m_fstep;
		if ((fvalue >= m_fmax - FLT_EPSILON) && (fvalue <= m_fmax + FLT_EPSILON)) num--;
		
		SAFEARRAYBOUND bound[2];
		bound[0].cElements = 4;	
		bound[1].cElements = num + 1;
		bound[0].lLbound = bound[1].lLbound = 0;
		
		SAFEARRAY* psa = SafeArrayCreate(VT_VARIANT, 2, bound);	
		COleVariant* pvdata = (COleVariant*)psa->pvData;		
		// first row
		pvdata[0] = COleVariant(m_fmin);		
		fvalue = (float)(div0 * m_fstep); 		
		pvdata[1] = COleVariant(fvalue);		
		//
		int i = 1;
		for (i; i < num;  i++)
		{
			fvalue = (float)((div0 + i - 1) * m_fstep);				
			pvdata[4 * i] = COleVariant(fvalue);
			
			fvalue = (float)((div0 + i) * m_fstep);				
			pvdata[4 * i + 1] = COleVariant(fvalue);			
		}		
		// last row
		if (fvalue < m_fmax)
		{		
			fvalue = (float)((div0 + i - 1) * m_fstep);		 			
			pvdata[4 * i] = COleVariant(fvalue);						
			pvdata[4 * i + 1] = COleVariant(m_fmax);			
		}			
		COleVariant olearr;
		olearr.vt = VT_ARRAY | VT_VARIANT;
		olearr.parray = psa;
		m_vsflex.LoadArray(olearr, COleVariant(1L), COleVariant(0L), COleVariant(0L), COleVariant(0L));
	}	
	m_vsflex.Refresh();	
}
void statDlg::OnClickedReset()
{
	resetFilter();
	setFilter();
}
void statDlg::OnClickedRestrict()
{
	if (!getDims())
	{
		MessageBox(msg, app.m_pszAppName, MB_ICONWARNING);
		return;
	}
	m_total = 0;
	m_faver = m_fprc = 0.0f;

	BeginWaitCursor();	
	CString filename = app.m_graph.m_inilines[sz.cx]->fext;	
	
	try
	{		
		FILE* stream0;
		fopen_s(&stream0, filename, "rb");		 	
		if (!stream0) throw 0;		

		float fvalue0, fvalue1;		
		int i, iz, jy, kx, 			 		 
		    offset0 = 0;	
		
		float *pfarr0 = new float[size],
		      *pfarr1 = new float[size];
		
		m_fmin = FLT_MAX;
		m_fmax = -FLT_MAX;
		
		if (bdiv)		// div
		{
			FILE* stream1;
			fopen_s(&stream1, filename, "rb");					
			UINT offset1 = 0;		
			
			if (bactive)											// active cells
			{
				for (iz = toolDim.zDim.cx; iz < toolDim.zDim.cy; iz++)
				{
					offset0 = (2 + iz * size + sz.cy * (space.cube() + 1)) << 1;
					fseek(stream0, offset0 << 2, SEEK_SET);									
					if (fread(pfarr0, 4, size, stream0) != size) throw 0;
	
					offset1 = (2 + iz * size + m_cy * (space.cube() + 1)) << 1;
					fseek(stream1, offset1 << 2, SEEK_SET);								
					if (fread(pfarr1, 4, size, stream1) != size) throw 1;

					for (jy = toolDim.yDim.cx; jy < toolDim.yDim.cy; jy++)
					{
						for (kx = toolDim.xDim.cx; kx < toolDim.xDim.cy; kx++)
						{
							i = (space.nX - 1) * (iz * (space.nY - 1) + jy) + kx;
							if (!pbarr[i]) continue;
							
							fvalue0 = pfarr0[jy * space.nX + kx];
							fvalue1 = pfarr1[jy * space.nX + kx];				
					
							if (fvalue1 != 0.0f) 
							{
								m_fmin = min(m_fmin, fvalue0 / fvalue1);
								m_fmax = max(m_fmax, fvalue0 / fvalue1);						
							}			
						}
					}
				}									
			}
			else													// not active cells
			{
				for (iz = toolDim.zDim.cx; iz < toolDim.zDim.cy; iz++)
				{
					offset0 = (2 + iz * size + sz.cy * (space.cube() + 1)) << 1;
					fseek(stream0, offset0 << 2, SEEK_SET);									
					if (fread(pfarr0, 4, size, stream0) != size) throw 0;
	
					offset1 = (2 + iz * size + m_cy * (space.cube() + 1)) << 1;
					fseek(stream1, offset1 << 2, SEEK_SET);								
					if (fread(pfarr1, 4, size, stream1) != size) throw 1;

					for (jy = toolDim.yDim.cx; jy < toolDim.yDim.cy; jy++)
					{
						for (kx = toolDim.xDim.cx; kx < toolDim.xDim.cy; kx++)
						{
							fvalue0 = pfarr0[jy * space.nX + kx];
							fvalue1 = pfarr1[jy * space.nX + kx];				
					
							if (fvalue1 != 0.0f) 
							{
								m_fmin = min(m_fmin, fvalue0 / fvalue1);
								m_fmax = max(m_fmax, fvalue0 / fvalue1);						
							}			
						}
					}
				}
			}
			fclose(stream1);	
		}
		else					// not div
		{
			if (sz.cy != -1)	// roxar
			{
				if (bactive)											// active cells
				{
					for (iz = toolDim.zDim.cx; iz < toolDim.zDim.cy; iz++)
					{
						offset0 = (2 + iz * size + sz.cy * (space.cube() + 1)) << 1;
						fseek(stream0, offset0 << 2, SEEK_SET);													
				
						if (fread(pfarr0, 4, size, stream0) != size) throw 0;				

						for (jy = toolDim.yDim.cx; jy < toolDim.yDim.cy; jy++)
						{
							for (kx = toolDim.xDim.cx; kx < toolDim.xDim.cy; kx++)
							{
								i = (space.nX - 1) * (iz * (space.nY - 1) + jy) + kx;
								if (!pbarr[i]) continue;

								fvalue0 = pfarr0[jy * space.nX + kx];						
					
								m_fmin = min(m_fmin, fvalue0);
								m_fmax = max(m_fmax, fvalue0);												
							}
						}
					}												
				}				
				else													// not active cells
				{
					for (iz = toolDim.zDim.cx; iz < toolDim.zDim.cy; iz++)
					{
						offset0 = (2 + iz * size + sz.cy * (space.cube() + 1)) << 1;
						fseek(stream0, offset0 << 2, SEEK_SET);													
				
						if (fread(pfarr0, 4, size, stream0) != size) throw 0;				

						for (jy = toolDim.yDim.cx; jy < toolDim.yDim.cy; jy++)
						{
							for (kx = toolDim.xDim.cx; kx < toolDim.xDim.cy; kx++)
							{
								fvalue0 = pfarr0[jy * space.nX + kx];						
					
								m_fmin = min(m_fmin, fvalue0);
								m_fmax = max(m_fmax, fvalue0);												
							}
						}
					}			
				}
			}
			else		// not roxar
			{
				if (bactive)											// active cells
				{
					for (iz = toolDim.zDim.cx; iz < toolDim.zDim.cy; iz++)
					{
						offset0 = iz * size;						
						fseek(stream0, ++offset0 << 3, SEEK_SET);													
					
						if (fread(pfarr0, 4, size, stream0) != size) throw 0;				
	
						for (jy = toolDim.yDim.cx; jy < toolDim.yDim.cy; jy++)
						{
							for (kx = toolDim.xDim.cx; kx < toolDim.xDim.cy; kx++)
							{
								i = (space.nX - 1) * (iz * (space.nY - 1) + jy) + kx;
								if (!pbarr[i]) continue;

								fvalue0 = pfarr0[jy * space.nX + kx];						
					
								m_fmin = min(m_fmin, fvalue0);
								m_fmax = max(m_fmax, fvalue0);												
							}
						}
					}												
				}
				else													// not active cells
				{
					for (iz = toolDim.zDim.cx; iz < toolDim.zDim.cy; iz++)
					{
						offset0 = iz * size;						
						fseek(stream0, ++offset0 << 3, SEEK_SET);													
					
						if (fread(pfarr0, 4, size, stream0) != size) throw 0;				
	
						for (jy = toolDim.yDim.cx; jy < toolDim.yDim.cy; jy++)
						{
							for (kx = toolDim.xDim.cx; kx < toolDim.xDim.cy; kx++)
							{
								fvalue0 = pfarr0[jy * space.nX + kx];						
					
								m_fmin = min(m_fmin, fvalue0);
								m_fmax = max(m_fmax, fvalue0);												
							}
						}
					}			
				}
			}
		}
		m_fmin = floor(m_fmin * 1000.0f) / 1000.0f;
		m_fmax = ceil(m_fmax * 1000.0f) / 1000.0f;

		UpdateData(0);				
		fillNullRow();
		
		SAFE_DELETE_ARRAY(pfarr0);
		SAFE_DELETE_ARRAY(pfarr1);		
		fclose(stream0);	
	}
	catch (int)	
	{		
		memset(msg, 0, MAX_PATH);
		sprintf_s(msg, MAX_PATH, "Не удалось открыть %s.", filename);					
		MessageBox(msg, app.m_pszAppName, MB_ICONWARNING);
	}		
	EndWaitCursor();
}
void statDlg::OnClickedTurn()
{	
	CButton *pButton = (CButton*)GetDlgItem(IDC_TURN); 	
	int nCmdShow = (BST_CHECKED == pButton->GetCheck()) ? SW_SHOW : SW_HIDE;			
		
	m_estep.ShowWindow(nCmdShow);		
	GetDlgItem(IDB_FILL)->ShowWindow(nCmdShow);		
	GetDlgItem(IDS_STEP)->ShowWindow(nCmdShow);			
}
void statDlg::OnDate0Selchange()
{
	sz.cy = (int)m_comboD0.SendMessage(CB_GETCURSEL, 0, 0);		
	
	minmax();
	UpdateData(0);
	fillNullRow();
}
void statDlg::OnDate1Selchange()
{
	m_cy = (int)m_comboD1.SendMessage(CB_GETCURSEL, 0, 0);			
}
void statDlg::OnEnterCellFlex()
{
	long col = m_vsflex.get_Col(),
		 newValue = (col > 1) ? 0 : 2;
		 
	m_vsflex.put_Editable(newValue);		
}
void statDlg::OnKeyPressEditFlex(long Row, long Col, short FAR* KeyAscii)
{
	if (!charFLOAT(*KeyAscii)) m_vsflex.PostMessage(WM_KEYDOWN, VK_BACK);		
}
void statDlg::OnMouseUpFlex(short Button, short Shift, float X, float Y)
{		
	m_row = m_vsflex.get_MouseRow();
	m_cols = m_vsflex.get_Cols();
	m_rows = m_vsflex.get_Rows();
	
	UINT nEnable0 = (m_row > 0) ? MF_ENABLED : MF_DISABLED | MF_GRAYED,
		 nEnable1 = (m_row > 1) && (m_rows > 2) ? MF_ENABLED : MF_DISABLED | MF_GRAYED;			 
	
	m_xmenu.EnableMenuItem(ID_ADDROW, nEnable0);		
	m_xmenu.EnableMenuItem(ID_REMOVEROW, nEnable1);		

	toolDlg::OnMouseUpFlex(Button, Shift, X, Y);		
}
void statDlg::OnParamSelchange()
{
	sz.cx = (int)m_comboP.SendMessage(CB_GETCURSEL, 0, 0);	
	LPVOID pData = (LPVOID)m_comboP.SendMessage(CB_GETITEMDATA, sz.cx, 0);							
	
	bool bdata = (pData != 0);	
	sz.cy = bdata ? (int)m_comboD0.SendMessage(CB_GETCURSEL, 0, 0) : -1;	
	m_comboD0.EnableWindow(bdata);				
	GetDlgItem(IDC_DIV)->EnableWindow(bdata);

	minmax();
	UpdateData(0);			
	fillNullRow();
}
// stockDlg
const CRect stockDlg::s_rcDefault(0, 0, 285, 371);
stockDlg::stockDlg() : toolDlg(stockDlg::IDD)
{
}
bool stockDlg::calc(const char* filename)
{
	bool bres = 1;
	BeginWaitCursor();
	try
	{
		FILE *stream;
		fopen_s(&stream, filename, "rb");
		if (!stream) throw 0;	

		float fvalue;
		int iz, jy, kx, offset, dsize = space.xStep * space.yStep;			 	     
		stock _stock;			
		float* pfarr = new float[size];		
	
		for (iz = toolDim.zDim.cx; iz < toolDim.zDim.cy; iz++)
		{
			_stock.area = _stock.oil = 0.0f;
			offset = iz * size;

			fseek(stream, ++offset << 3, SEEK_SET);				
			if (fread(pfarr, 4, size, stream) != size) throw 0;

			for (jy = toolDim.yDim.cx; jy < toolDim.yDim.cy; jy++)
			{							
				for (kx = toolDim.xDim.cx; kx < toolDim.xDim.cy; kx++)
				{
					fvalue = pfarr[jy * space.nX + kx];
					if (fvalue > 0.0f) _stock.area += dsize;					
					_stock.oil += fvalue * dsize / 1000.f;												
				}				
			}			
			stocks.push_back(_stock);
			m_progr.StepIt();			
		}											
		SAFE_DELETE_ARRAY(pfarr);	
		fclose(stream);
	}
	catch (int)
	{		
		memset(msg, 0, MAX_PATH);
		sprintf_s(msg, MAX_PATH, "Не удалось открыть %s.", filename);		
		bres = 0;		
	}
	EndWaitCursor();
	return bres;	
}
BOOL stockDlg::Create(UINT nIDTemplate, CWnd* pParentWnd)
{
	m_pParentWnd = pParentWnd;
	return toolDlg::Create(nIDTemplate, pParentWnd);
}
void stockDlg::DoDataExchange(CDataExchange* pDX)
{
	toolDlg::DoDataExchange(pDX);		

	DDX_Control(pDX, IDC_PROGR, m_progr);
	DDX_Control(pDX, IDX_VSFLEX, m_vsflex);	
	
	DDX_Control(pDX, IDE_XDIM, m_editX);	
	DDX_Control(pDX, IDE_YDIM, m_editY);	
	DDX_Control(pDX, IDE_ZDIM, m_editZ);	

	DDX_Text(pDX, IDS_NUMBER, m_ftotal);	
}
void stockDlg::OnCancel()
{		
	toolDlg::OnCancel();	
	m_pParentWnd->PostMessage(ID_STOCK_CLOSED);
}
void stockDlg::reload()
{
	flexReload();	
	resetFilter();
	setFilter();
	
	GetDlgItem(IDS_TOTAL)->ShowWindow(SW_HIDE);
	GetDlgItem(IDS_NUMBER)->ShowWindow(SW_HIDE);
}
void stockDlg::SaveCell()
{
	CFileDialog dlg(0, "*.txt", 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "Текстовые файлы (*.txt)|*.txt|", 0); 
	
	if (IDOK == dlg.DoModal())	
	{		
		m_vsflex.put_Rows(m_rows + 1);		
		m_vsflex.put_Cell(0, COleVariant(m_rows), COleVariant(2L), COleVariant(m_rows), COleVariant(2L),
						 COleVariant(m_ftotal));	
		m_vsflex.SaveGrid(dlg.m_ofn.lpstrFile, 4, COleVariant(1L, VT_BOOL));
		m_vsflex.put_Rows(m_rows);
	}
}
void stockDlg::setWindow(const RECT& rect)
{
	WINDOWPLACEMENT wndpl;
	wndpl.length = 44;	
	wndpl.rcNormalPosition = rect;	
	wndpl.rcNormalPosition.left = rect.right - s_rcDefault.Width();	
	wndpl.rcNormalPosition.bottom = rect.top + s_rcDefault.Height();
	
	SetWindowPlacement(&wndpl);	
}
BEGIN_MESSAGE_MAP(stockDlg, toolDlg)
	ON_BN_CLICKED(IDB_RESET, OnClickedReset)   	
	ON_BN_CLICKED(IDB_STOCK, OnClickedStock)   	
	ON_COMMAND(ID_COPYCELL, CopyCell)
	ON_COMMAND(ID_SAVECELL, SaveCell)
	ON_COMMAND(ID_SELALL, SelAll)
	ON_WM_CREATE()
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(stockDlg, toolDlg)
	ON_EVENT(stockDlg, IDX_VSFLEX, DISPID_MOUSEUP, OnMouseUpFlex, VTS_I2 VTS_I2 VTS_R4 VTS_R4)	
END_EVENTSINK_MAP()
int stockDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (toolDlg::OnCreate(lpCreateStruct) == -1) return -1;	
	
	m_xmenu.AppendMenu(MF_ENABLED, ID_SELALL, 5, _T("Выделить все"));
	m_xmenu.AppendMenu(MF_ENABLED, ID_COPYCELL, 6, _T("Копировать"));
	m_xmenu.AppendMenu(MF_SEPARATOR);
	m_xmenu.AppendMenu(MF_ENABLED, ID_SAVECELL, 7, _T("Сохранить"));		
	
	return 0; // return success	
}
void stockDlg::OnClickedReset()
{
	resetFilter();
	setFilter();
}
void stockDlg::OnClickedStock()
{
	try
	{
		if (!getDims()) throw msg;

		UCHAR i, size = toolDim.zLen();				
		CString filename = app.m_graph.m_inilines[_stockline.num]->fext;
		stock _stock;

		m_progr.SetRange(0, size); 		
		m_progr.SetPos(0);
		m_progr.ShowWindow(SW_SHOW);		

		if (!calc(filename)) throw msg;				
		// fill results					
		m_ftotal = 0.0f;	
		SAFEARRAYBOUND bound[2];
		bound[0].cElements = 3;	
		bound[1].cElements = size;
		bound[0].lLbound = bound[1].lLbound = 1;


		SAFEARRAY* psa = SafeArrayCreate(VT_VARIANT, 2, bound);	
		COleVariant* pvdata = (COleVariant*)psa->pvData;
		for (i = 0; i < size;  i++)
		{			
			_stock = stocks[i];
			m_ftotal += _stock.oil;

			pvdata[3 * i] = COleVariant((short)(toolDim.zDim.cx + i + 1));
			pvdata[3 * i + 1] = COleVariant(_stock.area / 1000.0f);
			pvdata[3 * i + 2] = COleVariant(_stock.oil);			
		}				
		stocks.clear();
		
		COleVariant olearr;
		olearr.vt = VT_ARRAY | VT_VARIANT;
		olearr.parray = psa;
		
		m_vsflex.LoadArray(olearr, COleVariant(1L), COleVariant(0L), COleVariant(0L), COleVariant(0L));		
		m_vsflex.Refresh();		
		
		m_ftotal = (int)(m_ftotal * 10.0f) / 10.0f;
		UpdateData(0);		
		GetDlgItem(IDS_TOTAL)->ShowWindow(SW_SHOW);
		GetDlgItem(IDS_NUMBER)->ShowWindow(SW_SHOW);
	}
	catch (char*)
	{
		MessageBox(msg, app.m_pszAppName, MB_ICONWARNING);
	}
	m_progr.ShowWindow(SW_HIDE);
}
void stockDlg::OnMouseUpFlex(short Button, short Shift, float X, float Y)
{
	m_cols = m_vsflex.get_Cols();
	m_rows = m_vsflex.get_Rows();	
	
	toolDlg::OnMouseUpFlex(Button, Shift, X, Y);
}
