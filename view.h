// view.h
#pragma once

#define UF_CREATE			0x01 
#define UF_RELEASE			0x02 

#include "doc.h"
// C3dView
class C3dView : public CView
{
	DECLARE_DYNCREATE(C3dView)
private:
	BOOL OnPreparePrinting(CPrintInfo* pInfo);
	HRESULT Create3DDevice(DWORD BehaviorFlags);			
	HRESULT InvalidateDeviceObjects();
	HRESULT	Open3D();	
	HRESULT renderBack();		
	HRESULT renderScene();  	
	HRESULT renderSkeleton();
	HRESULT renderSel();  
	HRESULT renderWell();	
	void CheckZBias();
	void Close3D();
#ifdef _DEBUG	
	void FPS();
#endif
	void RestoreDeviceObjects(); 			
	void TestDeviceState();
protected: 
	C3dView();	
	void OnInitialUpdate();
public:	
	HRESULT Reset3D();		
	LPDIRECT3DSURFACE9 getScene(HBITMAP hbmp);
	void updateFont(UCHAR uf = UF_CREATE);
// Overrides	
	void OnDraw(CDC*);  
	~C3dView();
// Attributes
private:	
	bool					m_bdrag;	
	HRESULT					m_hDeviceState;
	D3DDISPLAYMODE			d3ddm; 
	D3DLOCKED_RECT			lrect;
	D3DPRESENT_PARAMETERS	d3dpp;	
	CFont					dxfont;
	CPoint					pt;
	LPDIRECT3D9				lpdd;	
	LPD3DXFONT				lptxt;	
	LPDIRECT3DTEXTURE9		lptex;	
#ifdef _DEBUG	
	RECT					rcFPS;
	TCHAR					stats[32];
#endif
	CGraph*					pgraph;	
protected:
	DECLARE_MESSAGE_MAP()	
	afx_msg BOOL OnEraseBkgnd(CDC*);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnDestroy(); 
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);		
	afx_msg void OnSize(UINT nType, int cx, int cy); 
};
// CInfoView 
class CInfoView : public CView
{
	DECLARE_DYNCREATE(CInfoView)
private:
	BOOL OnPreparePrinting(CPrintInfo* pInfo);
public:
	CInfoView();
	HBITMAP getSnapShot();
// Overrides	
	void OnDraw(CDC* pDC);
	~CInfoView();
// Attributes
private:
	CFont					m_font;
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};