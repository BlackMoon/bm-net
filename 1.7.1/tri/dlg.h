// dlg.h
#pragma once
#include "ctrl.h"
#include "graph.h"
#include "vsflex.h"
#include "resource.h"
// message ID to sheet
#define ID_GENERAL_CHANGED	WM_USER + 101
#define ID_INFO_CHANGED		WM_USER + 102
#define ID_LEGEND_CHANGED	WM_USER + 103
#define ID_WELL_CHANGED		WM_USER + 104
// message ID to frame
#define ID_STOCK_CLOSED		WM_USER + 105
// update options need
#define UO_NONE				0x00
#define UO_GENERAL			0x01
#define UO_LEGEND			0x02
#define UO_SKEL				0x04
#define UO_ZCOEF			0x08
#define UO_INFO				0x10
#define UO_WELL				0x20							
#define UO_WFONT			0x40							
// g_set
struct g_set
{
	BOOL					bauto;	
	COLORREF				cl_lfont;				
	DWORD					dw_bkgnd, dw_cell;		
	FLOAT					zcoef;
	inline g_set()
	{
		bauto = 0;
		cl_lfont = 0x00;
		dw_bkgnd = 0xffffffff;
		dw_cell = 0xff000000;
		zcoef = 32.0f;
	}
	g_set& operator = (const g_set& g)
	{
		bauto = g.bauto;
		cl_lfont = g.cl_lfont;
		dw_bkgnd = g.dw_bkgnd;
		dw_cell = g.dw_cell;
		zcoef = g.zcoef;
		return *this;
	}
};
struct w_set
{		
	DWORD					dw_font, dw_main, dw_sel;
	LONG					wxo, wyo;	
	UINT					wh, wi, wr1, wr2;	
	D3DXFONT_DESC			ds;
	inline w_set()
	{
		dw_font = dw_main = 0xff000000;
		dw_sel = 0xffc0c0c0;
		wxo = wyo = 0;
		wh = 100;
		wi = 10;
		wr1 = 50;
		wr2 = 10;		
		
		ds.CharSet = RUSSIAN_CHARSET;
		ds.Height = 14;
		lstrcpy(ds.FaceName, "Tahoma");	
	}
	w_set& operator = (const w_set& w)
	{
		dw_font = w.dw_font;
		dw_main = w.dw_main;
		dw_sel = w.dw_sel;
		memcpy(&ds, &w.ds, 56);

		wh = w.wh;
		wi = w.wi;
		wr1 = w.wr1;
		wr2 = w.wr2;
		wxo = w.wxo;
		wyo = w.wyo;
		return *this;
	}
};
struct storage
{
	c_set					_c_set;
	g_set					_g_set;	
	w_set					_w_set;		
};
// CFilterDlg
class CFilterDlg : public CFileDialog
{
private:
	void OnTypeChange();
public: 
	CFilterDlg(BOOL bOpenFileDialog, LPCTSTR lpszDefExt = 0, LPCTSTR lpszFileName = 0,
		DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,	LPCTSTR lpszFilter = 0, 
		CWnd* pParentWnd = 0);
// Attributes
public:
	D3DXIMAGE_FILEFORMAT	dxFormat;
};
// CXFileDlg
class CXFileDlg : public CFileDialog
{
public:
	CXFileDlg(BOOL bOpenFileDialog, LPCTSTR lpszDefExt = "mvf", LPCTSTR lpszFileName = 0,
			  DWORD dwFlags = 0, 
			  LPCTSTR lpszFilter = "Файлы модели (*.mvf)|*.mvf|", CWnd* pParentWnd = 0);
// Overrides	
	BOOL OnInitDialog();	
};
// CXFontDlg
class CXFontDlg : public CFontDialog
{
public:	
	CXFontDlg(LPLOGFONT lplfInitial = 0, DWORD dwFlags = CF_EFFECTS | CF_SCREENFONTS,
			 CDC* pdcPrinter = 0, CWnd* pParentWnd = 0); 
// Overrides	
	BOOL OnInitDialog();	
};
// CInfoBar
class CInfoBar : public CDialogBar
{
protected:
	BOOL PreTranslateMessage(MSG* pMsg);	
	void DoDataExchange(CDataExchange* pDX);
public:
	CInfoBar();
	BOOL Create(CWnd* pParentWnd);	
	void setFilter();	
// Attributes
private:	
	CIEdit					m_editX, m_editY, m_editZ;
	CGraph*					pgraph;
public:
	CODProgress				m_progress;
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnClickReset();		
	afx_msg void OnXSmaller();
	afx_msg void OnXBigger();
	afx_msg void OnYSmaller();
	afx_msg void OnYBigger();
	afx_msg void OnZSmaller();
	afx_msg void OnZBigger();
	afx_msg void OnUpdateReset(CCmdUI* pCmdUI); 	
	afx_msg void OnUpdateXSmall(CCmdUI* pCmdUI); 
	afx_msg void OnUpdateXBig(CCmdUI* pCmdUI); 
	afx_msg void OnUpdateYSmall(CCmdUI* pCmdUI); 
	afx_msg void OnUpdateYBig(CCmdUI* pCmdUI); 
	afx_msg void OnUpdateZSmall(CCmdUI* pCmdUI); 
	afx_msg void OnUpdateZBig(CCmdUI* pCmdUI); 
};
// legendDlg
class legendDlg : public CDialog
{
private:
	BOOL OnInitDialog();
	void DoDataExchange(CDataExchange* pDX);	
public:
	legendDlg(DWORD* dw, UCHAR size);
	~legendDlg();	
// Attributes
private:	
	CXListBox				m_list;
public:
	enum {IDD = IDD_LEGEND};	
	DWORD*					m_dw;
	int						m_size;
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnDestroy();
};
// xPage
class xPage : public CPropertyPage
{
protected:
	virtual BOOL OnInitDialog();
	virtual void getData() = 0;
public:
	xPage(UINT nIDTemplate, UINT nIDCaption = 0);	
// Attributes
protected:
	CWnd*				parent;
};
// generalPage
class generalPage : public xPage
{
private:
	BOOL OnInitDialog();		
	void DoDataExchange(CDataExchange* pDX);
public:
	generalPage();	
	~generalPage();	
	void getData();
// Attributes
private:
	CFEdit					m_editZ;
	CXButton				m_bkgnd, m_cell, m_lfont;
	CXStatic				m_lstatic;
public:
	enum {IDD = IDD_GENERAL};	
	c_set					_c_set;
	g_set					_g_set;
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnChangeZ();
	afx_msg void OnClickedAuto();
	afx_msg void OnClickedBkgnd();
	afx_msg void OnClickedCell();
	afx_msg void OnClickedGray();
	afx_msg void OnClickedInv();
	afx_msg void OnClickedLegend();
	afx_msg void OnClickedLFont();
};
// wellPage
class wellPage : public xPage
{
private:
	BOOL OnInitDialog();
	void DoDataExchange(CDataExchange* pDX);
public:
	wellPage();	
	~wellPage();
	void getData();
// Attributes
private:	
	CIEdit					m_editXO, m_editYO;
	CUEdit					m_editH, m_editI, m_editR1, m_editR2;
	CFButton				m_wfont;
	CXButton				m_wmain, m_wsel;	
public:
	enum {IDD = IDD_WELL};	
	w_set					_w_set;
protected:
	DECLARE_MESSAGE_MAP()	
	afx_msg void OnChangeWell();	
	afx_msg void OnClickedWSel();
	afx_msg void OnClickedWFont();
	afx_msg void OnClickedWMain();
};
// highColorSheet
class highColorSheet : public CPropertySheet
{
private:
	BOOL OnInitDialog();
public:	
	highColorSheet(UINT nIDCaption, CWnd* pParentWnd = 0, UINT iSelectPage = 0);	
	INT_PTR DoModal();
// Attributes
private:	
	CStatic					m_istatic, m_tstatic;
	generalPage				m_gpage;
	wellPage				m_wpage;
public:
	UCHAR					u_opt;
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);			
	afx_msg void OnDestroy();
	afx_msg LRESULT OnGeneralChanged(WPARAM wParam, LPARAM);
	afx_msg LRESULT OnInfoChanged(WPARAM wParam, LPARAM);
	afx_msg LRESULT OnLegendChanged(WPARAM wParam, LPARAM);
	afx_msg LRESULT OnWellChanged(WPARAM wParam, LPARAM);
};
// toolDlg
class toolDlg : public CDialog
{
protected:
	bool getDims();	
	void CopyCell();	
	void flexReload();
	void resetFilter();	
	void SelAll();
	void setFilter();
	virtual void SaveCell() = 0;	
public:
	toolDlg(UINT nIDTemplate, CWnd* pParentWnd = 0);
	virtual BOOL OnInitDialog();
	virtual ~toolDlg();	
// Attrinutes
protected:	
	long					m_cols, m_rows;
	CIEdit					m_editX, m_editY, m_editZ;
	CODProgress				m_progr;
	CXMenu					m_xmenu;	
	xyzDim					toolDim;
	vsflex					m_vsflex;
protected:
	DECLARE_MESSAGE_MAP()	
	DECLARE_EVENTSINK_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);	
	afx_msg void OnMouseUpFlex(short Button, short Shift, float X, float Y); 
};
// statDlg
class statDlg : public toolDlg
{
private:					
	bool analise(const char* filename);	
	bool analiseDiv(const char* filename);	
	bool analiseRox(const char* filename);		
	BOOL OnInitDialog();	
	void DoDataExchange(CDataExchange* pDX);					
	void fillNullRow();		
	void minmax();			
	void AddRow();	
	void RemoveRow();
	void SaveCell();	
public:	
	statDlg();
	~statDlg();	
// Attributes
private:			
	struct stat
	{
		float				x0, x1;
		long				count;
		inline stat() {count = 0;}
	};	
	bool					bactive, bdiv, bread;		
	double					m_fstep;
	float					m_faver, m_fmin, m_fmax, m_fprc;
	long					m_row, m_total;			
	UINT					m_cy, m_size;	
	CComboBox				m_comboP, m_comboD0, m_comboD1;		
	CUFEdit					m_estep;	
	SIZE					sz;		
	bool*					pbarr;	
	std::vector<stat>		stats;
public:
	enum {IDD = IDD_STAT};	
protected:
	DECLARE_MESSAGE_MAP()	
	DECLARE_EVENTSINK_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);		
	afx_msg void OnClickedActive();
	afx_msg void OnClickedAnalise();
	afx_msg void OnClickedDiv();
	afx_msg void OnClickedFill();	
	afx_msg void OnClickedReset();
	afx_msg void OnClickedRestrict();
	afx_msg void OnClickedTurn();	
	afx_msg void OnDate0Selchange();	
	afx_msg void OnDate1Selchange();		
	afx_msg void OnEnterCellFlex();		
	afx_msg void OnKeyPressEditFlex(long Row, long Col, short FAR* KeyAscii); 
	afx_msg void OnMouseUpFlex(short Button, short Shift, float X, float Y); 	
	afx_msg void OnParamSelchange();	
};
// stockDlg
class stockDlg : public toolDlg
{
private:
	bool calc(const char* filename);			
	void DoDataExchange(CDataExchange* pDX);	
	void SaveCell();
protected:
	void OnCancel();
public:	
	stockDlg();
	BOOL Create(UINT nIDTemplate, CWnd* pParentWnd = 0);
	void reload();
	void setWindow(const RECT& rect);
// Attributes
private:
	struct stock
	{
		float				area, oil; 		
	};
	float					m_ftotal;
	std::vector<stock>		stocks;	
public:
	enum {IDD = IDD_STOCK};	
	static const CRect		s_rcDefault;   
protected:
	DECLARE_MESSAGE_MAP()	
	DECLARE_EVENTSINK_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);	
	afx_msg void OnClickedReset();
	afx_msg void OnClickedStock();
	afx_msg void OnMouseUpFlex(short Button, short Shift, float X, float Y); 
};