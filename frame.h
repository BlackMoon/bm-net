// frame.h
#pragma once
#include "ctrl.h"
#include "dlg.h"
#include "view.h"

#define INFOHEIGHT	136;
// CMainFrame
class CMainFrame : public CFrameWnd
{	
	DECLARE_DYNCREATE(CMainFrame)
//private:
public:
	BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	BOOL PreCreateWindow(CREATESTRUCT& cs);	
	void ActivateFrame(int nCmdShow = -1);
	void back();
	void bottom();	
	void cell();
	void first();
	void front();	
	void last();
	void left();
	void next();
	void panel1();
	void panel2();
	void panel3();
	void prev();
	void recalcSplit();
	void reset();
	void right();
	void top();	
	void well();	
protected: 
	CMainFrame();
public:
	void comboAddData(LPCTSTR lpszString, LPVOID lpData = 0, bool broxar = 0); 	
	void comboClearData();
	void dxsave(LPCTSTR pDestFile, D3DXIMAGE_FILEFORMAT DestFormat);
	void infoUpdate();	
	void setParam();
	void showStock(bool brel = 0);		
// Attributes
private:	
	struct bpanel					// control bar embedded members	
	{
		bool				binfo, bleft, btop;
	}		m_bpanel; 
	bool					m_bfirst, m_bsplit, m_bstock;		
	static const CRect		s_rcDefault;   
	static const TCHAR		s_filter[];
	CCtrlToolBar			m_leftbar, m_mainbar, m_topbar;	
	CNSplitter				m_split;
	CXMenu					m_xmenu, m_xpopup;	
	CGraph*					pgraph;
	CInfoView*				m_pinfoview;
	stockDlg*				m_pstockDlg;
public:
	CInfoBar				m_infobar;	
	C3dView*				m_p3dview;	
	CODCombo*				m_pcomboP;
	CODCombo*				m_pcomboD;		
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);			
	afx_msg LRESULT OnStockClosed(WPARAM wParam, LPARAM lParam);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnDataSelchange();		
	afx_msg void OnDestroy();	
	afx_msg void OnFileCopy(); 		
	afx_msg void OnFileSave();		
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMIS);
	afx_msg void OnParamSelchange();	
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnStock();
	afx_msg void OnUpdateCell(CCmdUI* pCmdUI);	
	afx_msg void OnUpdateNext(CCmdUI* pCmdUI);	
	afx_msg void OnUpdatePrev(CCmdUI* pCmdUI);	
	afx_msg void OnUpdateStock(CCmdUI* pCmdUI);	
	afx_msg void OnUpdateWell(CCmdUI* pCmdUI);		
};