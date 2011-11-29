// ctrl.h
#pragma once

#include "template.h"

#define ITEM_H				23
#define ITEM_HSMALL			15
#define PADDING				3
// progress messages
#define PBM_SETTEXTCOLOR	WM_USER + 100
#define PBM_SETSHOWTEXT		WM_USER + 101
// CCtrlToolBar
class CCtrlToolBar : public CToolBar
{
public:	
	bool setTrueColor(UINT nIDResource); 
	void dropArrow(UINT nID, UINT nMenu);
	CWnd* InsertControl(CRuntimeClass* pClass, LPCTSTR lpszWindowName, CRect& rect, UINT nID, DWORD dwStyle);
	CWnd* InsertControl(CWnd* pCtrl, CRect& rect, UINT nID);		
// Attributes
private:
	UINT		menuID;
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnDropDown(NMHDR *pNMHDR, LRESULT *pResult);
};
// CODCombo
class CODCombo : public CComboBox
{
	DECLARE_DYNAMIC(CODCombo)
public:	
// Overrides
	void DrawItem(LPDRAWITEMSTRUCT lpDIS);
	void MeasureItem(LPMEASUREITEMSTRUCT);	
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);	
};
// CNSplitter
class CNSplitter : public CSplitterWnd							// null - splitter
{
public:
	CNSplitter();	
// Overrides
	void OnDrawSplitter(CDC* pDC, ESplitType nType, const CRect& rect);		
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnNcHitTest(CPoint point);		
};
// CUEdit
class CUEdit : public CEdit		// unsigned int edit
{
protected:
	LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
};
// CIEdit
class CIEdit : public CUEdit	// int edit
{
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
};
// CFEdit
class CFEdit : public CUEdit	// float edit
{
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
};
// CUFEdit
class CUFEdit : public CUEdit	// unsigned float edit
{
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
};
// memDC
class memDC : public CDC
{
public:    
    memDC(CDC* pDC);
	~memDC();
    memDC* operator->() {return this;}   
    operator memDC*() {return this;}
private:
    BOOL		m_bmemDC;      
	CBitmap		m_bitmap;      
    CBitmap*	m_poldBitmap;  
    CDC*		m_pDC;         
    RECT		m_rc;            
};
// ODPropgress
class CODProgress : public CProgressCtrl
{
public:
	CODProgress();
	BOOL AlignText(DWORD aligment = DT_CENTER);				
	BOOL AlignTextInvalidate(DWORD aligment = DT_CENTER);	
	COLORREF getBarColor() const {return m_clBar;}
	COLORREF getBkgndColor() const {return m_clBkgnd;}
	COLORREF getTextColor() const {return m_clText;}
	COLORREF setBarColor(COLORREF crBar = CLR_DEFAULT);        
	COLORREF setBgndColor(COLORREF crBgnd = CLR_DEFAULT);  	
	COLORREF setTextColor(COLORREF crText = CLR_DEFAULT);
	void SetShowText(BOOL bShow);
protected:
	BOOL     m_bshowText;
	COLORREF m_clBar, m_clBkgnd, m_clText;
    int      m_nBarWidth, m_nMin, m_nMax, m_nPos, m_nStepSize;
    CString  m_strText;        
	DWORD    m_dwTextStyle;
protected:	
	DECLARE_MESSAGE_MAP()
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);	
    afx_msg LRESULT OnSetText(WPARAM, LPARAM szText);
    afx_msg LRESULT OnGetText(WPARAM cchTextMax, LPARAM szText);
    afx_msg LRESULT OnGetPos(WPARAM, LPARAM);
    afx_msg LRESULT OnSetPos(WPARAM newPos, LPARAM);
    afx_msg LRESULT OnStepIt(WPARAM, LPARAM);
    afx_msg LRESULT OnSetRange(WPARAM, LPARAM range);
    afx_msg LRESULT OnSetRange32(WPARAM lower, LPARAM higher);
    afx_msg LRESULT OnGetRange(WPARAM type, LPARAM PBRange);
    afx_msg LRESULT OnOffsetPos(WPARAM incr, LPARAM);
    afx_msg LRESULT OnSetStep(WPARAM step, LPARAM);
    afx_msg LRESULT OnSetBarColor(WPARAM, LPARAM cl);
    afx_msg LRESULT OnSetBkgndColor(WPARAM, LPARAM cl);
    afx_msg LRESULT OnSetShowText(WPARAM bshow, LPARAM);
	afx_msg LRESULT OnSetTextColor(WPARAM, LPARAM cl);    
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);	
};
// ÑODButton	 
class CODButton : public CButton
{
protected:	
	virtual void PreSubclassWindow();
public:
	CODButton();
// Attributes
public:
	bool					m_bchanged;
	COLORREF				m_color;
protected:	
	DECLARE_MESSAGE_MAP()
	afx_msg BOOL OnClickedEx();
};
// CFButton
class CFButton : public CODButton
{
public:
	CFButton();
	void DrawItem(LPDRAWITEMSTRUCT lpDIS);
// Attributes
public:
	LOGFONT					m_lf;
protected:	
	DECLARE_MESSAGE_MAP()
	afx_msg BOOL OnClickedEx();
};
// CXButton
class CXButton : public CODButton
{
public:
	void DrawItem(LPDRAWITEMSTRUCT lpDIS);
protected:	
	DECLARE_MESSAGE_MAP()
	afx_msg BOOL OnClickedEx();
};
// menuItem
struct menuItem
{
	int						index;
	TCHAR					text[32];	
	inline menuItem(LPCTSTR lpszText){lstrcpy(text, lpszText);}
};
// CXMenu
class CXMenu : public CMenu
{
protected:
	void DrawItem(LPDRAWITEMSTRUCT lpDIS);
	void MeasureItem(LPMEASUREITEMSTRUCT lpMIS);
public:
	CXMenu();
	~CXMenu();
	bool setTrueColor(UINT nIDResource);
	void AppendMenu(UINT nFlags, UINT_PTR nIDNewItem = 0, int iImage = -1, LPCTSTR lpszNewItem = 0); 		
// Attributes
private:
	CImageList				images;
	delPtrList<CPtrList, menuItem*>		items;
public:
	UINT					itemHeight, itemWidth;
};
// CXListBox
class CXListBox : public CListBox
{
protected:
// Overrides	
	virtual void DrawItem (LPDRAWITEMSTRUCT lpDIS);
	virtual void PreSubclassWindow();
public:
	CXListBox();	
	void createMenu();
	void setArr(UCHAR size);	
// Attributes	
protected:
	BOOL					bOutside;
	CXMenu					xmenu;
public:		
	DWORD*					m_dw;
protected:
	DECLARE_MESSAGE_MAP()	
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnDelete();
	afx_msg void OnInsert();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);	
};
// CXStatic
class CXStatic : public CStatic
{
public:
	CXStatic();
	inline void setGray(BOOL bgray){m_bgray = bgray;}
	inline void setInv(BOOL binv){m_binv = binv;}
	void setColors(DWORD* dw, UCHAR size);	
// Attributes
private:
	BOOL					m_bgray, m_binv;
	UCHAR					m_size;
	DWORD*					m_dw;
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
};