// graph.h
#pragma once
#pragma comment(lib, "d3d9")
#pragma comment(lib, "d3dx9")
#pragma comment(lib, "../dx/release/dx")

#include "d3dx9.h"
#include "obj1.h"
#include "template.h"
// update graph options
#define UG_CREATE				0x01
#define UG_READ					0x02
#define UG_SKEL					0x04
#define UG_ZCOEF				0x08
// visible surface
#define VS_TOP					0x01
#define VS_BOTTOM				0x02
#define VS_LEFT					0x04
#define VS_RIGHT				0x08
#define VS_FRONT				0x10
#define VS_BACK					0x20

#define MIN_CELL_PERSCREEN		2
#define TEX_VERTEX_NUM			4
#define VERTEX_FVF				(D3DFVF_XYZ | D3DFVF_DIFFUSE) 
#define VERTEX_FVFT				(D3DFVF_XYZ | D3DFVF_TEX1) 

#define RGB16(rgb32)			(((rgb32 % 32) << 3) | ((rgb32 / 32 % 64) << 10) | ((rgb32 / 32 / 64 % 32) << 19))
// stockline
struct stockline
{
	bool						bvalid;
	UCHAR						num;
	inline void init()
	{
		bvalid = 0;
		num = 0;
	}
};
// xyzDim
struct xyzDim
{
	SIZE						xDim, yDim, zDim;
	inline int xLen()
	{
		return xDim.cy - xDim.cx;
	}
	inline int yLen()
	{
		return yDim.cy - yDim.cx;
	}
	inline int zLen()
	{
		return zDim.cy - zDim.cx;
	}	
};
// D3DVERTEX
struct D3DVERTEX
{
	FLOAT x, y, z;	
	DWORD dw;
	inline D3DVERTEX()
	{
		x = y = z = 0.0f;
		dw = 0x0;
	}
	inline D3DVERTEX(const FLOAT x, const FLOAT y, const FLOAT z, DWORD dw)
	{
		this->x = x;
		this->y = y;
		this->z = z;			
		this->dw = dw;
	}
	inline D3DVERTEX(const D3DXVECTOR3& v, DWORD dw)
	{
		x = v.x;
		y = v.y;
		z = v.z;			
		this->dw = dw;
	}	
	D3DVERTEX& operator = (const D3DXVECTOR3& v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
		return *this;
	}
	D3DVERTEX& operator += (const D3DXVECTOR3& v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}
	D3DVERTEX& operator -= (const D3DXVECTOR3& v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}	
};
// D3DTEX
struct D3DTEX
{	
	FLOAT x, y, z;
	FLOAT tu, tv;
	inline D3DTEX()
	{
		x = y = z = 0.0f;
		tu = tv = 0.0f;
	}	
};
// desc
struct desc
{
	UINT	primitiveCount, startIndex;	
	inline desc()
	{
		primitiveCount = startIndex = 0;
	}
};
// buffers
struct iBuffer
{
	UINT					ilength;	
	LPDIRECT3DINDEXBUFFER9	ib;
	inline iBuffer()
	{
		ilength = 0;
		ib = 0;
	}
};
struct vBuffer 
{
	UINT					vlength;
	LPDIRECT3DVERTEXBUFFER9	vb;
	inline vBuffer()
	{
		vlength = 0;
		vb = 0;
	}
};
struct dBuffer : public desc, iBuffer
{
};
struct ivBuffer : public iBuffer, vBuffer
{
};
struct intersection
{
    bool					bwell;			// well or not	
	D3DXVECTOR3				v;				// coordinate		
	UCHAR					surf;			// picked surface	
	UINT					nwell;			// well number	
	inline intersection()
	{
		bwell = 0;
		nwell = 0;
	}
};
// CDXSlice
class CDXSlice
{		
private:		
	bool readStream(const char* filename);		
	bool pickBack(const D3DXVECTOR3* ppos, const D3DXVECTOR3* pdir, D3DXVECTOR3* pv);	
	bool pickBottom(const D3DXVECTOR3* ppos, const D3DXVECTOR3* pdir, D3DXVECTOR3* pv);	
	bool pickFront(const D3DXVECTOR3* ppos, const D3DXVECTOR3* pdir, D3DXVECTOR3* pv);	
	bool pickLeft(const D3DXVECTOR3* ppos, const D3DXVECTOR3* pdir, D3DXVECTOR3* pv);	
	bool pickRight(const D3DXVECTOR3* ppos, const D3DXVECTOR3* pdir, D3DXVECTOR3* pv);	
	bool pickTop(const D3DXVECTOR3* ppos, const D3DXVECTOR3* pdir, D3DXVECTOR3* pv);	
	HRESULT bottomBuffer();		
	HRESULT bottomSBuffer();		
	HRESULT sideBuffer();
	HRESULT sideSBuffer();
	HRESULT topBuffer();		
	HRESULT topSBuffer();		
	HRESULT updateSkeleton();				
	void freeBuffers();
	void freeContours();	
	void freeSBuffers();
public:
	CDXSlice(UCHAR index);							
	~CDXSlice();				
	HRESULT updateBuffer();
	float getPtHeight(UINT index) {return m_depths[0][index] - m_depths[1][index];} 
	void setBuff(UCHAR nbuffSize);				// new video buffer size			
	intersection pick(D3DXVECTOR3* prayPos, D3DXVECTOR3* prayDir);	
// Attributes		
private:		
	UCHAR					m_index;			// slice index
public:				
	struct bdraw
	{
		bool		bdrawTopNeeded, bdrawBottomNeeded;
	}			m_bdraw;	
	struct bfree
	{
		bool		bfreeBuffersNeeded, bfreeContoursNeeded, bfreeSBuffersNeeded;		
	}			m_bfree;				
	desc					m_desc[5], m_sdesc[5];
	UCHAR					m_buffSize;	
	float*					m_depths[2];						
	DWORD*					m_colors[2];		
	ivBuffer*				m_pbottomBuffer;	// top & bottom indices not equal cause cull	
	ivBuffer*				m_ptopBuffer;	
	ivBuffer				m_sideBuffer;

	dBuffer*				m_paddiSBuffer;		// skeleton additional index (roof & sole)		
	iBuffer*				m_piSBuffer;		// skeleton index (roof & sole)			
	vBuffer*				m_ptopSBuffer;		// skeleton top
	vBuffer*				m_pbottomSBuffer;	// skeleton	bottom			
	ivBuffer				m_sideSBuffer;		// skeleton	side				

	vBuffer*				m_pbottomRBuffer;	// render bottom	
	vBuffer*				m_ptopRBuffer;		// render top
	vBuffer					m_sideRBuffer;		// render side
//	delPtrList<CPtrList, CDXContour*>	m_dxContours;	
};
// CCylMesh
class CCylMesh
{
protected:	
	void _free();	
public:		
	CCylMesh();	
	~CCylMesh();	
	bool isectBottom(const D3DXVECTOR3* ppos, const D3DXVECTOR3* pdir, D3DXVECTOR3* pv);
	bool isectTop(const D3DXVECTOR3* ppos, const D3DXVECTOR3* pdir, D3DXVECTOR3* pv);
	bool isectSide(const D3DXVECTOR3* ppos, const D3DXVECTOR3* pdir, D3DXVECTOR3* pv);	
	void init(D3DXVECTOR3* pvtop, D3DXVECTOR3* pvbottom); 		

	virtual HRESULT update();			
// Attributes
private:	
	D3DXVECTOR3				m_vbottom;
public:		
	DWORD					m_dw;	
	UCHAR					m_sectors;	
	UINT					m_radius;
	ivBuffer				m_buffer;
	D3DXVECTOR3				m_vtop;
};
// CConeMesh
class CConeMesh : public CCylMesh
{
public:	
	bool intersect(const D3DXVECTOR3* ppos, const D3DXVECTOR3* pdir, D3DXVECTOR3* pv);  
	void init(const FLOAT xc, const FLOAT yc, bool brev = 0);
// Overrides		
	HRESULT update();	
// Attributes
private:
	bool					m_brev;						// reverse cone	
public:
	USHORT					m_h;	
	D3DXVECTOR3				m_vcenter;
};
// horzparam
struct horzparam
{
	int						xc,	yc;		
	float					depth;
};
// CDXWell
class CDXWell
{ 
public:
	CDXWell(const char* wellname);
	~CDXWell();
	bool pick(const D3DXVECTOR3* prayPos, const D3DXVECTOR3* prayDir, D3DXVECTOR3* pv);	
	HRESULT render();
	HRESULT update(const FLOAT top);	
	void init();
	void screen(D3DXMATRIX* pProj, D3DXMATRIX* pView, D3DXMATRIX* pWorld);
// Attributes
private:	
	bool		brev;
	FLOAT		ftop;
	RECT		rc;	
public:	
	char					name[9];
	CConeMesh				m_cone;	
	DWORD					dw_color;
	std::vector<horzparam>	m_hparams;
	delPtrArray<CPtrArray, CCylMesh*>	m_cyls; 
};
// iniline
class iniline : public iniext
{
public:	
	iniline(const char* key, const char* value);
    virtual ~iniline();	
	virtual bool initlegend();	
	virtual float getparam(UINT nx, UINT ny, USHORT nz);
// Attributes
public:
	CString			param;
	legend*			plegend;	
};
// iniwline
class iniwline : public iniline
{
public:
	iniwline(const char* key, const char* value);	
	~iniwline();
	bool initlegend();
	float getparam(UINT nx, UINT ny, USHORT nz);
// Attributes
public:
	UCHAR			periods;
	float*			m_flimits[2];	
};
// CGraph
class CGraph : public CCmdTarget
{	
	DECLARE_DYNCREATE(CGraph)
private:
	bool readStruct();	
	bool readWell();
	D3DXVECTOR3 mouseOnSphere(const POINT pt);					// arcball									
	void updateSel(D3DTEX* pvx); 
public:
	CGraph();
	~CGraph();	
	inline D3DXMATRIX* getProj(){return &m_mProj;}											
	inline D3DXMATRIX* getWorld(){return &m_mWorld;}		
	bool initDeviceObject();					
	HRESULT updateDeviceWell(bool buw = 0);	
	HRESULT updateDeviceObject(BYTE upd = UG_CREATE);	
	void BackView();
	void BottomView();
	void clear();
	void createView();
	void FrontView();
	void LeftView();
	void mouseClick(const POINT pt);
	void mouseDrag(const POINT pt);
	void mousePick(const POINT pt);
	void mouseRelease();
	void move(const CPoint pt);		
	void ResetView();
	void RightView();
	void scale(const int ny, bool bwheel = 0);
	void setProj(bool bfirst);			
	void TopView();
	void updateFonts();
// Attributes
private:
	float					m_radius;
	D3DXMATRIX				m_mProj, m_mView, m_mWorld;
	D3DXQUATERNION			m_down, m_now;	         			 
	D3DXVECTOR3				m_from, m_where;
	SHORT					m_curwell;	
public:
	struct _3dscale
	{
		float				curscale, maxscale;				
		UINT				begscale;	
		inline float viewscale() {return begscale / curscale;}				
	}		m_3dscale;	
	struct frustrum
	{
		float				zn, zf;		
		int					w, h;
		frustrum& operator = (const RECT& rc)
		{
			h = rc.bottom - rc.top;
			w = rc.right - rc.left;
			return *this;			
		}
	}		m_frustrum;
	struct show	
	{
		BOOL bshowCell, bshowCont, bshowWell;
	}			m_show; 	
	bool					m_binit;
	char					_actn[MAX_PATH];							// active cell file
	int						m_nwells;
	CString					m_dim, m_param, m_title;		
	CString					m_contfile, m_wellfile;
	iniext					m_structfile;	
	vBuffer					m_selBuffer;								// sel vertex buffer		
	legend*					m_plegend;									// struct legend
	std::vector<UINT>					m_layers, m_rows;				// device vb rows
	delPtrArray<CPtrArray, iniline*>	m_inilines;
	delPtrArray<CPtrArray, CDXSlice*>	m_dxSlices;
	delPtrArray<CPtrArray, CDXWell*>	m_dxWells;
protected:
	DECLARE_OLECREATE(CGraph)
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()	
	HRESULT ResetFilter();
	HRESULT SetDims(LONG x0, LONG x1, LONG y0, LONG y1, LONG z0, LONG z1);
	void ShowCCW(BOOL showCell, BOOL showCont, BOOL showWell);
	
	enum 
	{		
		dispidshowCCW = 10L,		dispidsetDims = 9L,		dispidresetFilter = 8L,		dispidtopView = 7L,		dispidrightView = 6L,		dispidresetView = 5L,		dispidleftView = 4L,		dispidfrontView = 3L,		dispidbottomView = 2L,		dispidbackView = 1L
	};						
};