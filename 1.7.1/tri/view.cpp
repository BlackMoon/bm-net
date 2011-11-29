// view.cpp
#include "stdafx.h"
#include "view.h"

bool						bmove = 0;

extern	char				_log[MAX_PATH], msg[MAX_PATH];
extern	UCHAR				surf;
extern	D3DCAPS9			dcaps;
extern	SIZE				szParam;
extern	xyzDim				_xyzDim;
extern	LPD3DXFONT			lpfont;					
extern	LPDIRECT3DDEVICE9	lpd3d;					
extern	LPDIRECT3DSURFACE9	lpsurf;	
inline DWORD F2DW(FLOAT f) {return *((DWORD*)&f);}
// C3dView
IMPLEMENT_DYNCREATE(C3dView, CView)
C3dView::C3dView()
{
	lpdd = 0;			// Just clear the data.
	lptex = 0;
	lptxt = 0;

	m_bdrag = 0;
	m_hDeviceState = D3DERR_INVALIDDEVICE;					

#ifdef _DEBUG	
	SetRect(&rcFPS, 2, 2, 128, 16);
#endif	
	pgraph = &app.m_graph;		
}
C3dView::~C3dView()
{
	dxfont.Detach();
	Close3D();
}
BOOL C3dView::OnPreparePrinting(CPrintInfo* pInfo)
{
	return DoPreparePrinting(pInfo);
}
HRESULT C3dView::Create3DDevice(DWORD BehaviorFlags)
{		
	memset(&d3dpp, 0, 56);
	d3dpp.Windowed = 1;    
	d3dpp.hDeviceWindow = m_hWnd;			
	d3dpp.BackBufferFormat = d3ddm.Format;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;	
	d3dpp.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
	// Check for z-buffer support
	HRESULT hres;
	if SUCCEEDED(lpdd->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, d3ddm.Format, 
		D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D16))
	{
		d3dpp.EnableAutoDepthStencil = 1;
		d3dpp.AutoDepthStencilFormat = D3DFMT_D16;		
	}
	else d3dpp.EnableAutoDepthStencil = 0;		

	if FAILED(hres = lpdd->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_hWnd, 
			  BehaviorFlags, &d3dpp, &lpd3d)) return hres;  						
	
	updateFont();
	D3DXCreateTextureFromResource(lpd3d, 0, MAKEINTRESOURCE(RC_TEX), &lptex);
	D3DXCreateFont(lpd3d, 12, 0, 0, 0, 0,  DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, 
		           DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Tahoma"), &lptxt);		

	return S_OK;	
}
HRESULT C3dView::InvalidateDeviceObjects()
{	
	HRESULT hres;
	if FAILED(hres = lptxt->OnLostDevice()) return hres;
	if FAILED(hres = lpfont->OnLostDevice()) return hres;	
	return S_OK;
}
HRESULT C3dView::Open3D()
{			
	HRESULT hres = E_FAIL;
	if (lpdd) return hres;	    

	try
	{		
		lpdd = Direct3DCreate9(D3D_SDK_VERSION);
		if (lpdd == 0) 
		{			
			memset(msg, 0, MAX_PATH);
			sprintf_s(msg, MAX_PATH, "Создание Direct3D завершилось с кодом 0x%x", hres);
			throw msg;
		}		
		if FAILED(hres = lpdd->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm)) 
		{
			memset(msg, 0, MAX_PATH);
			sprintf_s(msg, MAX_PATH, "GetAdapterDisplayMode() завершилось с кодом 0x%x", hres);				
			throw msg;
		}				
		lpdd->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &dcaps);			
		// Test which is supported, hardware or software vertex processing.
		DWORD VertexProcessing = 0;
		if (dcaps.VertexProcessingCaps != 0) VertexProcessing |= D3DCREATE_HARDWARE_VERTEXPROCESSING;
		else VertexProcessing |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;		
		
		if FAILED(hres = Create3DDevice(VertexProcessing)) 
		{
			memset(msg, 0, MAX_PATH);
			sprintf_s(msg, MAX_PATH, "Создание 3D устройства (HAL) завершилось с кодом 0x%x", hres);
			throw msg;					
		};
		CheckZBias();
		TestDeviceState();
		if SUCCEEDED(m_hDeviceState) RestoreDeviceObjects();		
	}
	catch (char*)
	{
		writelog(msg, _log);
		MessageBox(msg, app.m_pszAppName, MB_ICONSTOP);		
	}			
	return hres; 
}
HRESULT C3dView::renderBack()
{
	lpd3d->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_FLAT);	

	HRESULT hres;	
	int i;
	UINT j, j1, nbuff, nstartIndex;
	CDXSlice* pdxSlice;		

	for (i = _xyzDim.zDim.cx; i < _xyzDim.zDim.cy; i++)
	{
		pdxSlice = pgraph->m_dxSlices[i];
		for (nbuff = 0; nbuff < pdxSlice->m_buffSize; nbuff++)		
		{	
			j1 = pgraph->m_rows[nbuff];						
			// top			
			if ((pdxSlice->m_bdraw.bdrawTopNeeded) && (surf & VS_TOP))
			{									
				lpd3d->SetIndices(pdxSlice->m_ptopBuffer[nbuff].ib);
				lpd3d->SetStreamSource(0, pdxSlice->m_ptopRBuffer[nbuff].vb, 0, 16); 						
				nstartIndex = pdxSlice->m_desc[0].startIndex;						
				
				for (j = 0; j < j1; j++)
				{
					if FAILED(hres = lpd3d->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, 
						pdxSlice->m_ptopRBuffer[nbuff].vlength, nstartIndex, 
						pdxSlice->m_desc[0].primitiveCount)) return hres;
					nstartIndex += pdxSlice->m_desc[0].primitiveCount + 2;
				}
			}			
			// bottom
			if ((pdxSlice->m_bdraw.bdrawBottomNeeded) && (surf & VS_BOTTOM))
			{					
				lpd3d->SetIndices(pdxSlice->m_pbottomBuffer[nbuff].ib);
				lpd3d->SetStreamSource(0, pdxSlice->m_pbottomRBuffer[nbuff].vb, 0, 16);		
				nstartIndex = pdxSlice->m_desc[0].startIndex;						
				
				for (j = 0; j < j1; j++)
				{
					if FAILED(hres = lpd3d->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, 
						pdxSlice->m_pbottomRBuffer[nbuff].vlength, nstartIndex, 
						pdxSlice->m_desc[0].primitiveCount)) return hres;
					nstartIndex += pdxSlice->m_desc[0].primitiveCount + 2;
				}
			}
		}			
		// sides
		lpd3d->SetIndices(pdxSlice->m_sideBuffer.ib);
		lpd3d->SetStreamSource(0, pdxSlice->m_sideRBuffer.vb, 0, 16);
		// left
		if (surf & VS_LEFT)
		{
			nstartIndex = pdxSlice->m_desc[1].startIndex;		
			if FAILED(hres = lpd3d->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0,
				pdxSlice->m_sideRBuffer.vlength,
				nstartIndex, pdxSlice->m_desc[1].primitiveCount)) return hres;
			nstartIndex += pdxSlice->m_desc[1].primitiveCount + 2;
		}
		// right		
		if (surf & VS_RIGHT)
		{
			nstartIndex = pdxSlice->m_desc[2].startIndex;
			if FAILED(hres = lpd3d->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, 
				pdxSlice->m_sideRBuffer.vlength,
				nstartIndex, pdxSlice->m_desc[2].primitiveCount)) return hres;
			nstartIndex += pdxSlice->m_desc[2].primitiveCount + 2;
		}
		// front
		if (surf & VS_FRONT)
		{
			nstartIndex = pdxSlice->m_desc[3].startIndex;
			if FAILED(hres = lpd3d->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, 
				pdxSlice->m_sideRBuffer.vlength, 
				nstartIndex, pdxSlice->m_desc[3].primitiveCount)) return hres;
			nstartIndex += pdxSlice->m_desc[3].primitiveCount + 2;
		}
		// back
		if (surf & VS_BACK)
		{
			nstartIndex = pdxSlice->m_desc[4].startIndex;
			if FAILED(hres = lpd3d->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, 
				pdxSlice->m_sideRBuffer.vlength,  
				nstartIndex, pdxSlice->m_desc[4].primitiveCount)) return hres;
			nstartIndex += pdxSlice->m_desc[4].primitiveCount + 2;
		}		
	}
	return S_OK;
}
HRESULT C3dView::renderScene()
{		
	lpd3d->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);		
	
	HRESULT hres;	
	int i;
	UINT j, j1, nbuff, nstartIndex;		
	CDXSlice* pdxSlice;		
	for (i = _xyzDim.zDim.cx; i < _xyzDim.zDim.cy; i++)
	{		
		pdxSlice = pgraph->m_dxSlices[i];		
		for (nbuff = 0; nbuff < pdxSlice->m_buffSize; nbuff++)		
		{	
			j1 = pgraph->m_rows[nbuff];								
			// top			
			if ((pdxSlice->m_bdraw.bdrawTopNeeded) && (surf & VS_TOP))
			{								
				lpd3d->SetIndices(pdxSlice->m_ptopBuffer[nbuff].ib);
				lpd3d->SetStreamSource(0, pdxSlice->m_ptopBuffer[nbuff].vb, 0, 16);		
				nstartIndex = pdxSlice->m_desc[0].startIndex;										
				for (j = 0; j < j1; j++)
				{
					if FAILED(hres = lpd3d->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0,
						pdxSlice->m_ptopBuffer[nbuff].vlength, nstartIndex, 
						pdxSlice->m_desc[0].primitiveCount)) return hres;
					nstartIndex += pdxSlice->m_desc[0].primitiveCount + 2;
				}
			}
			// bottom
			if ((pdxSlice->m_bdraw.bdrawBottomNeeded) && (surf & VS_BOTTOM))
			{					
				lpd3d->SetIndices(pdxSlice->m_pbottomBuffer[nbuff].ib);
				lpd3d->SetStreamSource(0, pdxSlice->m_pbottomBuffer[nbuff].vb, 0, 16);		
				nstartIndex = pdxSlice->m_desc[0].startIndex;						
			
				for (j = 0; j < j1; j++)
				{
					if FAILED(hres = lpd3d->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, 
						pdxSlice->m_pbottomBuffer[nbuff].vlength, nstartIndex, 
						pdxSlice->m_desc[0].primitiveCount)) return hres;
					nstartIndex += pdxSlice->m_desc[0].primitiveCount + 2;
				}				
			}			
		}
		// sides
		lpd3d->SetIndices(pdxSlice->m_sideBuffer.ib);
		lpd3d->SetStreamSource(0, pdxSlice->m_sideBuffer.vb, 0, 16);
		// left
		if (surf & VS_LEFT)
		{
			nstartIndex = pdxSlice->m_desc[1].startIndex;		
			if FAILED(hres = lpd3d->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, 
				pdxSlice->m_sideBuffer.vlength, nstartIndex, 
				pdxSlice->m_desc[1].primitiveCount)) return hres;
			nstartIndex += pdxSlice->m_desc[1].primitiveCount + 2;
		}
		// right		
		if (surf & VS_RIGHT)
		{
			nstartIndex = pdxSlice->m_desc[2].startIndex;
			if FAILED(hres = lpd3d->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, 
				pdxSlice->m_sideBuffer.vlength, nstartIndex, 
				pdxSlice->m_desc[2].primitiveCount)) return hres;
			nstartIndex += pdxSlice->m_desc[2].primitiveCount + 2;
		}
		// front
		if (surf & VS_FRONT)
		{
			nstartIndex = pdxSlice->m_desc[3].startIndex;
			if FAILED(hres = lpd3d->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, 
				pdxSlice->m_sideBuffer.vlength, nstartIndex, 
				pdxSlice->m_desc[3].primitiveCount)) return hres;
			nstartIndex += pdxSlice->m_desc[3].primitiveCount + 2;
		}
		// back
		if (surf & VS_BACK)
		{
			nstartIndex = pdxSlice->m_desc[4].startIndex;
			if FAILED(hres = lpd3d->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, 
				pdxSlice->m_sideBuffer.vlength, nstartIndex, 
				pdxSlice->m_desc[4].primitiveCount)) return hres;
			nstartIndex += pdxSlice->m_desc[4].primitiveCount + 2;
		}		
	}
	return S_OK;
}
HRESULT C3dView::renderSkeleton()
{		
	lpd3d->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, F2DW(1.0f));			

	HRESULT hres;	
	int i, j, j1;
	UINT nbuff, nstartIndex;		
	CDXSlice* pdxSlice;	

	for (i = _xyzDim.zDim.cx; i < _xyzDim.zDim.cy; i++)
	{		
		pdxSlice = pgraph->m_dxSlices[i];	
		for (nbuff = 0; nbuff < pdxSlice->m_buffSize; nbuff++)		
		{	
			j1 = pgraph->m_rows[nbuff];								
			// top			
			if ((pdxSlice->m_bdraw.bdrawTopNeeded) && (surf & VS_TOP))
			{								
				lpd3d->SetIndices(pdxSlice->m_piSBuffer[nbuff].ib);															
				lpd3d->SetStreamSource(0, pdxSlice->m_ptopSBuffer[nbuff].vb, 0, 16);										
				
			
				nstartIndex = pdxSlice->m_sdesc[0].startIndex;						
				for (j = 0; j < j1 + 1; j++)
				{
					if FAILED(hres = lpd3d->DrawIndexedPrimitive(D3DPT_LINESTRIP, 0, 0, 
						pdxSlice->m_ptopSBuffer[nbuff].vlength,	nstartIndex, 
						pdxSlice->m_sdesc[0].primitiveCount)) return hres;
					nstartIndex += pdxSlice->m_sdesc[0].primitiveCount + 1;
				}					
				// additional indices  
				lpd3d->SetIndices(pdxSlice->m_paddiSBuffer[nbuff].ib);			
				nstartIndex = pdxSlice->m_paddiSBuffer[nbuff].startIndex;			
				for (j = 0; j < _xyzDim.xLen() + 1; j++)
				{
					if FAILED(hres = lpd3d->DrawIndexedPrimitive(D3DPT_LINESTRIP, 0, 0, 
						pdxSlice->m_ptopSBuffer[nbuff].vlength, nstartIndex, 
						pdxSlice->m_paddiSBuffer[nbuff].primitiveCount)) return hres;
					nstartIndex += pdxSlice->m_paddiSBuffer[nbuff].primitiveCount + 1; 					
				}								
			}
			// bottom
			if ((pdxSlice->m_bdraw.bdrawBottomNeeded) && (surf & VS_BOTTOM))
			{					
				lpd3d->SetIndices(pdxSlice->m_piSBuffer[nbuff].ib);										
				lpd3d->SetStreamSource(0, pdxSlice->m_pbottomSBuffer[nbuff].vb, 0, 16);														
			
				nstartIndex = pdxSlice->m_sdesc[0].startIndex;						
				for (j = 0; j < j1 + 1; j++)
				{
					if FAILED(hres = lpd3d->DrawIndexedPrimitive(D3DPT_LINESTRIP, 0, 0, 
						pdxSlice->m_pbottomSBuffer[nbuff].vlength,	nstartIndex, 
						pdxSlice->m_sdesc[0].primitiveCount)) return hres;
					nstartIndex += pdxSlice->m_sdesc[0].primitiveCount + 1;
				}					
				// additional indices  
				lpd3d->SetIndices(pdxSlice->m_paddiSBuffer[nbuff].ib);			
				nstartIndex = pdxSlice->m_paddiSBuffer[nbuff].startIndex;			
				for (j = 0; j < _xyzDim.xLen() + 1; j++)
				{
					if FAILED(hres = lpd3d->DrawIndexedPrimitive(D3DPT_LINESTRIP, 0, 0, 
						pdxSlice->m_pbottomSBuffer[nbuff].vlength, nstartIndex, 
						pdxSlice->m_paddiSBuffer[nbuff].primitiveCount)) return hres;
					nstartIndex += pdxSlice->m_paddiSBuffer[nbuff].primitiveCount + 1; 									
				}
			}			
		}		
		// sides
		j1 = (_xyzDim.xLen() + _xyzDim.yLen() + 2) << 2;
		lpd3d->SetIndices(pdxSlice->m_sideSBuffer.ib);			
		lpd3d->SetStreamSource(0, pdxSlice->m_sideSBuffer.vb, 0, 16);				
		// left
		if (surf & VS_LEFT)
		{				
			nstartIndex = pdxSlice->m_sdesc[1].startIndex;		
			for (j = _xyzDim.yDim.cx; j < _xyzDim.yDim.cy + 1; j++)
			{
				if FAILED(hres = lpd3d->DrawIndexedPrimitive(D3DPT_LINESTRIP, 0, 0, 
					pdxSlice->m_sideSBuffer.vlength, nstartIndex, 
					pdxSlice->m_sdesc[1].primitiveCount)) return hres;
				nstartIndex += pdxSlice->m_sdesc[1].primitiveCount + 1;
			}
			if (!pdxSlice->m_bdraw.bdrawBottomNeeded)
			{
				nstartIndex = j1;
				if FAILED(hres = lpd3d->DrawIndexedPrimitive(D3DPT_LINESTRIP, 0, 0, 
					pdxSlice->m_sideSBuffer.vlength, nstartIndex, 
					_xyzDim.yLen())) return hres;				
			}
		}
		// right		
		if (surf & VS_RIGHT)
		{
			nstartIndex = pdxSlice->m_sdesc[2].startIndex;
			for (j = _xyzDim.yDim.cx; j < _xyzDim.yDim.cy + 1; j++)
			{
				if FAILED(hres = lpd3d->DrawIndexedPrimitive(D3DPT_LINESTRIP, 0, 0, 
					pdxSlice->m_sideSBuffer.vlength, nstartIndex, 
					pdxSlice->m_sdesc[2].primitiveCount)) return hres;
				nstartIndex += pdxSlice->m_sdesc[2].primitiveCount + 1;
			}
			if (!pdxSlice->m_bdraw.bdrawBottomNeeded)
			{
				nstartIndex = j1 + _xyzDim.yLen() + 1;
				if FAILED(hres = lpd3d->DrawIndexedPrimitive(D3DPT_LINESTRIP, 0, 0, 
					pdxSlice->m_sideSBuffer.vlength, nstartIndex, _xyzDim.yLen())) return hres;					
			}
		}
		// front
		if (surf & VS_FRONT)
		{
			nstartIndex = pdxSlice->m_sdesc[3].startIndex;
			for (j = _xyzDim.xDim.cx; j < _xyzDim.xDim.cy + 1; j++)
			{
				if FAILED(hres = lpd3d->DrawIndexedPrimitive(D3DPT_LINESTRIP, 0, 0, 
					pdxSlice->m_sideSBuffer.vlength, nstartIndex, 
					pdxSlice->m_sdesc[3].primitiveCount)) return hres;
				nstartIndex += pdxSlice->m_sdesc[3].primitiveCount + 1;
			}
			if (!pdxSlice->m_bdraw.bdrawBottomNeeded)
			{
				nstartIndex = j1 + ((_xyzDim.yLen() + 1) << 1);
				if FAILED(hres = lpd3d->DrawIndexedPrimitive(D3DPT_LINESTRIP, 0, 0, 
					pdxSlice->m_sideSBuffer.vlength, nstartIndex, _xyzDim.xLen())) return hres;					
			}
		}
		// back
		if (surf & VS_BACK)
		{
			nstartIndex = pdxSlice->m_sdesc[4].startIndex;
			for (j = _xyzDim.xDim.cx; j < _xyzDim.xDim.cy + 1; j++)
			{
				if FAILED(hres = lpd3d->DrawIndexedPrimitive(D3DPT_LINESTRIP, 0, 0, 
					pdxSlice->m_sideSBuffer.vlength, nstartIndex, 
					pdxSlice->m_sdesc[4].primitiveCount)) return hres;
				nstartIndex += pdxSlice->m_sdesc[4].primitiveCount + 1;
			}
			if (!pdxSlice->m_bdraw.bdrawBottomNeeded)
			{
				nstartIndex = j1 + ((_xyzDim.yLen() + 1) << 1) + (_xyzDim.xLen() + 1);
				if FAILED(hres = lpd3d->DrawIndexedPrimitive(D3DPT_LINESTRIP, 0, 0, 
					pdxSlice->m_sideSBuffer.vlength, nstartIndex, _xyzDim.xLen())) return hres;					
			}
		}				
	}		
	return S_OK;
}
HRESULT C3dView::renderWell()
{	
	lpd3d->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_FLAT);	
	lpd3d->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, F2DW(0.0f));			
	lpd3d->SetRenderState(D3DRS_DEPTHBIAS, F2DW(0.0f));
	
	HRESULT hres;	
	CDXWell* pdxWell;
	for (int i = 0; i < pgraph->m_nwells; i++)
	{
		pdxWell = pgraph->m_dxWells[i];
		if FAILED(hres = pdxWell->render()) return hres;									
	}	
	return S_OK;
}
HRESULT C3dView::renderSel()
{	
	lpd3d->SetRenderState(D3DRS_ALPHABLENDENABLE, 1);
    lpd3d->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	lpd3d->SetStreamSource(0, pgraph->m_selBuffer.vb, 0, 20);
	HRESULT hres = lpd3d->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0,
											   TEX_VERTEX_NUM, 0, 2);		
	lpd3d->SetRenderState(D3DRS_ALPHABLENDENABLE, 0);
	
	if FAILED(hres) return hres;    
	return S_OK;
}
HRESULT C3dView::Reset3D()
{	
	RECT rc;
	GetClientRect(&rc);	
	pgraph->m_frustrum = rc;

	d3dpp.BackBufferWidth = rc.right - rc.left;
    d3dpp.BackBufferHeight = rc.bottom - rc.top;	

	SAFE_RELEASE(lpsurf);	
	if (!lpd3d) return D3DERR_INVALIDDEVICE;		
	
	HRESULT hres; 	
	if FAILED(hres = InvalidateDeviceObjects()) return hres;			
	if FAILED(hres = lpd3d->Reset(&d3dpp)) return hres;      		
	RestoreDeviceObjects();						// Restore derived class data.		
	
	return S_OK;
}
LPDIRECT3DSURFACE9 C3dView::getScene(HBITMAP hbmp)
{
	LPDIRECT3DSURFACE9 lpscene, lpshot;
	lpd3d->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &lpshot);		

	SIZE sz;
	GetBitmapDimensionEx(hbmp, &sz);	

	D3DSURFACE_DESC desc;
	lpshot->GetDesc(&desc);	
	lpd3d->CreateOffscreenPlainSurface(desc.Width, desc.Height + sz.cy, desc.Format, 
		D3DPOOL_DEFAULT, &lpscene, 0);		
	
	D3DLOCKED_RECT	lrscene, lrshot;
	lpscene->LockRect(&lrscene, 0, 0);
	lpshot->LockRect(&lrshot, 0, D3DLOCK_READONLY);	
	// surface
	DWORD *pbits0 = (DWORD*)lrscene.pBits,
		  *pbits1 = (DWORD*)lrshot.pBits;
	
	int i, j;
	for (i = 0; i < (int)desc.Height; i++) 
	{			
		memcpy(pbits0, pbits1, lrscene.Pitch);
		pbits0 += (lrscene.Pitch >> 2);
		pbits1 += (lrshot.Pitch >> 2);		
	}	
	lpshot->UnlockRect();		
	SAFE_RELEASE(lpshot);
	// bitmap		
	BYTE r, g, b;
	DWORD dw;	

	DWORD* pbits = new DWORD[sz.cx * sz.cy];	
	GetBitmapBits(hbmp, (sz.cx * sz.cy) << 2, pbits);	
	for (i = 0; i < sz.cy; i++) 
	{
		for (j = 0; j < sz.cx; j++)
		{
			dw = pbits[i * sz.cx + j];			
			r = (BYTE)dw;
			g = (BYTE)(dw >> 0x8);
			b = (BYTE)(dw >> 0x10);
			// change r and b values							
			*pbits0++ = D3DCOLOR_XRGB(b, g, r);
		}
		pbits0 += (lrscene.Pitch >> 2) - sz.cx; 
	}	
	SAFE_DELETE_ARRAY(pbits);	
	lpscene->UnlockRect();			
	
	return lpscene;
}
void C3dView::CheckZBias()
{
	if (!(dcaps.RasterCaps & D3DPRASTERCAPS_DEPTHBIAS) || 
			!(dcaps.RasterCaps & D3DPRASTERCAPS_SLOPESCALEDEPTHBIAS))
	{
		memset(msg, 0, MAX_PATH);
		sprintf_s(msg, MAX_PATH, "Внимание! Отсутствует поддержка DepthBias.\nВозможно некорректное отображение сетки.");
		writelog(msg, _log);
		MessageBox(msg, app.m_pszAppName, MB_ICONWARNING);				
	}		
}
void C3dView::Close3D()
{
	if (lpd3d) InvalidateDeviceObjects();	// Make sure everything is invalidated.			
	m_hDeviceState = D3DERR_INVALIDDEVICE;
	
	updateFont(UF_RELEASE);	
	SAFE_RELEASE(lptxt);
	SAFE_RELEASE(lptex);
	SAFE_RELEASE(lpd3d);		
	SAFE_RELEASE(lpdd);	
}
void C3dView::OnDraw(CDC*)
{	
	TestDeviceState();
	if FAILED(m_hDeviceState) return;								
	
	lpd3d->BeginScene();			
	lpd3d->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, app.stg._g_set.dw_bkgnd, 1.0f, 0L);	
	if (pgraph->m_binit)
	{
		lpd3d->SetTransform(D3DTS_PROJECTION, pgraph->getProj());		
		lpd3d->SetTransform(D3DTS_WORLD, pgraph->getWorld());			
		lpd3d->SetFVF(VERTEX_FVF);	
		lpd3d->SetTexture(0, 0);		

		LPDIRECT3DSURFACE9 lpback;
		lpd3d->GetRenderTarget(0, &lpback);
		// back
		lpd3d->SetRenderTarget(0, lpsurf);				
		lpd3d->Clear(0, 0, D3DCLEAR_TARGET, 0xffffffff, 1.0f, 0);								
		renderBack();					
		// scene
		lpd3d->SetRenderTarget(0, lpback);										
		renderScene();											// normal		
		if (pgraph->m_show.bshowWell) renderWell();				// well					
		if (pgraph->m_show.bshowCell) renderSkeleton();			// grid					
		if (pgraph->m_param.GetLength())
		{			
			lpd3d->SetFVF(VERTEX_FVFT);	
			lpd3d->SetTexture(0, lptex);
			renderSel();				
		}				
		
		SAFE_RELEASE(lpback);
		bmove = 1;
	}	
#ifdef _DEBUG	
	FPS();
#endif
	lpd3d->EndScene();					
	lpd3d->Present(0, 0, 0, 0);		
}
void C3dView::OnInitialUpdate()
{
	Open3D();		
	CView::OnInitialUpdate();
}
#ifdef _DEBUG
void C3dView::FPS()
{  	
	static FLOAT fLastTime = 0.0f;
	static DWORD dwFrames  = 0;		
	
	FLOAT fps = 0.0f, fTime = GetTickCount() * 0.001f;   	
	++dwFrames;   

	if (fTime - fLastTime > 1.0f)
    {
        fps = dwFrames / (fTime - fLastTime);
		sprintf_s(stats, 32, "%.2f fps, (%dx%d)", fps, pgraph->m_frustrum.w, 
				pgraph->m_frustrum.h);			
		
        fLastTime = fTime;
        dwFrames  = 0;
	}
	lptxt->DrawText(0, stats, -1, &rcFPS, DT_NOCLIP, 0xff000000);		
}
#endif
void C3dView::RestoreDeviceObjects()
{	
	lptxt->OnResetDevice();
	lpfont->OnResetDevice();  

	lpd3d->SetRenderState(D3DRS_LIGHTING, 0);		
	lpd3d->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	lpd3d->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);	

	if (pgraph->m_binit) pgraph->setProj(0);		
}
void C3dView::TestDeviceState()
{
	if (!lpd3d)
	{
		m_hDeviceState = D3DERR_INVALIDDEVICE;
		return;
	}
	// Load and check the cooperative level.
	m_hDeviceState = lpd3d->TestCooperativeLevel();
	if (m_hDeviceState == D3DERR_DEVICENOTRESET)
	{		
		if FAILED(m_hDeviceState = Reset3D()) return;			// try to reset
	}	
}
void C3dView::updateFont(UCHAR uf)
{	
	if (uf & UF_RELEASE) SAFE_RELEASE(lpfont);
	if (uf & UF_CREATE) D3DXCreateFontIndirect(lpd3d, &app.stg._w_set.ds, &lpfont);	 	
}
BEGIN_MESSAGE_MAP(C3dView, CView)
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)	
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_SIZE()
END_MESSAGE_MAP()
BOOL C3dView::OnEraseBkgnd(CDC*)
{
	return 0;
}
BOOL C3dView::OnMouseWheel(UINT nFlags, short zDelta, CPoint point)
{
	pgraph->scale(zDelta, 1);			
	return 0;
}
void C3dView::OnDestroy()
{
	Close3D();
	CView::OnDestroy();
}
void C3dView::OnMouseMove(UINT nFlags, CPoint point)
{
	if (bmove)
	{
		switch (nFlags)
		{
			case MK_LBUTTON:
			{					
				pgraph->mouseDrag(point);
			}		
			break;
			case MK_CONTROL | MK_LBUTTON:
			{
				pgraph->scale(pt.y - point.y);
			}
			break;
			case MK_SHIFT | MK_LBUTTON:
			{
				pgraph->move(pt - point);
			}
			break;				
		}
		pt = point;
		m_bdrag = 1;
	}	
}
void C3dView::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_bdrag = 0;
	if (bmove) pgraph->mouseClick(point);	
}
void C3dView::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_bdrag) pgraph->mouseRelease();
	else
	{		
		pgraph->mousePick(point);		
		app.m_pFrame->infoUpdate();
	}		
}
void C3dView::OnSize(UINT nType, int cx, int cy)
{	
	if SUCCEEDED(m_hDeviceState)
	{	// We're good to go.  See if the backbuffer size will be changing.
		if ((cx != pgraph->m_frustrum.w) || (cy != pgraph->m_frustrum.h))
		{	// The size will be changing.  Force a reset.  When finished, force a refresh of the device state. 
			// We'll put up the hourglass while we work.			
			BeginWaitCursor();			
			Reset3D();
			TestDeviceState();
			EndWaitCursor();
		}
	}
	// Let the base class run as well.
	CView::OnSize(nType, cx, cy);
}
// CInfoView
IMPLEMENT_DYNCREATE(CInfoView, CView)
CInfoView::CInfoView()
{
	m_font.CreateFont(-11, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Tahoma");
}
CInfoView::~CInfoView()
{
	m_font.DeleteObject();
}
BOOL CInfoView::OnPreparePrinting(CPrintInfo* pInfo)
{
	return DoPreparePrinting(pInfo);
}
HBITMAP CInfoView::getSnapShot()
{
	RECT rc;
	GetClientRect(&rc);			

	UINT h = rc.bottom - rc.top,
		 w = rc.right - rc.left;

	HDC hdc0 = ::GetDC(m_hWnd),
		hdc1 = CreateCompatibleDC(hdc0);
	
	HBITMAP hbm0 = CreateCompatibleBitmap(hdc0, w, h),
		    hbm1 = (HBITMAP)SelectObject(hdc1, hbm0);
	
	SetBitmapDimensionEx(hbm0, w, h, 0);
	BitBlt(hdc1, 0, 0, w, h, hdc0, 0, 0, SRCCOPY);
	SelectObject(hdc1, hbm1);		

	DeleteDC(hdc1);
	DeleteDC(hdc0);		
	
	return hbm0;
}
void CInfoView::OnDraw(CDC* pDC)
{
	CFont* poldfont = pDC->SelectObject(&m_font);	
	
	if (app.m_graph.m_binit)
	{
		legend* plegend;		
		CString line("Глубина");

		int index = szParam.cy;		
		if (index < 0) index = 0;		
	
		if (szParam.cx == 0) plegend = app.m_graph.m_plegend;		
		else 		
		{						
			line = app.m_graph.m_inilines[szParam.cx - 1]->param;			
			plegend = app.m_graph.m_inilines[szParam.cx - 1]->plegend;
		}
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(app.stg._g_set.cl_lfont);
		plegend->draw(pDC, app.stg._g_set.cl_lfont);		
		
		pDC->TextOut(390, 4, "Объект(ы):");						
		pDC->TextOut(460, 4, app.m_graph.m_title);							 
		pDC->TextOut(390, 18, line);
		pDC->TextOut(660, 4, app.m_graph.m_dim);
		pDC->TextOut(660, 18, app.m_graph.m_param);
	}	
	pDC->SelectObject(poldfont);
}
BEGIN_MESSAGE_MAP(CInfoView, CView)
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)	
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()
BOOL CInfoView::OnEraseBkgnd(CDC* pDC)
{	
	BYTE r = (BYTE)((app.stg._g_set.dw_bkgnd >> 0x10) & 0xff),
		 g = (BYTE)((app.stg._g_set.dw_bkgnd >> 0x8) & 0xff),
		 b = (BYTE)(app.stg._g_set.dw_bkgnd & 0xff);
	
	CBrush br(RGB(r, g, b));
    CBrush* poldbrush = pDC->SelectObject(&br);    
	
	RECT rc;
    pDC->GetClipBox(&rc);
    pDC->PatBlt(rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, PATCOPY);
    pDC->SelectObject(poldbrush);      
	
	return 1;
}