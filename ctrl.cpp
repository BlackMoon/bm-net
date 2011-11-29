// ctrl.cpp
#include "stdafx.h"
#include "ctrl.h"
#include ".\ctrl.h"

char						dec;
DWORD						cust[] = {0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 
									  0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 
									  0xffffff, 0xffffff, 0xffffff, 0xffffff};

bool charUINT(UINT nChar)
{
	switch (nChar)
	{
		case 0x8:
		case 0x30:				
		case 0x31:
		case 0x32:
		case 0x33:
		case 0x34:
		case 0x35:
		case 0x36:
		case 0x37:
		case 0x38:
		case 0x39: return 1;
	}
	return 0;
}
bool charINT(UINT nChar)
{
	if (nChar == 0x2D) return 1;
	return charUINT(nChar);		
}
bool charFLOAT(UINT nChar)
{
	if (nChar == dec) return 1;
	return charINT(nChar);
}
bool charUFLOAT(UINT nChar)
{
	if (nChar == dec) return 1;
	return charUINT(nChar);
}
void drawEmbossed(CDC* pDC, CImageList* pimages, UINT index, const RECT& rect)
{
	CDC	memDC;
	memDC.CreateCompatibleDC(pDC);
	int cx = rect.right - rect.left, cy = rect.bottom - rect.top;	
	
	CBitmap bm;	
	bm.CreateCompatibleBitmap(pDC, cx, cy);
	// draw image into memory DC fill BG white first
	CBitmap* poldBitmap = memDC.SelectObject(&bm);
	memDC.PatBlt(0, 0, cx, cy, WHITENESS);
	pimages->Draw(&memDC, index, CPoint(0, 0), ILD_NORMAL);
	// This seems to be required. Why, I don't know. ???
	COLORREF oldBG = pDC->SetBkColor(0xffffffff);
	// Draw using hilite offset by (1, 1), then shadow	
	CBrush brShadow(GetSysColor(COLOR_3DSHADOW));
	CBrush brHilite(GetSysColor(COLOR_3DHIGHLIGHT));

	CBrush* poldBrush = pDC->SelectObject(&brHilite);
	pDC->BitBlt(rect.left + 1, rect.top + 1, cx, cy, &memDC, 0, 0, 0xb8074a);
	pDC->SelectObject(&brShadow);
	pDC->BitBlt(rect.left, rect.top, cx, cy, &memDC, 0, 0, 0xb8074a);
	pDC->SelectObject(poldBrush);
	pDC->SetBkColor(oldBG);				 // restore
	memDC.SelectObject(poldBitmap);		 
	
	bm.Detach();	
	brShadow.Detach();
	brHilite.Detach();
}
// CCtrlToolBar
bool CCtrlToolBar::setTrueColor(UINT nIDResource)
{
	BITMAP bm;
	CBitmap bitmap;
	CImageList ilist;
	
	if (!bitmap.Attach(LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(nIDResource), 
		IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE | LR_CREATEDIBSECTION))) return 0; 
	if (!bitmap.GetBitmap(&bm)) return 0;			
	
	if (!ilist.Attach(ImageList_Create(24, ITEM_H, ILC_COLORDDB | ILC_MASK, bm.bmWidth / 24, 0)))
		return 0;		
	
	RGBTRIPLE* rgb = (RGBTRIPLE*)(bm.bmBits);
	COLORREF rgbMask = RGB(rgb[0].rgbtRed, rgb[0].rgbtGreen, rgb[0].rgbtBlue);

	if (ilist.Add(&bitmap, rgbMask) == -1) return 0;
    SendMessage(TB_SETIMAGELIST, 0, (LPARAM)ilist.m_hImageList);
    
	ilist.Detach(); 
	bitmap.DeleteObject();
	
	return 1;
}
void CCtrlToolBar::dropArrow(UINT nID, UINT nMenu)
{
	menuID = nMenu;
	SendMessage(TB_SETEXTENDEDSTYLE, 0, (LPARAM)TBSTYLE_EX_DRAWDDARROWS);	
	
	DWORD dw = GetButtonStyle(CommandToIndex(nID));
	dw |= TBSTYLE_DROPDOWN;
	SetButtonStyle(CommandToIndex(nID), dw);	
}
CWnd* CCtrlToolBar::InsertControl(CRuntimeClass* pClass, LPCTSTR lpszWindowName, CRect& rect, UINT nID, DWORD dwStyle)
{
	CWnd* pCtrl = 0;	
	if (pClass->IsDerivedFrom(RUNTIME_CLASS(CComboBox)))	// CComboBox control.
	{
		pCtrl = new CODCombo();
		ASSERT_VALID(pCtrl);
		if (!((CODCombo*)pCtrl)->Create(WS_CHILD | WS_VISIBLE | dwStyle, rect, this, nID))
		{			
			SAFE_DELETE(pCtrl);
			return 0;
		}
	}
	else if( pClass->IsDerivedFrom(RUNTIME_CLASS(CEdit)))		// CEdit control.
	{
		pCtrl = new CEdit();
		ASSERT_VALID(pCtrl);
		if (!((CEdit*)pCtrl)->Create(WS_CHILD | WS_VISIBLE | dwStyle, rect, this, nID))
		{
			SAFE_DELETE(pCtrl);
			return 0;
		}
	}
	else if (pClass->IsDerivedFrom(RUNTIME_CLASS(CButton)))		// CButton control.
	{
		pCtrl = new CButton();
		ASSERT_VALID(pCtrl);
		if (!((CButton*)pCtrl)->Create(lpszWindowName, WS_CHILD | WS_VISIBLE | dwStyle, rect, this, nID))
		{
			SAFE_DELETE(pCtrl);
			return 0;
		}
	}
	else if( pClass->IsDerivedFrom(RUNTIME_CLASS(CWnd)))			// CWnd object.
	{
		pCtrl = new CWnd();		
		ASSERT_VALID(pCtrl);
#ifdef _UNICODE
		TCHAR szClassName[_MAX_PATH];
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, pClass->m_lpszClassName, -1, szClassName, 255);
		if (!((CWnd*)pCtrl)->Create(szClassName, lpszWindowName, WS_CHILD | WS_VISIBLE | dwStyle, rect, this, nID))
		{
			SAFE_DELETE(pCtrl);
			return 0;
		}
#else
		if (!((CWnd*)pCtrl)->Create(pClass->m_lpszClassName, lpszWindowName, WS_CHILD | WS_VISIBLE | dwStyle, rect, this, nID))
		{
			SAFE_DELETE(pCtrl);
			return 0;
		}
#endif
	}
	else															// An invalid object was passed in
	{
		ASSERT(0);
		return 0;
	}
	return InsertControl(pCtrl, rect, nID);	
}
CWnd* CCtrlToolBar::InsertControl(CWnd* pCtrl, CRect& rect, UINT nID)
{
	ASSERT_VALID(pCtrl);
	// make sure the id is valid, and set the button style for a seperator.
	int nState, nIndex = CommandToIndex(nID);
	if (nIndex > -1)
	{
		ASSERT(nIndex >= 0);
		SetButtonInfo(nIndex, nID, TBBS_SEPARATOR, rect.Width());
		// insert the control into the toolbar.
		GetItemRect(nIndex, &rect);				
		pCtrl->SetWindowPos(0, rect.left, rect.top, 0, 0,
			SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOCOPYBITS );
		pCtrl->SetFont(GetFont( ));
		BOOL bVert = (m_dwStyle & CBRS_ORIENT_VERT) != 0;
		if (bVert)
		{
		   	nState = GetToolBarCtrl().GetState(nIndex);
		   	GetToolBarCtrl().SetState(nID,(nState | TBSTATE_HIDDEN));
			pCtrl->ShowWindow(SW_HIDE);
		}
		else
		{
		   	nState = GetToolBarCtrl().GetState(nIndex);
		   	GetToolBarCtrl().SetState(nIndex,(nState & ~TBSTATE_HIDDEN));
			pCtrl->ShowWindow(SW_SHOW);
		}
	}
	else pCtrl->ShowWindow(SW_HIDE);	
	return pCtrl;	
}
BEGIN_MESSAGE_MAP(CCtrlToolBar, CToolBar)
	ON_NOTIFY_REFLECT(TBN_DROPDOWN, OnDropDown)
END_MESSAGE_MAP()
void CCtrlToolBar::OnDropDown(NMHDR *pNMHDR, LRESULT *pResult)
{	
	LPNMTOOLBAR pNMTB = reinterpret_cast<LPNMTOOLBAR>(pNMHDR);
	CMenu menu;
	if (!menu.LoadMenu(menuID))
	{
		TRACE("Failed to load %u\n", menuID);
		return;
	}	
	RECT rc;	
	SendMessage(TB_GETRECT, (WPARAM)pNMTB->iItem, (LPARAM)&rc);
	ClientToScreen(&rc);			
	menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, rc.left, rc.bottom, this, 0); 
}
// CODCombo
IMPLEMENT_DYNAMIC(CODCombo, CComboBox)
void CODCombo::DrawItem(LPDRAWITEMSTRUCT lpDIS)
{
	if (SendMessage(CB_GETCOUNT, 0, 0) == 0) return;
	
	CDC dc;
	dc.Attach(lpDIS->hDC);	
	dc.SelectStockObject(DEFAULT_GUI_FONT);   
	// Save these value to restore them when done drawing.
	COLORREF oldText = dc.GetTextColor(),
			 oldBkgnd = dc.GetBkColor();

	if (!IsWindowEnabled()) dc.SetTextColor(::GetSysColor(COLOR_GRAYTEXT));		
	if ((lpDIS->itemAction | ODA_SELECT) && (lpDIS->itemState  & ODS_SELECTED))
	{
		dc.SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
		dc.SetBkColor(::GetSysColor(COLOR_HIGHLIGHT));
		dc.FillSolidRect(&lpDIS->rcItem, ::GetSysColor(COLOR_HIGHLIGHT));
	}
	else dc.FillSolidRect(&lpDIS->rcItem, oldBkgnd);		

	RECT rc(lpDIS->rcItem);
	rc.left += 2;
	rc.right -= 2;	   

	TCHAR line[32];
	memset(line, 0, 32);
	SendMessage(CB_GETLBTEXT, lpDIS->itemID, (LPARAM)line);

	dc.DrawText(line, &rc, DT_LEFT | DT_SINGLELINE | DT_VCENTER);				

	dc.SetTextColor(oldText);
	dc.SetBkColor(oldBkgnd);
	dc.Detach();
}
void CODCombo::MeasureItem(LPMEASUREITEMSTRUCT)
{
	ASSERT(1);
}
BEGIN_MESSAGE_MAP(CODCombo, CComboBox)
	ON_WM_CREATE()
END_MESSAGE_MAP()
int CODCombo::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CComboBox::OnCreate(lpCreateStruct) == -1)
	{
		TRACE0("Failed to create odcombo\n");
		return -1;      // fail to create
	}
	SendMessage(CB_SETITEMHEIGHT, -1, MAKELONG(ITEM_H, 0)); 	
	return 0;
}
// CNSplitter
CNSplitter::CNSplitter()
{
	m_cxSplitter = m_cySplitter = 1;
	m_cxBorderShare = m_cyBorderShare = 0;
	m_cxSplitterGap = m_cySplitterGap = 1;
	m_cxBorder = m_cyBorder = 1;
}
void CNSplitter::OnDrawSplitter(CDC* pDC, ESplitType nType, const CRect& rect)
{	// let CSplitterWnd handle everything but the border-drawing
	if ((nType != splitBorder) || (pDC == 0))
	{
		CSplitterWnd::OnDrawSplitter(pDC, nType, rect);
		return;
	}

	BYTE r = (BYTE)((app.stg._g_set.dw_bkgnd >> 0x10) & 0xff),
		 g = (BYTE)((app.stg._g_set.dw_bkgnd >> 0x8) & 0xff),
		 b = (BYTE)(app.stg._g_set.dw_bkgnd & 0xff);	
	COLORREF clr = RGB(r, g, b);
	
	ASSERT_VALID(pDC);			
	pDC->Draw3dRect(rect, clr, clr);		
	if (rect.top == 0) // draw views border
	{
		pDC->FillSolidRect(CRect(rect.left, rect.top, rect.left + 1, rect.bottom), 
			GetSysColor(COLOR_BTNSHADOW));
		pDC->FillSolidRect(CRect(rect.left, rect.top, rect.right, rect.top + 1), 
			GetSysColor(COLOR_BTNSHADOW));
		pDC->FillSolidRect(CRect(rect.right, rect.top, rect.right - 1, rect.bottom), 
			GetSysColor(COLOR_BTNSHADOW));
	}
	else 
	{
		pDC->FillSolidRect(CRect(rect.left, rect.top, rect.left + 1, rect.bottom), 
			GetSysColor(COLOR_BTNSHADOW));
		pDC->FillSolidRect(CRect(rect.left, rect.bottom, rect.right, rect.bottom - 1), 
			GetSysColor(COLOR_BTNSHADOW));
		pDC->FillSolidRect(CRect(rect.right, rect.top, rect.right - 1, rect.bottom), 
			GetSysColor(COLOR_BTNSHADOW));
	}	
}
BEGIN_MESSAGE_MAP(CNSplitter, CSplitterWnd)
	ON_WM_NCHITTEST()
END_MESSAGE_MAP()
LRESULT CNSplitter::OnNcHitTest(CPoint point)
{
	return HTNOWHERE;
}
// CUEdit
LRESULT CUEdit::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_PASTE:
		case WM_RBUTTONDOWN:	return 0;
		default: return CEdit::WindowProc(message, wParam, lParam);
	}		
}
BEGIN_MESSAGE_MAP(CUEdit, CEdit)
	ON_WM_CHAR()
END_MESSAGE_MAP()
void CUEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{	
	if (charUINT(nChar)) CEdit::OnChar(nChar, nRepCnt, nFlags);	
}
// CIEdit
BEGIN_MESSAGE_MAP(CIEdit, CUEdit)
	ON_WM_CHAR()
END_MESSAGE_MAP()
void CIEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{	
	if (charINT(nChar)) CEdit::OnChar(nChar, nRepCnt, nFlags);	
}
// CFEDit
BEGIN_MESSAGE_MAP(CFEdit, CUEdit)
	ON_WM_CHAR()
END_MESSAGE_MAP()
void CFEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{	
	if (charFLOAT(nChar)) CEdit::OnChar(nChar, nRepCnt, nFlags);	
}
// CUFEdit
BEGIN_MESSAGE_MAP(CUFEdit, CUEdit)
	ON_WM_CHAR()
END_MESSAGE_MAP()
void CUFEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{	
	if (charUFLOAT(nChar)) CEdit::OnChar(nChar, nRepCnt, nFlags);	
}
// memDC
memDC::memDC(CDC* pDC) : CDC()
{
	ASSERT(pDC);
    m_pDC = pDC;
    m_poldBitmap = 0;
    m_bmemDC = !pDC->IsPrinting();
              
    if (m_bmemDC)    // Create a Memory DC
    {
		pDC->GetClipBox(&m_rc);
        CreateCompatibleDC(pDC);
		m_bitmap.CreateCompatibleBitmap(pDC, m_rc.right - m_rc.left, m_rc.bottom - m_rc.top);
        m_poldBitmap = SelectObject(&m_bitmap);
        SetWindowOrg(m_rc.left, m_rc.top);
	}
    else        // Make a copy of the relevent parts of the current DC for printing
    {            
		m_hAttribDC = pDC->m_hAttribDC;
		m_hDC = pDC->m_hDC;
		m_bPrinting = pDC->m_bPrinting;
	}}        
memDC::~memDC()
{
	if (m_bmemDC) 
    {               
		m_pDC->BitBlt(m_rc.left, m_rc.top, m_rc.right - m_rc.left, m_rc.bottom - m_rc.top,
                      this, m_rc.left, m_rc.top, SRCCOPY);
        SelectObject(m_poldBitmap);
    }
	else m_hAttribDC = m_hDC = 0;        
}    
// CODProgress
CODProgress::CODProgress()
{
	m_nBarWidth = -1;
	m_nMin = m_nPos = 0;
    m_nMax = 100;
	m_nStepSize = 1;   
    
	m_bshowText = 1;    
	m_strText.Empty();

    m_clBar = m_clBkgnd = m_clText = CLR_DEFAULT;
	m_dwTextStyle = DT_CENTER | DT_SINGLELINE | DT_VCENTER;    
}
COLORREF CODProgress::setBarColor(COLORREF cl)
{
    if (::IsWindow(m_hWnd)) Invalidate();

    COLORREF cloldBar = m_clBar;
    m_clBar = cl;
    return cloldBar;
}
COLORREF CODProgress::setBgndColor(COLORREF cl)
{
    if (::IsWindow(m_hWnd)) Invalidate();

    COLORREF cloldBkgnd = m_clBkgnd;
    m_clBkgnd = cl;
    return cloldBkgnd;
}
COLORREF CODProgress::setTextColor(COLORREF cl)
{
    if (::IsWindow(m_hWnd)) Invalidate();

    COLORREF cloldText = m_clText;
    m_clText = cl;
    return cloldText;
}
void CODProgress::SetShowText(BOOL bshow)
{ 
    if (::IsWindow(m_hWnd) && m_bshowText != bshow) Invalidate();
    m_bshowText = bshow;
}
BEGIN_MESSAGE_MAP(CODProgress, CProgressCtrl)        
    ON_MESSAGE(WM_GETTEXT, OnGetText)
	ON_MESSAGE(WM_SETTEXT, OnSetText)    
    ON_MESSAGE(PBM_DELTAPOS, OnOffsetPos)
    ON_MESSAGE(PBM_GETPOS, OnGetPos)
    ON_MESSAGE(PBM_GETRANGE, OnGetRange)
    ON_MESSAGE(PBM_SETBARCOLOR, OnSetBarColor)
    ON_MESSAGE(PBM_SETBKCOLOR, OnSetBkgndColor)
    ON_MESSAGE(PBM_SETPOS, OnSetPos)
    ON_MESSAGE(PBM_SETRANGE, OnSetRange)
    ON_MESSAGE(PBM_SETRANGE32, OnSetRange32)
    ON_MESSAGE(PBM_SETSTEP, OnSetStep)
    ON_MESSAGE(PBM_STEPIT, OnStepIt)
    ON_MESSAGE(PBM_SETSHOWTEXT, OnSetShowText)
	ON_MESSAGE(PBM_SETTEXTCOLOR, OnSetTextColor)    
	ON_WM_ERASEBKGND()
    ON_WM_PAINT()
    ON_WM_SIZE()
END_MESSAGE_MAP()
LRESULT CODProgress::OnGetText(UINT wParam, LPARAM lParam)
{
    UINT cchTextMax = (UINT)wParam;
    LPTSTR szText = (LPTSTR)lParam;
    if (!strncpy_s(szText, 32, m_strText, cchTextMax)) return 0;
    else return min(cchTextMax, (UINT)m_strText.GetLength());
}
LRESULT CODProgress::OnSetText(WPARAM, LPARAM lParam)
{
    LRESULT result = Default();
    LPCTSTR szText = (LPCTSTR) lParam;

    if ((!szText && m_strText.GetLength()) || (szText && (m_strText != szText)))
    {
        m_strText = szText;
        Invalidate();
    }
    return result;
}
LRESULT CODProgress::OnOffsetPos(WPARAM nPos, LPARAM)
{
    return (LRESULT)SetPos(m_nPos + (int)nPos);
}
LRESULT CODProgress::OnGetPos(WPARAM, LPARAM)
{
    return (LRESULT)m_nPos;
}
LRESULT CODProgress::OnGetRange(WPARAM type, LPARAM range)
{
    BOOL bType = (BOOL)type;
    PPBRANGE pRange = (PPBRANGE)range;
    if (pRange)
    {
        pRange->iHigh = m_nMax;
        pRange->iLow = m_nMin;
    }
    return (LRESULT)(bType ? m_nMin : m_nMax);
}
LRESULT CODProgress::OnSetBarColor(WPARAM, LPARAM cl)
{
    return (LRESULT)setBarColor((COLORREF)cl);
}
LRESULT CODProgress::OnSetBkgndColor(WPARAM, LPARAM cl)
{
    return (LRESULT)setBgndColor((COLORREF)cl);
}
LRESULT CODProgress::OnSetPos(WPARAM newPos, LPARAM) 
{    
    if (!::IsWindow(m_hWnd)) return -1;

    int nPos = (int)newPos, oldPos = m_nPos;
    m_nPos = nPos;

    CRect rect;
    GetClientRect(rect);

    double fraction = (double)(m_nPos - m_nMin) / ((double)(m_nMax - m_nMin));
    int nBarWidth = (int)(fraction * rect.Width());

    if (nBarWidth != m_nBarWidth)
    {
        m_nBarWidth = nBarWidth;
        RedrawWindow();
    }
    return (LRESULT)oldPos;
}
LRESULT CODProgress::OnSetRange(WPARAM, LPARAM range)
{
    return OnSetRange32(LOWORD(range), HIWORD(range));
}
LRESULT CODProgress::OnSetRange32(WPARAM Lower, LPARAM Upper)
{
    m_nMax = (int)Upper;
    m_nMin = (int)Lower;    
    return (DWORD)(LOWORD(m_nMax) << 0x10) & DWORD(LOWORD(m_nMin));    
}
LRESULT CODProgress::OnSetStep(WPARAM step, LPARAM)
{
    int oldStep = m_nStepSize;
    m_nStepSize = (int)step;
    return (LRESULT)oldStep;
}
LRESULT CODProgress::OnStepIt(WPARAM, LPARAM) 
{    
   return (LRESULT)SetPos(m_nPos + m_nStepSize);
}
LRESULT CODProgress::OnSetShowText(WPARAM bshow, LPARAM)
{
    LRESULT result = Default();
    SetShowText((BOOL)bshow);
    return result;
}
LRESULT CODProgress::OnSetTextColor(WPARAM, LPARAM cl)
{
    return (LRESULT)setTextColor((COLORREF)cl);
}
BOOL CODProgress::OnEraseBkgnd(CDC*) 
{    
     return 1;
}
void CODProgress::OnSize(UINT nType, int cx, int cy) 
{
    CProgressCtrl::OnSize(nType, cx, cy);    
    m_nBarWidth = -1;  
}
void CODProgress::OnPaint() 
{
    if (m_nMin >= m_nMax) return;
    COLORREF clBar, clBkgnd;

    clBar = (m_clBar == CLR_DEFAULT) ? ::GetSysColor(COLOR_HIGHLIGHT) : m_clBar;
    clBkgnd = (m_clBkgnd == CLR_DEFAULT) ? ::GetSysColor(COLOR_WINDOW) : m_clBkgnd;

    RECT rclient, rleft, rright;
    GetClientRect(&rclient);

    double fraction = (double)(m_nPos - m_nMin) / ((double)(m_nMax - m_nMin));
    CPaintDC paintDC(this); 
    memDC dc(&paintDC);    

    dc.SelectObject(GetParent()->GetFont()); 
    // Draw Bar
    rleft = rright = rclient;

#ifdef PBS_VERTICAL
    DWORD dwStyle = GetStyle();
    if (dwStyle & PBS_VERTICAL)
    {
        rleft.top = rleft.bottom - (int)((rleft.bottom - rleft.top) * fraction);
        rright.bottom = rleft.top;
    }
    else
#endif
    {
        rleft.right = rleft.left + (int)((rleft.right - rleft.left) * fraction);
        rright.left = rleft.right;
    }
    dc.FillSolidRect(&rleft, clBar);
    dc.FillSolidRect(&rright, clBkgnd);

    // Draw Text if not vertical
    if (m_bshowText
#ifdef PBS_VERTICAL
        && (dwStyle & PBS_VERTICAL) == 0
#endif	
	)
    {
        CString line;
        if (m_strText.GetLength()) line.Format("%s %d%%", m_strText, (int)(fraction * 100.0));
        else line.Format("%d%%", (int)(fraction * 100.0));

        dc.SetBkMode(TRANSPARENT);
		
        CFont font, *poldFont = 0;
#ifdef PBS_VERTICAL
        if (dwStyle & PBS_VERTICAL)
        {
            LOGFONT lf;
            GetFont()->GetLogFont(&lf);
            lf.lfEscapement = lf.lfOrientation = 900;
            font.CreateFontIndirect(&lf);
            poldFont = (CFont*)dc.SelectObject(&font);
        }
#endif
        CRgn rgn;
        rgn.CreateRectRgn(rleft.left, rleft.top, rleft.right, rleft.bottom);
        dc.SelectClipRgn(&rgn);
        dc.SetTextColor(m_clText == CLR_DEFAULT ? clBkgnd : m_clText);
        dc.DrawText(line, &rclient, m_dwTextStyle);

        rgn.DeleteObject();
        rgn.CreateRectRgn(rright.left, rright.top, rright.right, rright.bottom);
        dc.SelectClipRgn(&rgn);
        dc.SetTextColor(m_clText == CLR_DEFAULT ? clBar : m_clText);
        dc.DrawText(line, &rclient, m_dwTextStyle);

        if (poldFont)
        {
            dc.SelectObject(poldFont);
            font.DeleteObject();
        }
    }
}
// CODButton
CODButton::CODButton()
{
	m_bchanged = 0;
}
void CODButton::PreSubclassWindow()
{
	ModifyStyle(0, BS_OWNERDRAW);
	CButton::PreSubclassWindow();
}
BEGIN_MESSAGE_MAP(CODButton, CButton)
	ON_CONTROL_REFLECT_EX(BN_CLICKED, OnClickedEx)
END_MESSAGE_MAP()
BOOL CODButton::OnClickedEx()
{
	if (m_bchanged) InvalidateRect(0);
	return 0;
}
// CFButton
CFButton::CFButton()
{
	memset(&m_lf, 0, 60);
}
void CFButton::DrawItem(LPDRAWITEMSTRUCT lpDIS)
{
	CClientDC dc(this); // device context for painting	
	RECT rc;
	GetClientRect(&rc);

	dc.SaveDC();
	dc.FillSolidRect(&rc, ::GetSysColor(COLOR_BTNFACE));
	if (::GetFocus() == m_hWnd)
	{		
		CPen pen(PS_SOLID | PS_COSMETIC, 1, ::GetSysColor(COLOR_BTNTEXT));	// darkshadow		
		
		dc.MoveTo(rc.left, rc.bottom-1);		
		dc.SelectObject(pen);		
		dc.LineTo(rc.left, rc.top);
		dc.LineTo(rc.right - 1, rc.top);		
		dc.LineTo(rc.right - 1, rc.bottom - 1);
		dc.LineTo(rc.left, rc.bottom - 1);	
	}
	dc.RestoreDC(-1);	

	rc.left++;
	rc.top++;
	rc.right--;
	rc.bottom--;	
	//Draw border
	UINT uFrameCtrl = DFCS_BUTTONPUSH;	
	//Draw the frame, but ignore the area inside
	if (lpDIS->itemState & ODS_SELECTED) uFrameCtrl |= DFCS_PUSHED;					
	
	dc.SaveDC();
	dc.DrawFrameControl(&rc, DFC_BUTTON, uFrameCtrl);
	dc.RestoreDC(-1);
	// Draw text	
	CFont font;
	font.CreateFontIndirect(&m_lf);
	
	dc.SetBkMode(TRANSPARENT);	
	dc.SelectObject(&font);
    dc.SetTextColor(m_color);
	dc.DrawText(m_lf.lfFaceName, &rc, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
	dc.RestoreDC(-1);
	font.Detach();
}
BEGIN_MESSAGE_MAP(CFButton, CODButton)
	ON_CONTROL_REFLECT_EX(BN_CLICKED, OnClickedEx)
END_MESSAGE_MAP()
BOOL CFButton::OnClickedEx()
{
	LOGFONT lf;
	memcpy(&lf, &m_lf, 60);
	
	CXFontDlg dlg(&lf);			
	dlg.m_cf.rgbColors = m_color;

	if (IDOK == dlg.DoModal())
	{		
		COLORREF color = dlg.GetColor();
		if (color != m_color) 
		{
			m_bchanged = 1;
			m_color = color;						
		}		
		if (memcmp(&m_lf, &lf, 60) != 0)
		{			
			m_bchanged = 1;
			memcpy(&m_lf, &lf, 60);					
		}	
	}
	return CODButton::OnClickedEx();
}
// CXButton
void CXButton::DrawItem(LPDRAWITEMSTRUCT lpDIS)
{
	CClientDC dc(this); // device context for painting	
	RECT rc;
	GetClientRect(&rc);

	dc.SaveDC();
	dc.FillSolidRect(&rc, ::GetSysColor(COLOR_BTNFACE));
	if (::GetFocus() == m_hWnd)
	{		
		CPen pen(PS_SOLID | PS_COSMETIC, 1, ::GetSysColor(COLOR_BTNTEXT));	// darkshadow		
		
		dc.MoveTo(rc.left, rc.bottom-1);		
		dc.SelectObject(pen);		
		dc.LineTo(rc.left, rc.top);
		dc.LineTo(rc.right - 1, rc.top);		
		dc.LineTo(rc.right - 1, rc.bottom - 1);
		dc.LineTo(rc.left, rc.bottom - 1);	
	}
	dc.RestoreDC(-1);	
	
	rc.left++;
	rc.top++;
	rc.right--;
	rc.bottom--;
	//Draw border
	UINT uFrameCtrl = DFCS_BUTTONPUSH;	
	//Draw the frame, but ignore the area inside
	if (lpDIS->itemState & ODS_SELECTED) uFrameCtrl |= DFCS_PUSHED;					
	
	dc.SaveDC();
	dc.DrawFrameControl(&rc, DFC_BUTTON, uFrameCtrl);
	dc.RestoreDC(-1);		
	//Draw color
	rc.left += 2;
	rc.top += 2;
	rc.right -= 2;
	rc.bottom -= 2;
	dc.FillSolidRect(&rc, m_color);	
}
BEGIN_MESSAGE_MAP(CXButton, CODButton)
	ON_CONTROL_REFLECT_EX(BN_CLICKED, OnClickedEx)
END_MESSAGE_MAP()
BOOL CXButton::OnClickedEx()
{
	CColorDialog dlg(m_color, CC_RGBINIT);		
	cust[0] = m_color;
	dlg.m_cc.lpCustColors = cust;	
	if (IDOK == dlg.DoModal())
	{
		COLORREF color = dlg.GetColor();
		if (color != m_color) 
		{
			m_bchanged = 1;
			m_color = color;
		}
	}
	return CODButton::OnClickedEx();
}
// CXStatic
CXStatic::CXStatic()
{	
	m_bgray = m_binv = 0;
	m_dw = 0;
}
void CXStatic::setColors(DWORD* dw, UCHAR size)
{
	m_dw = dw;
	m_size = size;
}
BEGIN_MESSAGE_MAP(CXStatic, CStatic)
	ON_WM_PAINT()
END_MESSAGE_MAP()
void CXStatic::OnPaint()
{
	CPaintDC dc(this); 
	RECT rcColor, rcFrame;	
	CBrush brColor, brFrame(DWORD(0));	
	
	GetClientRect(&rcFrame);			
	
	dc.SaveDC();
	dc.FrameRect(&rcFrame, &brFrame);		

	BYTE r, g, b, gray;
	UCHAR i, j;	
	
	for (i = 0; i < m_size; i++)
	{
		SetRect(&rcColor, i * (rcFrame.right - rcFrame.left - 2) / m_size, rcFrame.top + 1, 
			(i + 1) * (rcFrame.right - rcFrame.left - 2) / m_size, rcFrame.bottom - 1);
		OffsetRect(&rcColor, 1 + rcFrame.left, 0);
		
		j = m_binv ? m_size - (i + 1) : i;		

		r = (BYTE)((m_dw[j] >> 0x10) & 0xff),
		g = (BYTE)((m_dw[j] >> 0x8) & 0xff),
		b = (BYTE)(m_dw[j] & 0xff); 					
		
		if (m_bgray)
		{
			gray = (BYTE)ROUND(0.3f * r + 0.59f * g + 0.11f * b);
			brColor.CreateSolidBrush(RGB(gray, gray, gray));
		}
		else brColor.CreateSolidBrush(RGB(r, g, b));				
		
		dc.FillRect(&rcColor, &brColor);
		brColor.DeleteObject();				
	}
	dc.RestoreDC(-1);
}
// CXMenu
CXMenu::CXMenu()
{
	itemHeight = 20;
	itemWidth = 140;
}
CXMenu::~CXMenu()
{
	images.Detach();
	items.RemoveAll();
}
bool CXMenu::setTrueColor(UINT nIDResource)
{
	BITMAP	bm;
	CBitmap bitmap;	
	
	if (!bitmap.Attach(LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(nIDResource), 
		IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE | LR_CREATEDIBSECTION))) return 0; 	
	if (!bitmap.GetBitmap(&bm)) return 0;			
	
	if (!images.Attach(ImageList_Create(16, ITEM_HSMALL, ILC_COLORDDB | ILC_MASK, bm.bmWidth / 16, 0)))
		return 0;		
	
	RGBTRIPLE* rgb	= (RGBTRIPLE*)(bm.bmBits);
	COLORREF rgbMask = RGB(rgb[0].rgbtRed, rgb[0].rgbtGreen, rgb[0].rgbtBlue);

	if (images.Add(&bitmap, rgbMask) == -1) return 0;    	
	bitmap.DeleteObject();	
	
	return 1;
}
void CXMenu::AppendMenu(UINT nFlags, UINT_PTR nIDNewItem, int iImage, LPCTSTR lpszNewItem)
{
	menuItem* pItem = 0;
	if (nIDNewItem > 0) 
	{	
		if (iImage != -1) nFlags |= MF_OWNERDRAW;
		pItem = new menuItem(lpszNewItem);
		pItem->index = iImage;		
		items.AddTail(pItem);
	}	
	CMenu::AppendMenu(nFlags, nIDNewItem, (LPCTSTR)pItem);	
}
void CXMenu::DrawItem(LPDRAWITEMSTRUCT lpDIS)
{	
	CDC* pDC = CDC::FromHandle(lpDIS->hDC);
	menuItem* pItem = (menuItem*)lpDIS->itemData;
	UINT state = GetMenuState(lpDIS->itemID, MF_BYCOMMAND);	
	
	CRect r(&lpDIS->rcItem); 	
	// get icon region and text region
	CRect rIcon(r.left, r.top, r.left + 20, r.top + 20),
		  rBmp(rIcon), 	
		  rSel(rIcon.right + 1, r.top, r.right, r.bottom),  
		  rText(rSel.left + 6, rSel.top + 2, rSel.right, rSel.bottom);
	rBmp.DeflateRect(2, 2);

	pDC->SetTextColor(GetSysColor(COLOR_WINDOWTEXT));
	if (lpDIS->itemAction & ODA_DRAWENTIRE)
	{					
		pDC->FillRect(r, &CBrush(GetSysColor(COLOR_3DFACE)));		
		if (state & MFS_DISABLED) 
		{
			pDC->SetTextColor(GetSysColor(COLOR_GRAYTEXT));	
			drawEmbossed(pDC, &images, pItem->index, rBmp);			
		}
		else images.Draw(pDC, pItem->index, rBmp.TopLeft(), ILD_NORMAL);		
	}
	if ((lpDIS->itemState & ODS_SELECTED) && (lpDIS->itemAction & (ODA_SELECT | ODA_DRAWENTIRE)))
	{		// item has been selected		
		if (state & MFS_DISABLED) return;							
		
		CRect rTemp(r);				
		pDC->Draw3dRect(rIcon,GetSysColor(COLOR_BTNHIGHLIGHT), GetSysColor(COLOR_BTNSHADOW)); 
		
		rTemp.CopyRect(&rSel);
		pDC->FillRect(rTemp, &CBrush(GetSysColor(COLOR_HIGHLIGHT)));

		pDC->SetTextColor(GetSysColor(COLOR_HIGHLIGHTTEXT));		
		images.Draw(pDC, pItem->index, rBmp.TopLeft(), ILD_NORMAL);
	}	
	if (!(lpDIS->itemState & ODS_SELECTED) && (lpDIS->itemAction & ODA_SELECT))
	{	// item has been de-selected		
		pDC->FillRect(r, &CBrush(GetSysColor(COLOR_3DFACE)));				
		if (state & MFS_DISABLED) 
		{
			pDC->SetTextColor(GetSysColor(COLOR_GRAYTEXT));
			drawEmbossed(pDC, &images, pItem->index, rBmp);
		}
		else images.Draw(pDC, pItem->index, rBmp.TopLeft(), ILD_NORMAL);		
	}	
	pDC->SetBkMode(TRANSPARENT);		
	pDC->DrawText(pItem->text, rText, DT_LEFT | DT_SINGLELINE);		
}
void CXMenu::MeasureItem(LPMEASUREITEMSTRUCT lpMIS)
{
	lpMIS->itemWidth = itemWidth;
	lpMIS->itemHeight = itemHeight;
}
CXListBox::CXListBox()
{
	m_dw = 0;	
}
void CXListBox::createMenu()
{		
	xmenu.CreatePopupMenu();			
	xmenu.AppendMenu(MF_ENABLED, ID_LINSERT, 3, _T("Вставить"));
	xmenu.AppendMenu(MF_ENABLED, ID_LDELETE, 4, _T("Удалить"));
	xmenu.itemWidth = 80;
	xmenu.setTrueColor(IDB_XMENU);
}
void CXListBox::DrawItem(LPDRAWITEMSTRUCT lpDIS)
{	
	BYTE r = (BYTE)((m_dw[lpDIS->itemID] >> 0x10) & 0xff),
		 g = (BYTE)((m_dw[lpDIS->itemID] >> 0x8) & 0xff),
		 b = (BYTE)(m_dw[lpDIS->itemID] & 0xff);
	
	CDC dc;
    dc.Attach(lpDIS->hDC);
	dc.FillSolidRect(&lpDIS->rcItem, RGB(r, g, b));	
	if (lpDIS->itemState & ODS_FOCUS) dc.DrawFocusRect(&lpDIS->rcItem);	
	dc.Detach();
}
void CXListBox::PreSubclassWindow()
{
	ModifyStyle(0, LBS_OWNERDRAWFIXED);
	CListBox::PreSubclassWindow();
}
void CXListBox::setArr(UCHAR size)
{
	SendMessage(LB_RESETCONTENT);		
	for (UCHAR i = 0; i < size; i++)
	{
		SendMessage(LB_ADDSTRING);
		SendMessage(LB_SETITEMHEIGHT, i, MAKELONG(ITEM_H, 0)); 		
	}	
}
BEGIN_MESSAGE_MAP(CXListBox, CListBox)
	ON_COMMAND(ID_LDELETE, OnDelete)
	ON_COMMAND(ID_LINSERT, OnInsert)
	ON_WM_CONTEXTMENU()
	ON_WM_LBUTTONDBLCLK()	
END_MESSAGE_MAP()
void CXListBox::OnContextMenu(CWnd* pWnd, CPoint point)
{		
	UINT nEnable = (SendMessage(LB_GETCOUNT) > 1) ? MF_ENABLED : MF_DISABLED | MF_GRAYED;	
	xmenu.EnableMenuItem(ID_LDELETE, nEnable);	
	xmenu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this, 0);
}
void CXListBox::OnDelete()
{	
	int count = (int)SendMessage(LB_GETCOUNT),
	    index = (int)SendMessage(LB_GETCURSEL); 		 		 
	index = index < 0 ? 0 : index;
	SendMessage(LB_DELETESTRING, index);		
	
	DWORD* pdw = new DWORD[count];
	memcpy(pdw, m_dw, index << 2);	
	for (UCHAR i = index; i < count; i++)	
	{
		pdw[i] = m_dw[i + 1];
	}	
	SAFE_DELETE_ARRAY(m_dw);
	
	m_dw = new DWORD[count];
	memcpy(m_dw, pdw, count << 2);
	SAFE_DELETE_ARRAY(pdw);	
	SendMessage(WM_VSCROLL);
}
void CXListBox::OnInsert()
{			
	int count = (int)SendMessage(LB_GETCOUNT),
	    index = (int)SendMessage(LB_GETCURSEL);	
	index = index < count ? index : 0;
	SendMessage(LB_INSERTSTRING, index);	
	
	DWORD* pdw = new DWORD[++count];
	memcpy(pdw, m_dw, index << 2);
	for (UCHAR i = ++index; i < count; i++)	
	{
		pdw[i] = m_dw[i - 1];
	}	
	SAFE_DELETE_ARRAY(m_dw);
	
	m_dw = new DWORD[count];
	memcpy(m_dw, pdw, count << 2);
	SAFE_DELETE_ARRAY(pdw);
}
void CXListBox::OnLButtonDblClk(UINT nFlags, CPoint point)
{	
	int index = (int)SendMessage(LB_GETCURSEL, 0, 0);
	BYTE r = (BYTE)((m_dw[index] >> 0x10) & 0xff),
		 g = (BYTE)((m_dw[index] >> 0x8) & 0xff),
		 b = (BYTE)(m_dw[index] & 0xff);

	COLORREF color0 = r | g << 0x8 | b << 0x10, color1;
	CColorDialog dlg(color0, CC_RGBINIT);
	cust[0] = color0;
	dlg.m_cc.lpCustColors = cust;	
	
	if (IDOK == dlg.DoModal())
	{
		color1 = dlg.GetColor();
		if (color0 != color1) 
		{
			r = GetRValue(color1);
			g = GetGValue(color1);
			b = GetBValue(color1);
			m_dw[index] = 0xff000000 | ((r & 0xff) << 0x10) | ((g & 0xff) << 0x8) | (b & 0xff);
			InvalidateRect(0, 0);
		}
	}
}