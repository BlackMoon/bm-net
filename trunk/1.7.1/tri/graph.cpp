// graph.cpp
#include "stdafx.h"
#include "graph.h"
#include "d3dx9math.h"

bool						uw = 1;									// update well
char						_log[MAX_PATH], msg[MAX_PATH];
UCHAR						surf, ug;								// update graph	
UINT						size = 0;								// nx * ny
D3DCAPS9					dcaps;
D3DXVECTOR3					vec3 = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// scene centre
LPD3DXFONT					lpfont = 0;								// font	
LPDIRECT3DDEVICE9			lpd3d = 0;								// device
LPDIRECT3DSURFACE9			lpsurf = 0;								// offscreen surface
SIZE						szParam;								// param numbers	
SPACE						space;	
stockline					_stockline;
xyzDim						_xyzDim;		

// functions
bool arrayVal(std::vector<UINT>* pvec, UINT nVal)
{	
	UINT val, sum = 0;
	for (UINT i = 0; i < pvec->size(); i++)
	{
		val = pvec->at(i);
		sum += val;
		if (sum == nVal) return 1;		
	}
	return 0;
}
bool hitTri(const D3DXVECTOR3* p0, const D3DXVECTOR3* p1, const D3DXVECTOR3* p2, 
			const D3DXVECTOR3* prayPos, const D3DXVECTOR3* prayDir, D3DXVECTOR3* pout)
{
	D3DXPLANE plane;	
	D3DXPlaneFromPoints(&plane, p0, p1, p2);
	D3DXPlaneIntersectLine(pout, &plane, prayPos, prayDir);
	
	D3DXVECTOR3 ax, cx, v;	
	D3DXVECTOR3 lines[3];
	lines[0] = *p1 - *p0; // AB
	lines[1] = *p2 - *p1; // BC
	lines[2] = *p0 - *p2; // CA
	ax = *pout - *p0;
	cx = *pout - *p2; 	
	
	double sabc, saxb, scxa, sbxc;
	D3DXVec3Cross(&v, &lines[0], &lines[1]);
	sabc = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);

	D3DXVec3Cross(&v, &lines[0], &ax);
	saxb = D3DXVec3Length(&v);
	
	D3DXVec3Cross(&v, &lines[1], &cx);
	sbxc = D3DXVec3Length(&v);
	
	D3DXVec3Cross(&v, &lines[2], &cx);
	scxa = D3DXVec3Length(&v);	
	
	return (fabs(saxb + sbxc + scxa - sabc) < 1E-1);
}
D3DXVECTOR3 eulerFromMatrix(const D3DXMATRIX* pM)
{
	D3DXVECTOR3 v; // v.x - pitch, v.y - yaw, v.z - roll	                 	
	
	v.y = -asin(pM->_13);
	if (pM->_13 > 1.0f) v.y = -D3DX_PI/2.0f;
	if (pM->_13 < -1.0f) v.y = D3DX_PI/2.0f;
	
	float a, b, c = cosf(v.y);	
	if (fabs(c) > 1E-3)
	{
		a = pM->_33/c;
		b = pM->_23/c;
		v.x = atan2(b, a);

		a = pM->_11/c;
		b = pM->_12/c;
		v.z = atan2(b, a);
	}
	else
	{		
		v.x = 0;

		a = pM->_22/c;
		b = pM->_21/c;
		v.z = atan2(b, a);
	}	
	return v;
}
void resetFilter()
{		
	memset(&_xyzDim, 0, 24);	
	_xyzDim.xDim.cy = space.nX - 1;
	_xyzDim.yDim.cy = space.nY - 1;
	_xyzDim.zDim.cy = space.nZ;			
}
void tex0(D3DTEX* pvx)
{
	pvx[0].tu = 0.5f;
	pvx[0].tv = 0.5f;
	
	pvx[1].tu = 0.5f;
	pvx[1].tv = 1.0f;

	pvx[2].tu = 1.0f;
	pvx[2].tv = 0.5f;					

	pvx[3].tu = 1.0f;
	pvx[3].tv = 1.0f;
}
void tex1(D3DTEX* pvx)
{
	pvx[0].tu = 0.5f;
	pvx[0].tv = 0.0f;

	pvx[1].tu = 0.5f;
	pvx[1].tv = 0.5f;

	pvx[2].tu = 1.0f;
	pvx[2].tv = 0.0f;	
					
	pvx[3].tu = 1.0f;
	pvx[3].tv = 0.5f;
}
void tex2(D3DTEX* pvx)
{
	pvx[0].tu = 0.0f;
	pvx[0].tv = 0.5f;

	pvx[1].tu = 0.0f;
	pvx[1].tv = 1.0f;

	pvx[2].tu = 0.5f;
	pvx[2].tv = 0.5f;	
					
	pvx[3].tu = 0.5f;
	pvx[3].tv = 1.0f;
}
void tex3(D3DTEX* pvx)
{
	pvx[0].tu = 0.0f;
	pvx[0].tv = 0.0f;

	pvx[1].tu = 0.0f;
	pvx[1].tv = 0.5f;

	pvx[2].tu = 0.5f;
	pvx[2].tv = 0.0f;
						
	pvx[3].tu = 0.5f;
	pvx[3].tv = 0.5f;
}
// iniline
iniline::iniline(const char* key, const char* value) : iniext(key)
{	
	plegend = 0;
	param = CString(value);	
}
iniline::~iniline()
{
	SAFE_DELETE(plegend);
}
bool iniline::initlegend()
{
	try
	{
		FILE* stream;
		fopen_s(&stream, fext, "rb");	
		if (!stream) throw 0;

		float fzmin, fzmax;		
		if (fread(&fzmin, 4, 1, stream) != 1) throw -1;
		if (fread(&fzmax, 4, 1, stream) != 1) throw -1;
		
		plegend = new legend(fzmin, fzmax);
		plegend->setTemplate(&app.stg._c_set);
		
		fclose(stream);
	}
	catch (int)
	{	
		memset(msg, 0, MAX_PATH);
		sprintf_s(msg, MAX_PATH, "Не удалось открыть %s.", fext);				
		return 0;
	}	
	
	return 1;
}
float iniline::getparam(UINT nx, UINT ny, USHORT nz)
{
	float fvalue;
	FILE* stream; 
	fopen_s(&stream, fext, "rb");
	UINT offset = ((1 + nz * size) << 1) + ny * space.nX + nx; 
	
	fseek(stream, offset << 2, SEEK_SET);
	fread(&fvalue, 4, 1, stream);		
	fclose(stream);		

	return fvalue;
}
// iniwline
iniwline::iniwline(const char* key, const char* value) : iniline(key, value)
{
	m_flimits[0] = 0;
	m_flimits[1] = 0;
	periods = 0;
}
iniwline::~iniwline()
{
	SAFE_DELETE_ARRAY(m_flimits[0]);
	SAFE_DELETE_ARRAY(m_flimits[1]);		
}
bool iniwline::initlegend()
{	
	try
	{
		FILE* stream; 
		fopen_s(&stream, fext, "rb");	
	    if (!stream) throw 0;

		float fzmin, fzmax;		
		if (fread(&fzmin, 4, 1, stream) != 1) throw -1;
		if (fread(&fzmax, 4, 1, stream) != 1) throw - 1;
		
		plegend = new legend(fzmin, fzmax);
		plegend->setTemplate(&app.stg._c_set);

		m_flimits[0] = new float[periods];
		m_flimits[1] = new float[periods];
		UINT offset = space.cube();

		for (UCHAR i = 0; i < periods; i++)
		{
			if (fread(&m_flimits[0][i], 4, 1, stream) != 1) throw -1;
			if (fread(&m_flimits[1][i], 4, 1, stream) != 1) throw - 1;
			
			fseek(stream, offset << 3, SEEK_CUR);
		}
		fclose(stream);
	}
	catch (int)
	{	
		memset(msg, 0, MAX_PATH);
		sprintf_s(msg, MAX_PATH, "Не удалось открыть %s.", fext);				
		return 0;
	}		
	return 1;
}
float iniwline::getparam(UINT nx, UINT ny, USHORT nz)
{
	float fvalue;
	FILE* stream; 
	fopen_s(&stream, fext, "rb");
	UINT offset = ((2 + nz * size) << 1) + ny * space.nX + nx;		
	     offset += (szParam.cy * (space.cube() + 1)) << 1; 			 
	
	fseek(stream, offset << 2, SEEK_SET);
	fread(&fvalue, 4, 1, stream);		
	fclose(stream);		

	return fvalue;
}
// CDXSlice
CDXSlice::CDXSlice(UCHAR index)
{
	ASSERT(index > -1);
	m_index = index;	
	
	UINT dsize = space.square();
	m_colors[0] = new DWORD[dsize];
	m_colors[1] = new DWORD[dsize];
	
	m_depths[0] = new float[dsize];
	m_depths[1] = new float[dsize];
	
	memset(&m_bdraw, 0, 2);
	memset(&m_bfree, 0, 3); 	
}
CDXSlice::~CDXSlice()
{			
	SAFE_DELETE_ARRAY(m_colors[0]);	
	SAFE_DELETE_ARRAY(m_colors[1]);

	SAFE_DELETE_ARRAY(m_depths[0]);
	SAFE_DELETE_ARRAY(m_depths[1]);
		
	if (m_bfree.bfreeBuffersNeeded) freeBuffers();
	if (m_bfree.bfreeSBuffersNeeded) freeSBuffers();

//	m_dxContours.RemoveAll();
}
bool CDXSlice::pickBack(const D3DXVECTOR3* ppos, const D3DXVECTOR3* pdir, D3DXVECTOR3* pv)
{	
	DWORD* pindices;		
	D3DXVECTOR3 v0, v1, v2;
	D3DVERTEX* pvertices;				
	
	m_sideBuffer.ib->Lock(0, 0, (VOID**)&pindices, 0);
	m_sideBuffer.vb->Lock(0, 0, (VOID**)&pvertices, 0);			
	m_sideBuffer.ib->Unlock();
	m_sideBuffer.vb->Unlock();	
		
	UINT nstartIndex = m_desc[4].startIndex;
	for (UINT i = nstartIndex; i < nstartIndex + m_desc[4].primitiveCount; i++)
	{	
		v0.x = pvertices[pindices[i]].x;
		v0.y = pvertices[pindices[i]].y;
		v0.z = pvertices[pindices[i]].z;

		v1.x = pvertices[pindices[i + 1]].x;
		v1.y = pvertices[pindices[i + 1]].y;
		v1.z = pvertices[pindices[i + 1]].z;

		v2.x = pvertices[pindices[i + 2]].x;
		v2.y = pvertices[pindices[i + 2]].y;
		v2.z = pvertices[pindices[i + 2]].z;
		
		if (hitTri(&v0, &v1, &v2, ppos, pdir, pv)) return 1;					
	}	
	return 0;
}
bool CDXSlice::pickBottom(const D3DXVECTOR3* ppos, const D3DXVECTOR3* pdir, D3DXVECTOR3* pv)
{
	DWORD* pindices;		
	D3DXVECTOR3 v0, v1, v2, v;
	D3DVERTEX* pvertices;	
	
	std::vector<D3DXVECTOR3> vectors;

	int j;
	UINT i, i0;
	for (UCHAR nbuff = 0; nbuff < m_buffSize; nbuff++)
	{	
		m_pbottomBuffer[nbuff].ib->Lock(0, 0, (VOID**)&pindices, 0);
		m_pbottomBuffer[nbuff].vb->Lock(0, 0, (VOID**)&pvertices, 0);				
		m_pbottomBuffer[nbuff].ib->Unlock();
		m_pbottomBuffer[nbuff].vb->Unlock();		
		m_pbottomBuffer[nbuff].ilength = 0;

		i0 = app.m_graph.m_rows[nbuff];	
		for (i = 0; i < i0; i++)
		{			
			for (j = 0; j < _xyzDim.xLen() << 1; j++)
			{
				v0.x = pvertices[pindices[m_pbottomBuffer[nbuff].ilength]].x;
				v0.y = pvertices[pindices[m_pbottomBuffer[nbuff].ilength]].y;
				v0.z = pvertices[pindices[m_pbottomBuffer[nbuff].ilength]].z;

				v1.x = pvertices[pindices[m_pbottomBuffer[nbuff].ilength + 1]].x;
				v1.y = pvertices[pindices[m_pbottomBuffer[nbuff].ilength + 1]].y;
				v1.z = pvertices[pindices[m_pbottomBuffer[nbuff].ilength + 1]].z;

				v2.x = pvertices[pindices[m_pbottomBuffer[nbuff].ilength + 2]].x;
				v2.y = pvertices[pindices[m_pbottomBuffer[nbuff].ilength + 2]].y;
				v2.z = pvertices[pindices[m_pbottomBuffer[nbuff].ilength + 2]].z;
				
				if (hitTri(&v0, &v1, &v2, ppos, pdir, &v)) 
				{
					vectors.push_back(v);															
				}
				m_pbottomBuffer[nbuff].ilength++;
			}			
			m_pbottomBuffer[nbuff].ilength += 2;
		}		
	}
	UCHAR index, nsize = (UCHAR)vectors.size();	
	if (nsize == 0) return 0;
	// check for min intersection
	D3DXVECTOR3 vlen;
	float f0 = FLT_MAX, f1;
	for (i = 0; i < nsize; i++)
	{		
		vlen = *ppos - vectors[i];
		f1 = D3DXVec3Length(&vlen);
		if (f1 < f0) 
		{
			f0 = f1; 
			index = i;		
		}
	}		
	pv->x = vectors[index].x;
	pv->y = vectors[index].y;
	pv->z = vectors[index].z;
	vectors.clear();		
	
	return 1;
}
bool CDXSlice::pickFront(const D3DXVECTOR3* ppos, const D3DXVECTOR3* pdir, D3DXVECTOR3* pv)
{	
	DWORD* pindices;		
	D3DXVECTOR3 v0, v1, v2;
	D3DVERTEX* pvertices;				
	
	m_sideBuffer.ib->Lock(0, 0, (VOID**)&pindices, 0);
	m_sideBuffer.vb->Lock(0, 0, (VOID**)&pvertices, 0);			
	m_sideBuffer.ib->Unlock();
	m_sideBuffer.vb->Unlock();	
		
	UINT nstartIndex = m_desc[3].startIndex;
	for (UINT i = nstartIndex; i < nstartIndex + m_desc[3].primitiveCount; i++)
	{	
		v0.x = pvertices[pindices[i]].x;
		v0.y = pvertices[pindices[i]].y;
		v0.z = pvertices[pindices[i]].z;

		v1.x = pvertices[pindices[i + 1]].x;
		v1.y = pvertices[pindices[i + 1]].y;
		v1.z = pvertices[pindices[i + 1]].z;

		v2.x = pvertices[pindices[i + 2]].x;
		v2.y = pvertices[pindices[i + 2]].y;
		v2.z = pvertices[pindices[i + 2]].z;
		
		if (hitTri(&v0, &v1, &v2, ppos, pdir, pv)) return 1;					
	}	
	return 0;
}
bool CDXSlice::pickLeft(const D3DXVECTOR3* ppos, const D3DXVECTOR3* pdir, D3DXVECTOR3* pv)
{		
	DWORD* pindices;		
	D3DXVECTOR3 v0, v1, v2;
	D3DVERTEX* pvertices;				
	
	m_sideBuffer.ib->Lock(0, 0, (VOID**)&pindices, 0);
	m_sideBuffer.vb->Lock(0, 0, (VOID**)&pvertices, 0);			
	m_sideBuffer.ib->Unlock();
	m_sideBuffer.vb->Unlock();	
		
	UINT nstartIndex = m_desc[1].startIndex;
	for (UINT i = nstartIndex; i < nstartIndex + m_desc[1].primitiveCount; i++)
	{	
		v0.x = pvertices[pindices[i]].x;
		v0.y = pvertices[pindices[i]].y;
		v0.z = pvertices[pindices[i]].z;

		v1.x = pvertices[pindices[i + 1]].x;
		v1.y = pvertices[pindices[i + 1]].y;
		v1.z = pvertices[pindices[i + 1]].z;

		v2.x = pvertices[pindices[i + 2]].x;
		v2.y = pvertices[pindices[i + 2]].y;
		v2.z = pvertices[pindices[i + 2]].z;
		
		if (hitTri(&v0, &v1, &v2, ppos, pdir, pv)) return 1;					
	}	
	return 0;
}
bool CDXSlice::pickRight(const D3DXVECTOR3* ppos, const D3DXVECTOR3* pdir, D3DXVECTOR3* pv)
{	
	DWORD* pindices;		
	D3DXVECTOR3 v0, v1, v2;
	D3DVERTEX* pvertices;				
	
	m_sideBuffer.ib->Lock(0, 0, (VOID**)&pindices, 0);
	m_sideBuffer.vb->Lock(0, 0, (VOID**)&pvertices, 0);			
	m_sideBuffer.ib->Unlock();
	m_sideBuffer.vb->Unlock();	
		
	UINT nstartIndex = m_desc[2].startIndex;
	for (UINT i = nstartIndex; i < nstartIndex + m_desc[2].primitiveCount; i++)
	{	
		v0.x = pvertices[pindices[i]].x;
		v0.y = pvertices[pindices[i]].y;
		v0.z = pvertices[pindices[i]].z;

		v1.x = pvertices[pindices[i + 1]].x;
		v1.y = pvertices[pindices[i + 1]].y;
		v1.z = pvertices[pindices[i + 1]].z;

		v2.x = pvertices[pindices[i + 2]].x;
		v2.y = pvertices[pindices[i + 2]].y;
		v2.z = pvertices[pindices[i + 2]].z;
		
		if (hitTri(&v0, &v1, &v2, ppos, pdir, pv)) return 1;					
	}	
	return 0;
}
bool CDXSlice::pickTop(const D3DXVECTOR3* ppos, const D3DXVECTOR3* pdir, D3DXVECTOR3* pv)
{
	DWORD* pindices;		
	D3DXVECTOR3 v0, v1, v2, v;
	D3DVERTEX* pvertices;	
	
	std::vector<D3DXVECTOR3> vectors;
	
	int j;
	UINT i, i0;
	for (UCHAR nbuff = 0; nbuff < m_buffSize; nbuff++)
	{	
		m_ptopBuffer[nbuff].ib->Lock(0, 0, (VOID**)&pindices, 0);
		m_ptopBuffer[nbuff].vb->Lock(0, 0, (VOID**)&pvertices, 0);				
		m_ptopBuffer[nbuff].ib->Unlock();
		m_ptopBuffer[nbuff].vb->Unlock();		
		m_ptopBuffer[nbuff].ilength = 0;

		i0 = app.m_graph.m_rows[nbuff];	
		for (i = 0; i < i0; i++)
		{			
			for (j = 0; j < _xyzDim.xLen() << 1; j++)
			{
				v0.x = pvertices[pindices[m_ptopBuffer[nbuff].ilength]].x;
				v0.y = pvertices[pindices[m_ptopBuffer[nbuff].ilength]].y;
				v0.z = pvertices[pindices[m_ptopBuffer[nbuff].ilength]].z;

				v1.x = pvertices[pindices[m_ptopBuffer[nbuff].ilength + 1]].x;
				v1.y = pvertices[pindices[m_ptopBuffer[nbuff].ilength + 1]].y;
				v1.z = pvertices[pindices[m_ptopBuffer[nbuff].ilength + 1]].z;

				v2.x = pvertices[pindices[m_ptopBuffer[nbuff].ilength + 2]].x;
				v2.y = pvertices[pindices[m_ptopBuffer[nbuff].ilength + 2]].y;
				v2.z = pvertices[pindices[m_ptopBuffer[nbuff].ilength + 2]].z;
				
				if (hitTri(&v0, &v1, &v2, ppos, pdir, &v)) 
				{
					vectors.push_back(v);															
				}
				m_ptopBuffer[nbuff].ilength++;
			}			
			m_ptopBuffer[nbuff].ilength += 2;
		}		
	}
	UCHAR index, nsize = (UCHAR)vectors.size();	
	if (nsize == 0) return 0;
	// check for min intersection
	D3DXVECTOR3 vlen;
	float f0 = FLT_MAX, f1;
	for (i = 0; i < nsize; i++)
	{		
		vlen = *ppos - vectors[i];
		f1 = D3DXVec3Length(&vlen);
		if (f1 < f0) 
		{
			f0 = f1; 
			index = i;		
		}
	}		
	pv->x = vectors[index].x;
	pv->y = vectors[index].y;
	pv->z = vectors[index].z;
	vectors.clear();		
	
	return 1;
}
bool CDXSlice::readStream(const char* filename)
{
	bool bres = 1;		
	
	try
	{		
		FILE* stream; 
		fopen_s(&stream, filename, "rb");
		if (!stream) throw 0;

		UINT offset = (m_index * size) << 1;		
		
		if (szParam.cy != -1)												// roxar data
		{
			offset += (2 + szParam.cy * (space.cube() + 1)) << 1;
			fseek(stream, offset << 2, SEEK_SET);			
			fseek(stream, size << 2, SEEK_CUR);									
			if (fread(m_colors[0], 4, size, stream) != size) throw - 1;			
			memcpy(m_colors[1], m_colors[0], size << 2);
		}
		else
		{
			if (szParam.cx == 0)								// roof & sole
			{
				fseek(stream, (++offset << 3), SEEK_SET);
			
				fseek(stream, size << 2, SEEK_CUR);			
				if (fread(m_colors[0], 4, size, stream) != size) throw - 1;
				fseek(stream, size << 2, SEEK_CUR);
				if (fread(m_colors[1], 4, size, stream) != size) throw - 1;
			}
			else
			{			
				fseek(stream, (offset << 2) + 8, SEEK_SET);			
				fseek(stream, size << 2, SEEK_CUR);			
				if (fread(m_colors[0], 4, size, stream) != size) throw - 1;			
				memcpy(m_colors[1], m_colors[0], size << 2);
			}
		}
		fclose(stream);			
	}	
	catch (int)
	{
		memset(msg, 0, MAX_PATH);
		sprintf_s(msg, MAX_PATH, "Не  удалось открыть %s.", filename);		
		bres = 0;
	}	
	
	return bres;
}
intersection CDXSlice::pick(D3DXVECTOR3* prayPos, D3DXVECTOR3* prayDir)
{	
	intersection isection;
	memset(&isection, 0, 24);	
	std::vector<intersection> isections;	

	if ((surf & VS_TOP) && m_bdraw.bdrawTopNeeded) 
	{
		if (pickTop(prayPos, prayDir, &isection.v)) 
		{
			isection.surf = VS_TOP;
			isections.push_back(isection);
		}
	}
	if ((surf & VS_BOTTOM) && m_bdraw.bdrawBottomNeeded) 
	{
		if (pickBottom(prayPos, prayDir, &isection.v)) 
		{
			isection.surf = VS_BOTTOM;
			isections.push_back(isection);			
		}
	}
	if (surf & VS_LEFT)
	{
		if (pickLeft(prayPos, prayDir, &isection.v)) 
		{
			isection.surf = VS_LEFT;
			isections.push_back(isection);			
		}
	}
	else if (surf & VS_RIGHT)
	{
		if (pickRight(prayPos, prayDir, &isection.v)) 
		{
			isection.surf = VS_RIGHT;
			isections.push_back(isection);			
		}
	}
	if (surf & VS_FRONT)
	{
		if (pickFront(prayPos, prayDir, &isection.v)) 
		{
			isection.surf = VS_FRONT;
			isections.push_back(isection);			
		}
	}
	else if (surf & VS_BACK)
	{
		if (pickBack(prayPos, prayDir, &isection.v)) 
		{
			isection.surf = VS_BACK;
			isections.push_back(isection);			
		}
	}
	
	D3DXVECTOR3 vlen;
	float f0 = FLT_MAX, f1;
	UCHAR i = 0, index;
	for (i; i < isections.size(); i++)
	{		
		vlen = *prayPos - isections[i].v;
		f1 = D3DXVec3LengthSq(&vlen);
		if (f1 < f0) 
		{
			f0 = f1; 
			index = i;		
		}	
	}		
	isection = isections[index];
	isections.clear();		
	return isection;
}
HRESULT CDXSlice::bottomBuffer()
{
	HRESULT hres = S_OK;
	
	DWORD* pindices;	
	D3DVERTEX* pvertices, *prvertices;		
	
	D3DVERTEX vx;
	int j;
	UINT i, i0 = _xyzDim.yDim.cx, i1;		
		
	for (UCHAR nbuff = 0; nbuff < m_buffSize; nbuff++)
	{				
		i1 = app.m_graph.m_rows[nbuff];													
		if (ug & UG_CREATE)
		{
			m_pbottomBuffer[nbuff].ilength = ((_xyzDim.xLen() + 1) << 1) * i1;
			m_pbottomBuffer[nbuff].vlength = 
			m_pbottomRBuffer[nbuff].vlength = (_xyzDim.xLen() + 1) * (i1 + 1);								 
			
			if FAILED(hres = lpd3d->CreateIndexBuffer(m_pbottomBuffer[nbuff].ilength << 2,
					0, D3DFMT_INDEX32, D3DPOOL_MANAGED, &m_pbottomBuffer[nbuff].ib, 0)) return hres;					
			if FAILED(hres = lpd3d->CreateVertexBuffer(m_pbottomBuffer[nbuff].vlength << 4,
					0, VERTEX_FVF, D3DPOOL_MANAGED, &m_pbottomBuffer[nbuff].vb, 0)) return hres;							
			if FAILED(hres = lpd3d->CreateVertexBuffer(m_pbottomRBuffer[nbuff].vlength << 4,
					0, VERTEX_FVF, D3DPOOL_MANAGED, &m_pbottomRBuffer[nbuff].vb, 0)) return hres;
		}
		m_pbottomBuffer[nbuff].ib->Lock(0, m_pbottomBuffer[nbuff].ilength << 2, 
				(VOID**)&pindices, 0);
		m_pbottomBuffer[nbuff].vb->Lock(0, m_pbottomBuffer[nbuff].vlength << 4, 
				(VOID**)&pvertices, 0);	
		m_pbottomRBuffer[nbuff].vb->Lock(0, m_pbottomRBuffer[nbuff].vlength << 4, 
					(VOID**)&prvertices, 0);								
			
		m_pbottomBuffer[nbuff].ilength = m_pbottomBuffer[nbuff].vlength = 
		m_pbottomRBuffer[nbuff].vlength = 0;
			
		for (i = i0; i < i0 + i1; i++)
		{
			for (j = _xyzDim.xDim.cx; j < _xyzDim.xDim.cy + 1; j++)
			{					
				pindices[m_pbottomBuffer[nbuff].ilength] = m_pbottomBuffer[nbuff].vlength + 
					(_xyzDim.xLen() + 1);						
				m_pbottomBuffer[nbuff].ilength++;			
				pindices[m_pbottomBuffer[nbuff].ilength] = m_pbottomBuffer[nbuff].vlength;
				m_pbottomBuffer[nbuff].ilength++;							
										
				vx.x = (FLOAT)(space.xStep * j);
				vx.y = (FLOAT)(space.yStep * i);
				vx.z = m_depths[1][i * space.nX + j];				;							
						
				vx -= vec3;
				vx.z *= app.stg._g_set.zcoef;
				vx.dw = m_colors[1][i * space.nX + j];				

				pvertices[m_pbottomBuffer[nbuff].vlength] = vx;				
				m_pbottomBuffer[nbuff].vlength++;
					
				vx.dw = RGB16(m_index);

				prvertices[m_pbottomRBuffer[nbuff].vlength] = vx;
				m_pbottomRBuffer[nbuff].vlength++;					
			}					
		}
		m_pbottomBuffer[nbuff].ib->Unlock();	
		// remove if from circle - last row without indices
		for (j = _xyzDim.xDim.cx; j < _xyzDim.xDim.cy + 1; j++)
		{						
			vx.x = (FLOAT)(space.xStep * j);
			vx.y = (FLOAT)(space.yStep * i);
			vx.z = m_depths[1][i * space.nX + j];

			vx -= vec3;
			vx.z *= app.stg._g_set.zcoef;
			vx.dw = m_colors[1][i * space.nX + j];

			pvertices[m_pbottomBuffer[nbuff].vlength] = vx;	
			m_pbottomBuffer[nbuff].vlength++;
						
			vx.dw = RGB16(m_index);

			prvertices[m_pbottomRBuffer[nbuff].vlength] = vx;					
			m_pbottomRBuffer[nbuff].vlength++;
		}						
		m_pbottomBuffer[nbuff].vb->Unlock();									
		m_pbottomRBuffer[nbuff].vb->Unlock();									
		i0 += i1;		
	}	
	
	return hres;
}
HRESULT CDXSlice::bottomSBuffer()
{
	HRESULT hres = S_OK;

	D3DVERTEX* pvertices;		
	
	D3DVERTEX vx;
	int j;
	UINT i, i0 = _xyzDim.yDim.cx, i1;
	
	for (UCHAR nbuff = 0; nbuff < m_buffSize; nbuff++)
	{
		i1 = app.m_graph.m_rows[nbuff];
		
		if (ug & UG_CREATE)
		{
			m_pbottomSBuffer[nbuff].vlength = (_xyzDim.xLen() + 1) * (i1 + 1);												
		
			if FAILED(hres = lpd3d->CreateVertexBuffer(m_pbottomSBuffer[nbuff].vlength << 4,
					0, VERTEX_FVF, D3DPOOL_MANAGED, &m_pbottomSBuffer[nbuff].vb, 0)) return hres;												
		}
		m_pbottomSBuffer[nbuff].vb->Lock(0, m_pbottomSBuffer[nbuff].vlength << 4, (VOID**)&pvertices, 0);											
				
		m_pbottomSBuffer[nbuff].vlength = 0;						
		for (i = i0; i < i0 + i1 + 1; i++)
		{
			for (j = _xyzDim.xDim.cx; j < _xyzDim.xDim.cy + 1; j++)
			{							
				vx.x = (FLOAT)(space.xStep * j);
				vx.y = (FLOAT)(space.yStep * i);
				vx.z = m_depths[1][i * space.nX + j];			
					
				vx -= vec3;
				vx.z *= app.stg._g_set.zcoef;
				vx.dw = app.stg._g_set.dw_cell;;
				pvertices[m_pbottomSBuffer[nbuff].vlength] = vx;				
				m_pbottomSBuffer[nbuff].vlength++;
			}
		}				
		m_pbottomSBuffer[nbuff].vb->Unlock();
		i0 += i1;
	}		
	return hres;
}
HRESULT CDXSlice::sideBuffer()
{
	HRESULT hres = 0;		

	DWORD* pindices;	
	D3DVERTEX *pvertices, *prvertices;		
	
	D3DVERTEX vx;
	int j;
	UINT i;	

	if (ug & UG_CREATE)
	{
		m_sideBuffer.ilength = (_xyzDim.xLen() + _xyzDim.yLen() + 2) << 2;
		m_sideBuffer.vlength = 
		m_sideRBuffer.vlength = (_xyzDim.xLen() + _xyzDim.yLen() + 2) << 2; 

		if FAILED(hres = lpd3d->CreateIndexBuffer(m_sideBuffer.ilength << 2,
			0, D3DFMT_INDEX32, D3DPOOL_MANAGED, &m_sideBuffer.ib, 0)) return hres;				
		if FAILED(hres = lpd3d->CreateVertexBuffer(m_sideBuffer.vlength << 4,
			0, VERTEX_FVF, D3DPOOL_MANAGED, &m_sideBuffer.vb, 0)) return hres;
		if FAILED(hres = lpd3d->CreateVertexBuffer(m_sideRBuffer.vlength << 4,
			0, VERTEX_FVF, D3DPOOL_MANAGED, &m_sideRBuffer.vb, 0)) return hres;
	}		
	m_sideBuffer.ib->Lock(0, m_sideBuffer.ilength << 2, (VOID**)&pindices, 0);
	m_sideBuffer.vb->Lock(0, m_sideBuffer.vlength << 4, (VOID**)&pvertices, 0);								
	m_sideRBuffer.vb->Lock(0, m_sideRBuffer.vlength << 4, (VOID**)&prvertices, 0);								
	
	m_sideBuffer.ilength = m_sideBuffer.vlength = m_sideRBuffer.vlength = 0;									
	// left		
	m_desc[1].startIndex = m_sideBuffer.ilength;
	i = _xyzDim.xDim.cx;						
	for (j = _xyzDim.yDim.cx; j < _xyzDim.yDim.cy + 1; j++)	
	{			
		pindices[m_sideBuffer.ilength] = m_sideBuffer.vlength;
		m_sideBuffer.ilength++;													
			
		vx.x = (FLOAT)(space.xStep * i);
		vx.y = (FLOAT)(space.yStep * j);
		vx.z = m_depths[0][j * space.nX + i];
			
		vx -= vec3;
		vx.z *= app.stg._g_set.zcoef;
		vx.dw = m_colors[0][j * space.nX + i];			

		pvertices[m_sideBuffer.vlength] = vx;			
		m_sideBuffer.vlength++;		
			
		vx.dw = RGB16(m_index);

		prvertices[m_sideRBuffer.vlength] = vx;			
		m_sideRBuffer.vlength++;		
		//
		pindices[m_sideBuffer.ilength] = m_sideBuffer.vlength;
		m_sideBuffer.ilength++;									

		vx.x = (FLOAT)(space.xStep * i);
		vx.y = (FLOAT)(space.yStep * j);
		vx.z = m_depths[1][j * space.nX + i];

		vx -= vec3;
		vx.z *= app.stg._g_set.zcoef;
		vx.dw = m_colors[1][j * space.nX + i];			

		pvertices[m_sideBuffer.vlength] = vx;			
		m_sideBuffer.vlength++;		
			
		vx.dw = RGB16(m_index);

		prvertices[m_sideRBuffer.vlength] = vx;			
		m_sideRBuffer.vlength++;					
	}
	m_desc[1].primitiveCount = _xyzDim.yLen() << 1; 
	// right
	m_desc[2].startIndex = m_sideBuffer.ilength;		
	i = _xyzDim.xDim.cy;							
	for (j = _xyzDim.yDim.cx; j < _xyzDim.yDim.cy + 1; j++)
	{
		pindices[m_sideBuffer.ilength] = m_sideBuffer.vlength;
		m_sideBuffer.ilength++;			
			
		vx.x = (FLOAT)(space.xStep * i);
		vx.y = (FLOAT)(space.yStep * j);
		vx.z = m_depths[1][j * space.nX + i];

		vx -= vec3;
		vx.z *= app.stg._g_set.zcoef;
		vx.dw = m_colors[1][j * space.nX + i];

		pvertices[m_sideBuffer.vlength] = vx;			
		m_sideBuffer.vlength++;		
			
		vx.dw = RGB16(m_index);

		prvertices[m_sideRBuffer.vlength] = vx;			
		m_sideRBuffer.vlength++;	
		//
		pindices[m_sideBuffer.ilength] = m_sideBuffer.vlength;
		m_sideBuffer.ilength++;		
			
		vx.x = (FLOAT)(space.xStep * i);
		vx.y = (FLOAT)(space.yStep * j);
		vx.z = m_depths[0][j * space.nX + i];

		vx -= vec3;
		vx.z *= app.stg._g_set.zcoef;
		vx.dw = m_colors[0][j * space.nX + i];

		pvertices[m_sideBuffer.vlength] = vx;			
		m_sideBuffer.vlength++;		
			
		vx.dw = RGB16(m_index);
			
		prvertices[m_sideRBuffer.vlength] = vx;			
		m_sideRBuffer.vlength++;	
	}		
	m_desc[2].primitiveCount = _xyzDim.yLen() << 1; 
	// front
	m_desc[3].startIndex = m_sideBuffer.ilength;
	i = _xyzDim.yDim.cx;										
	for (j = _xyzDim.xDim.cx; j < _xyzDim.xDim.cy + 1; j++)
	{
		pindices[m_sideBuffer.ilength] = m_sideBuffer.vlength;
		m_sideBuffer.ilength++;		
			
		vx.x = (FLOAT)(space.xStep * j);
		vx.y = (FLOAT)(space.yStep * i);
		vx.z = m_depths[1][i * space.nX + j];

		vx -= vec3;
		vx.z *= app.stg._g_set.zcoef;
		vx.dw = m_colors[1][i * space.nX + j];

		pvertices[m_sideBuffer.vlength] = vx;			
		m_sideBuffer.vlength++;		
			
		vx.dw = RGB16(m_index);

		prvertices[m_sideRBuffer.vlength] = vx;			
		m_sideRBuffer.vlength++;	
		//
		pindices[m_sideBuffer.ilength] = m_sideBuffer.vlength;
		m_sideBuffer.ilength++;									
			
		vx.x = (FLOAT)(space.xStep * j);
		vx.y = (FLOAT)(space.yStep * i);
		vx.z = m_depths[0][i * space.nX + j];							

		vx -= vec3;
		vx.z *= app.stg._g_set.zcoef;
		vx.dw = m_colors[0][i * space.nX + j];

		pvertices[m_sideBuffer.vlength] = vx;			
		m_sideBuffer.vlength++;		
			
		vx.dw = RGB16(m_index);
			
		prvertices[m_sideRBuffer.vlength] = vx;			
		m_sideRBuffer.vlength++;	
	}
	m_desc[3].primitiveCount = _xyzDim.xLen() << 1; 
	// back
	m_desc[4].startIndex = m_sideBuffer.ilength;
	i = _xyzDim.yDim.cy;						
	for (j = _xyzDim.xDim.cx; j < _xyzDim.xDim.cy + 1; j++)
	{
		pindices[m_sideBuffer.ilength] = m_sideBuffer.vlength;
		m_sideBuffer.ilength++;									
			
		vx.x = (FLOAT)(space.xStep * j);
		vx.y = (FLOAT)(space.yStep * i);
		vx.z = m_depths[0][i * space.nX + j];					

		vx -= vec3;
		vx.z *= app.stg._g_set.zcoef;
		vx.dw = m_colors[0][i * space.nX + j];

		pvertices[m_sideBuffer.vlength] = vx;			
		m_sideBuffer.vlength++;		
			
		vx.dw = RGB16(m_index);

		prvertices[m_sideRBuffer.vlength] = vx;			
		m_sideRBuffer.vlength++;	
			//
		pindices[m_sideBuffer.ilength] = m_sideBuffer.vlength;
		m_sideBuffer.ilength++;													
			
		vx.x = (FLOAT)(space.xStep * j);
		vx.y = (FLOAT)(space.yStep * i);
		vx.z = m_depths[1][i * space.nX + j];				

		vx -= vec3;
		vx.z *= app.stg._g_set.zcoef;
		vx.dw = m_colors[1][i * space.nX + j];

		pvertices[m_sideBuffer.vlength] = vx;			
		m_sideBuffer.vlength++;		
			
		vx.dw = RGB16(m_index);

		prvertices[m_sideRBuffer.vlength] = vx;			
		m_sideRBuffer.vlength++;	
	}
	m_desc[4].primitiveCount = _xyzDim.xLen() << 1; 

	m_sideBuffer.ib->Unlock();
	m_sideBuffer.vb->Unlock();
	m_sideRBuffer.vb->Unlock();			
	
	return hres;
}
HRESULT CDXSlice::sideSBuffer()
{
	HRESULT hres = S_OK;

	DWORD* pindices;	
	D3DVERTEX* pvertices;		
	
	D3DVERTEX vx;	
	int j;
	UINT i, k = (_xyzDim.xLen() + _xyzDim.yLen() + 2) << 1;	
	
	if (ug & UG_CREATE)
	{
		m_sideSBuffer.ilength = k + ((_xyzDim.xLen() + _xyzDim.yLen() + 2) << 2);
		m_sideSBuffer.vlength = (_xyzDim.xLen() + _xyzDim.yLen() + 2) << 2;

		if FAILED(hres = lpd3d->CreateIndexBuffer(m_sideSBuffer.ilength << 2,
			0, D3DFMT_INDEX32, D3DPOOL_MANAGED, &m_sideSBuffer.ib, 0)) return hres;				
		if FAILED(hres = lpd3d->CreateVertexBuffer(m_sideSBuffer.vlength << 4,
			0, VERTEX_FVF, D3DPOOL_MANAGED, &m_sideSBuffer.vb, 0)) return hres;		
	}
		
	m_sideSBuffer.ib->Lock(0, m_sideSBuffer.ilength << 2, (VOID**)&pindices, 0);
	m_sideSBuffer.vb->Lock(0, m_sideSBuffer.vlength << 4, (VOID**)&pvertices, 0);										
	
	m_sideSBuffer.ilength = m_sideSBuffer.vlength = 0;						
	// left		
	m_sdesc[1].startIndex = m_sideSBuffer.ilength;
	i = _xyzDim.xDim.cx;
	k = (_xyzDim.xLen() + _xyzDim.yLen() + 2) << 2;
	for (j = _xyzDim.yDim.cx; j < _xyzDim.yDim.cy + 1; j++)	
	{
		pindices[m_sideSBuffer.ilength] = m_sideSBuffer.ilength;
		m_sideSBuffer.ilength++;									
			
		vx.x = (FLOAT)(space.xStep * i);
		vx.y = (FLOAT)(space.yStep * j);
		vx.z = m_depths[0][j * space.nX + i];

		vx -= vec3;
		vx.z *= app.stg._g_set.zcoef;
		vx.dw = app.stg._g_set.dw_cell;
			
		pvertices[m_sideSBuffer.vlength] = vx;			
		m_sideSBuffer.vlength++;

		pindices[m_sideSBuffer.ilength] = m_sideSBuffer.ilength;
		// frame
		pindices[k] = m_sideSBuffer.ilength;
		k++;
		m_sideSBuffer.ilength++;													
			
		vx.x = (FLOAT)(space.xStep * i);
		vx.y = (FLOAT)(space.yStep * j);
		vx.z = m_depths[1][j * space.nX + i];

		vx -= vec3;
		vx.z *= app.stg._g_set.zcoef;
		vx.dw = app.stg._g_set.dw_cell;

		pvertices[m_sideSBuffer.vlength] = vx;
		m_sideSBuffer.vlength++;			
	}
	m_sdesc[1].primitiveCount = 1;	
	// right
	m_sdesc[2].startIndex = m_sideSBuffer.ilength;
	i = _xyzDim.xDim.cy;			
	for (j = _xyzDim.yDim.cx; j < _xyzDim.yDim.cy + 1; j++)
	{								
		pindices[m_sideSBuffer.ilength] = m_sideSBuffer.vlength;
		m_sideSBuffer.ilength++;									
			
		vx.x = (FLOAT)(space.xStep * i);
		vx.y = (FLOAT)(space.yStep * j);
		vx.z = m_depths[0][j * space.nX + i];

		vx -= vec3;
		vx.z *= app.stg._g_set.zcoef;
		vx.dw = app.stg._g_set.dw_cell;
			
		pvertices[m_sideSBuffer.vlength] = vx;			
		m_sideSBuffer.vlength++;

		pindices[m_sideSBuffer.ilength] = m_sideSBuffer.ilength;		
		// frame
		pindices[k] = m_sideSBuffer.ilength;
		k++;
		m_sideSBuffer.ilength++;													
			
		vx.x = (FLOAT)(space.xStep * i);
		vx.y = (FLOAT)(space.yStep * j);
		vx.z = m_depths[1][j * space.nX + i];

		vx -= vec3;
		vx.z *= app.stg._g_set.zcoef;
		vx.dw = app.stg._g_set.dw_cell;

		pvertices[m_sideSBuffer.vlength] = vx;
		m_sideSBuffer.vlength++;						
	}		
	m_sdesc[2].primitiveCount = 1;
	// front
	m_sdesc[3].startIndex = m_sideSBuffer.ilength;		
	i = _xyzDim.yDim.cx;										
	for (j = _xyzDim.xDim.cx; j < _xyzDim.xDim.cy + 1; j++)	
	{
		pindices[m_sideSBuffer.ilength] = m_sideSBuffer.vlength;
		m_sideSBuffer.ilength++;
			
		vx.x = (FLOAT)(space.xStep * j);
		vx.y = (FLOAT)(space.yStep * i);
		vx.z = m_depths[0][i * space.nX + j];

		vx -= vec3;
		vx.z *= app.stg._g_set.zcoef;
		vx.dw = app.stg._g_set.dw_cell;

		pvertices[m_sideSBuffer.vlength] = vx;
		m_sideSBuffer.vlength++;

		pindices[m_sideSBuffer.ilength] = m_sideSBuffer.vlength;		
		// frame
		pindices[k] = m_sideSBuffer.ilength;
		k++;
		m_sideSBuffer.ilength++;																
			
		vx.x = (FLOAT)(space.xStep * j);
		vx.y = (FLOAT)(space.yStep * i);
		vx.z = m_depths[1][i * space.nX + j];

		vx -= vec3;
		vx.z *= app.stg._g_set.zcoef;
		vx.dw = app.stg._g_set.dw_cell;

		pvertices[m_sideSBuffer.vlength] = vx;
		m_sideSBuffer.vlength++;		
	}
	m_sdesc[3].primitiveCount = 1;
	// back
	m_sdesc[4].startIndex = m_sideSBuffer.ilength;		
	i = _xyzDim.yDim.cy;										
	for (j = _xyzDim.xDim.cx; j < _xyzDim.xDim.cy + 1; j++)	
	{
		pindices[m_sideSBuffer.ilength] = m_sideSBuffer.vlength;
		m_sideSBuffer.ilength++;
			
		vx.x = (FLOAT)(space.xStep * j);
		vx.y = (FLOAT)(space.yStep * i);
		vx.z = m_depths[0][i * space.nX + j];

		vx -= vec3;
		vx.z *= app.stg._g_set.zcoef;
		vx.dw = app.stg._g_set.dw_cell;

		pvertices[m_sideSBuffer.vlength] = vx;
		m_sideSBuffer.vlength++;

		pindices[m_sideSBuffer.ilength] = m_sideSBuffer.vlength;		
		// frame
		pindices[k] = m_sideSBuffer.ilength;
		k++;
		m_sideSBuffer.ilength++;																
			
		vx.x = (FLOAT)(space.xStep * j);
		vx.y = (FLOAT)(space.yStep * i);
		vx.z = m_depths[1][i * space.nX + j];

		vx -= vec3;
		vx.z *= app.stg._g_set.zcoef;
		vx.dw = app.stg._g_set.dw_cell;

		pvertices[m_sideSBuffer.vlength] = vx;
		m_sideSBuffer.vlength++;		
	}
	m_sdesc[4].primitiveCount = 1;

	m_sideSBuffer.ib->Unlock();
	m_sideSBuffer.vb->Unlock();		

	return hres;
}
HRESULT CDXSlice::topBuffer()
{
	HRESULT hres = 0;
	
	DWORD* pindices;	
	D3DVERTEX* pvertices, *prvertices;		
	
	D3DVERTEX vx;
	int jx;
	UINT i0 = _xyzDim.yDim.cx, i1, iy; 		 
		
	for (UCHAR nbuff = 0; nbuff < m_buffSize; nbuff++)
	{				
		i1 = app.m_graph.m_rows[nbuff];													
		if (ug & UG_CREATE)
		{
			m_ptopBuffer[nbuff].ilength = ((_xyzDim.xLen() + 1) << 1) * i1;
			m_ptopBuffer[nbuff].vlength = 
			m_ptopRBuffer[nbuff].vlength = (_xyzDim.xLen() + 1) * (i1 + 1);								 
			
			if FAILED(hres = lpd3d->CreateIndexBuffer(m_ptopBuffer[nbuff].ilength << 2,
					0, D3DFMT_INDEX32, D3DPOOL_MANAGED, 
					&m_ptopBuffer[nbuff].ib, 0)) return hres;					
			if FAILED(hres = lpd3d->CreateVertexBuffer(m_ptopBuffer[nbuff].vlength << 4,
					0, VERTEX_FVF, D3DPOOL_MANAGED, 
					&m_ptopBuffer[nbuff].vb, 0)) return hres;							
			if FAILED(hres = lpd3d->CreateVertexBuffer(m_ptopRBuffer[nbuff].vlength << 4,
					0, VERTEX_FVF, D3DPOOL_MANAGED, 
					&m_ptopRBuffer[nbuff].vb, 0)) return hres;
		}
		m_ptopBuffer[nbuff].ib->Lock(0, m_ptopBuffer[nbuff].ilength << 2, 
				(VOID**)&pindices, 0);
		m_ptopBuffer[nbuff].vb->Lock(0, m_ptopBuffer[nbuff].vlength << 4, 
				(VOID**)&pvertices, 0);	
		m_ptopRBuffer[nbuff].vb->Lock(0, m_ptopRBuffer[nbuff].vlength << 4, 
					(VOID**)&prvertices, 0);								
			
		m_ptopBuffer[nbuff].ilength = m_ptopBuffer[nbuff].vlength = 
		m_ptopRBuffer[nbuff].vlength = 0;
			
		for (iy = i0; iy < i0 + i1; iy++)
		{
			for (jx = _xyzDim.xDim.cx; jx < _xyzDim.xDim.cy + 1; jx++)
			{					
				pindices[m_ptopBuffer[nbuff].ilength] = m_ptopBuffer[nbuff].vlength;
				m_ptopBuffer[nbuff].ilength++;						
				pindices[m_ptopBuffer[nbuff].ilength] = m_ptopBuffer[nbuff].vlength + 
					(_xyzDim.xLen() + 1);						
				m_ptopBuffer[nbuff].ilength++;			
										
				vx.x = (FLOAT)(space.xStep * jx);
				vx.y = (FLOAT)(space.yStep * iy);
				vx.z = m_depths[0][iy * space.nX + jx];		
						
				vx -= vec3;				
				vx.z *= app.stg._g_set.zcoef;
				vx.dw = m_colors[0][iy * space.nX + jx];				

				pvertices[m_ptopBuffer[nbuff].vlength] = vx;				
				m_ptopBuffer[nbuff].vlength++;
					
				vx.dw = RGB16(m_index);

				prvertices[m_ptopRBuffer[nbuff].vlength] = vx;
				m_ptopRBuffer[nbuff].vlength++;					
			}					
		}
		m_ptopBuffer[nbuff].ib->Unlock();	
		// remove if from circle - last row without indices
		for (jx = _xyzDim.xDim.cx; jx < _xyzDim.xDim.cy + 1; jx++)
		{						
			vx.x = (FLOAT)(space.xStep * jx);
			vx.y = (FLOAT)(space.yStep * iy);
			vx.z = m_depths[0][iy * space.nX + jx];

			vx -= vec3;
			vx.z *= app.stg._g_set.zcoef;
			vx.dw = m_colors[0][iy * space.nX + jx];

			pvertices[m_ptopBuffer[nbuff].vlength] = vx;	
			m_ptopBuffer[nbuff].vlength++;
						
			vx.dw = RGB16(m_index);

			prvertices[m_ptopRBuffer[nbuff].vlength] = vx;					
			m_ptopRBuffer[nbuff].vlength++;
		}						
		m_ptopBuffer[nbuff].vb->Unlock();									
		m_ptopRBuffer[nbuff].vb->Unlock();									
		i0 += i1;		
	}	
	
	return hres;
}
HRESULT CDXSlice::topSBuffer()
{
	HRESULT hres = S_OK;
	D3DVERTEX* pvertices;		
	
	D3DVERTEX vx;	
	int j;
	UINT i, i0 = _xyzDim.yDim.cx, i1;	
	
	for (UCHAR nbuff = 0; nbuff < m_buffSize; nbuff++)
	{
		i1 = app.m_graph.m_rows[nbuff];
		if (ug & UG_CREATE)
		{
			m_ptopSBuffer[nbuff].vlength = (_xyzDim.xLen() + 1) * (i1 + 1);								
				
			if FAILED(hres = lpd3d->CreateVertexBuffer(m_ptopSBuffer[nbuff].vlength << 4,
					0, VERTEX_FVF, D3DPOOL_MANAGED, &m_ptopSBuffer[nbuff].vb, 0)) return hres;												
		}
		m_ptopSBuffer[nbuff].vb->Lock(0, m_ptopSBuffer[nbuff].vlength << 4, (VOID**)&pvertices, 0);											
				
		m_ptopSBuffer[nbuff].vlength = 0;						
		for (i = i0; i < i0 + i1 + 1; i++)
		{
			for (j = _xyzDim.xDim.cx; j < _xyzDim.xDim.cy + 1; j++)
			{							
				vx.x = (FLOAT)(space.xStep * j);
				vx.y = (FLOAT)(space.yStep * i);
				vx.z = m_depths[0][i * space.nX + j];
					
				vx -= vec3;
				vx.z *= app.stg._g_set.zcoef;
				vx.dw = app.stg._g_set.dw_cell;
				pvertices[m_ptopSBuffer[nbuff].vlength] = vx;				
				m_ptopSBuffer[nbuff].vlength++;
			}
		}				
		m_ptopSBuffer[nbuff].vb->Unlock();
		i0 += i1;
	}	
	return hres;
}
HRESULT CDXSlice::updateBuffer()
{	
	HRESULT hres = S_OK;			
	// read stream data
	if (ug & UG_READ)
	{			
		CString filename = (szParam.cx == 0) ? app.m_graph.m_structfile.fext : 
			app.m_graph.m_inilines[szParam.cx - 1]->fext;	
		if (!readStream(filename)) return E_FAIL;	
	}	
	m_desc[0].startIndex = 0;
	m_desc[0].primitiveCount = _xyzDim.xLen() << 1;

	if FAILED(hres = sideBuffer()) return hres;
	
	if (m_bdraw.bdrawTopNeeded) 
		if FAILED(hres = topBuffer()) return hres;
	if (m_bdraw.bdrawBottomNeeded) 
		if FAILED(hres = bottomBuffer()) return hres;	
	m_bfree.bfreeBuffersNeeded = 1;	
	
	if (ug & UG_CREATE | UG_SKEL | UG_ZCOEF) updateSkeleton();			
	return hres;
}
HRESULT CDXSlice::updateSkeleton()
{
	HRESULT hres = S_OK;		
	DWORD* pindices;		
	int i, i0 = _xyzDim.yDim.cx, i1, j;	
		
	m_sdesc[0].startIndex = 0;
	m_sdesc[0].primitiveCount = _xyzDim.xLen(); 			
	for (UCHAR nbuff = 0; nbuff < m_buffSize; nbuff++)
	{			
		i1 = app.m_graph.m_rows[nbuff];
		if (ug & UG_CREATE)
		{
			m_piSBuffer[nbuff].ilength = (_xyzDim.xLen() + 1) * (i1 + 1);
			
			if FAILED(hres = lpd3d->CreateIndexBuffer(m_piSBuffer[nbuff].ilength << 2,
				D3DUSAGE_WRITEONLY, D3DFMT_INDEX32, D3DPOOL_MANAGED, &m_piSBuffer[nbuff].ib, 0)) return hres;					
			m_paddiSBuffer[nbuff].ilength = (_xyzDim.xLen() + 1) * (i1 + 1);			
			// additional index buffer
			if FAILED(hres = lpd3d->CreateIndexBuffer(m_paddiSBuffer[nbuff].ilength << 2,
				D3DUSAGE_WRITEONLY, D3DFMT_INDEX32, D3DPOOL_MANAGED, &m_paddiSBuffer[nbuff].ib, 0)) return hres;		
		}		
		m_piSBuffer[nbuff].ib->Lock(0, m_piSBuffer[nbuff].ilength << 2, (VOID**)&pindices, 0);
		
		m_piSBuffer[nbuff].ilength = 0;		
		for (i = i0; i < i0 + i1 + 1; i++)
		{
			for (j = _xyzDim.xDim.cx; j < _xyzDim.xDim.cy + 1; j++)
			{				
				pindices[m_piSBuffer[nbuff].ilength] = m_piSBuffer[nbuff].ilength;
				m_piSBuffer[nbuff].ilength++;
			}
		}
		m_piSBuffer[nbuff].ib->Unlock();	
		// additional index buffer			
		m_paddiSBuffer[nbuff].ib->Lock(0, m_paddiSBuffer[nbuff].ilength << 2, (VOID**)&pindices, 0);
			
		m_paddiSBuffer[nbuff].ilength = 0;
		m_paddiSBuffer[nbuff].startIndex = 0;			

		for (i = 0; i < _xyzDim.xLen() + 1; i++)
		{	
			for (j = 0; j < i1 + 1; j++)
			{
				pindices[m_paddiSBuffer[nbuff].ilength] = i + j * (_xyzDim.xLen() + 1);
				m_paddiSBuffer[nbuff].ilength++;						
			}
		}
		m_paddiSBuffer[nbuff].ib->Unlock();
		m_paddiSBuffer[nbuff].primitiveCount = i1;	
		i0 += i1;
	}
	
	if FAILED(hres = sideSBuffer()) return hres;
	if (m_bdraw.bdrawBottomNeeded) 
		if FAILED(hres = bottomSBuffer()) return hres;;
	if (m_bdraw.bdrawTopNeeded) 
		if FAILED(hres = topSBuffer()) return hres;
	m_bfree.bfreeSBuffersNeeded = 1;	

	return hres;
}
void CDXSlice::freeBuffers()
{
	for (UCHAR i = 0; i < m_buffSize; i++)
	{			
		SAFE_RELEASE(m_pbottomBuffer[i].ib);
		SAFE_RELEASE(m_pbottomBuffer[i].vb);
		SAFE_RELEASE(m_ptopBuffer[i].ib);
		SAFE_RELEASE(m_ptopBuffer[i].vb);			
		// offsceen
		SAFE_RELEASE(m_pbottomRBuffer[i].vb);
		SAFE_RELEASE(m_ptopRBuffer[i].vb);			
	}			
	SAFE_DELETE_ARRAY(m_pbottomBuffer);	
	SAFE_DELETE_ARRAY(m_ptopBuffer);

	SAFE_RELEASE(m_sideBuffer.ib);	
	SAFE_RELEASE(m_sideBuffer.vb);		
	SAFE_RELEASE(m_sideRBuffer.vb);		

	SAFE_DELETE_ARRAY(m_pbottomRBuffer);
	SAFE_DELETE_ARRAY(m_ptopRBuffer);	
	
	m_bfree.bfreeBuffersNeeded = 0;
}
void CDXSlice::freeSBuffers()
{	
	for (UCHAR i = 0; i < m_buffSize; i++)
	{		
		SAFE_RELEASE(m_paddiSBuffer[i].ib);
		SAFE_RELEASE(m_piSBuffer[i].ib);
		SAFE_RELEASE(m_pbottomSBuffer[i].vb);
		SAFE_RELEASE(m_ptopSBuffer[i].vb);						
	}		
	SAFE_DELETE_ARRAY(m_paddiSBuffer);		
	SAFE_DELETE_ARRAY(m_piSBuffer);		
	SAFE_DELETE_ARRAY(m_pbottomSBuffer);	
	SAFE_DELETE_ARRAY(m_ptopSBuffer);		

	SAFE_RELEASE(m_sideSBuffer.ib);
	SAFE_RELEASE(m_sideSBuffer.vb);		
	
	m_bfree.bfreeSBuffersNeeded = 0;
}
void CDXSlice::setBuff(UCHAR buffSize)
{
	if (m_bfree.bfreeBuffersNeeded) freeBuffers();	
	if (m_bfree.bfreeSBuffersNeeded) freeSBuffers();			

	m_buffSize = buffSize;
	if (m_buffSize == 0) return;

	m_pbottomBuffer = new ivBuffer[m_buffSize];				
	m_ptopBuffer = new ivBuffer[m_buffSize];	
	// offscreen
	m_pbottomRBuffer = new vBuffer[m_buffSize];	
	m_ptopRBuffer = new vBuffer[m_buffSize];	
	
	m_paddiSBuffer = new dBuffer[m_buffSize];
	m_piSBuffer = new iBuffer[m_buffSize];				
	m_pbottomSBuffer = new vBuffer[m_buffSize];				
	m_ptopSBuffer = new vBuffer[m_buffSize];			
}
// CCylMesh
CCylMesh::CCylMesh()
{
	m_sectors = 16;
}
CCylMesh::~CCylMesh()
{
	_free();
}
void CCylMesh::_free()
{	
	SAFE_RELEASE(m_buffer.ib);
	SAFE_RELEASE(m_buffer.vb);	
}
void CCylMesh::init(D3DXVECTOR3* pvtop, D3DXVECTOR3* pvbottom)
{		
	m_vtop = *pvtop;
	m_vbottom = * pvbottom;		
}
bool CCylMesh::isectBottom(const D3DXVECTOR3* ppos, const D3DXVECTOR3* pdir, D3DXVECTOR3* pv)
{	
	WORD* pindices;	
	D3DVERTEX* pvertices;	
	D3DXVECTOR3 v0, v1, v2;	

	m_buffer.ib->Lock(0, 0, (VOID**)&pindices, 0);
	m_buffer.vb->Lock(0, 0, (VOID**)&pvertices, 0);	
	m_buffer.ib->Unlock();
	m_buffer.vb->Unlock();

	v0.x = pvertices[m_sectors + 1].x;
	v0.y = pvertices[m_sectors + 1].y;
	v0.z = pvertices[m_sectors + 1].z;

	UCHAR i = 1;
	for (i; i < m_sectors; i++)
	{
		v1.x = pvertices[m_sectors + 1 + i].x;
		v1.y = pvertices[m_sectors + 1 + i].y;
		v1.z = pvertices[m_sectors + 1 + i].z;
		
		v2.x = pvertices[m_sectors + 1 + (i + 1)].x;
		v2.y = pvertices[m_sectors + 1 + (i + 1)].y;
		v2.z = pvertices[m_sectors + 1 + (i + 1)].z;

		if (hitTri(&v0, &v1, &v2, ppos, pdir, pv)) return 1;
	}	
	// last triangle
	v1.x = pvertices[m_sectors + 1 + i].x;
	v1.y = pvertices[m_sectors + 1 + i].y;
	v1.z = pvertices[m_sectors + 1 + i].z;
		
	v2.x = pvertices[m_sectors + 2].x;
	v2.y = pvertices[m_sectors + 2].y;
	v2.z = pvertices[m_sectors + 2].z;	
		
	if (hitTri(&v0, &v1, &v2, ppos, pdir, pv)) return 1;
	
	return 0;
}
bool CCylMesh::isectTop(const D3DXVECTOR3* ppos, const D3DXVECTOR3* pdir, D3DXVECTOR3* pv)
{	
	WORD* pindices;	
	D3DVERTEX* pvertices;	
	D3DXVECTOR3 v0, v1, v2;		

	m_buffer.ib->Lock(0, 0, (VOID**)&pindices, 0);
	m_buffer.vb->Lock(0, 0, (VOID**)&pvertices, 0);	
	m_buffer.ib->Unlock();
	m_buffer.vb->Unlock();

	v0.x = pvertices[0].x;
	v0.y = pvertices[0].y;
	v0.z = pvertices[0].z;

	UCHAR i = 1;
	for (i; i < m_sectors; i++)
	{
		v1.x = pvertices[i].x;
		v1.y = pvertices[i].y;
		v1.z = pvertices[i].z;
		
		v2.x = pvertices[i + 1].x;
		v2.y = pvertices[i + 1].y;
		v2.z = pvertices[i + 1].z;

		if (hitTri(&v0, &v1, &v2, ppos, pdir, pv)) return 1;
	}	
	// last triangle
	v1.x = pvertices[i].x;
	v1.y = pvertices[i].y;
	v1.z = pvertices[i].z;
		
	v2.x = pvertices[1].x;
	v2.y = pvertices[1].y;
	v2.z = pvertices[1].z;	
		
	if (hitTri(&v0, &v1, &v2, ppos, pdir, pv)) return 1;
	
	return 0;
}
bool CCylMesh::isectSide(const D3DXVECTOR3* ppos, const D3DXVECTOR3* pdir, D3DXVECTOR3* pv)
{
	WORD* pindices;	
	D3DVERTEX* pvertices;	
	D3DXVECTOR3 v0, v1, v2;	
	std::vector<D3DXVECTOR3> vectors;
		
	m_buffer.ib->Lock(0, 0, (VOID**)&pindices, 0);
	m_buffer.vb->Lock(0, 0, (VOID**)&pvertices, 0);	
	m_buffer.ib->Unlock();
	m_buffer.vb->Unlock();

	UCHAR i = 1;
	for (i; i < m_sectors - 1; i++)
	{			
		v0.x = pvertices[i].x;
		v0.y = pvertices[i].y;
		v0.z = pvertices[i].z;

		v1.x = pvertices[i + 1].x;
		v1.y = pvertices[i + 1].y;
		v1.z = pvertices[i + 1].z;

		v2.x = pvertices[m_sectors + 1 + i].x;
		v2.y = pvertices[m_sectors + 1 + i].y;
		v2.z = pvertices[m_sectors + 1 + i].z;
		
		if (hitTri(&v0, &v1, &v2, ppos, pdir, pv)) vectors.push_back(*pv);

		v0.x = pvertices[m_sectors + 1 + i].x;
		v0.y = pvertices[m_sectors + 1 + i].y;
		v0.z = pvertices[m_sectors + 1 + i].z;

		v1.x = pvertices[i + 1].x;
		v1.y = pvertices[i + 1].y;
		v1.z = pvertices[i + 1].z;

		v2.x = pvertices[m_sectors + 1 + (i + 1)].x;
		v2.y = pvertices[m_sectors + 1 + (i + 1)].y;
		v2.z = pvertices[m_sectors + 1 + (i + 1)].z;

		if (hitTri(&v0, &v1, &v2, ppos, pdir, pv)) vectors.push_back(*pv);
	}	
	UCHAR index, nsize = (UCHAR)vectors.size();	
	if (nsize == 0) return 0;
	// check for min intersection
	D3DXVECTOR3 vlen;
	float f0 = FLT_MAX, f1;
	for (i = 0; i < nsize; i++)
	{		
		vlen = *ppos - vectors[i];
		f1 = D3DXVec3Length(&vlen);
		if (f1 < f0) 
		{
			f0 = f1; 
			index = i;		
		}
	}		
	pv->x = vectors[index].x;
	pv->y = vectors[index].y;
	pv->z = vectors[index].z;
	vectors.clear();

	return 1;
}
HRESULT CCylMesh::update()
{	
	D3DVERTEX* pvertices;		
	WORD* pindices;	

	HRESULT hres = S_OK;		
	
	if (uw)
	{
		_free();
		m_buffer.ilength = (m_sectors << 2) + 6;
		m_buffer.vlength = (m_sectors + 1) << 1;		

		if (FAILED(hres = lpd3d->CreateIndexBuffer(m_buffer.ilength << 1, 0, 
			D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_buffer.ib, 0))) return hres;
		if (FAILED(hres = lpd3d->CreateVertexBuffer(m_buffer.vlength << 4, 0, 
			VERTEX_FVF, D3DPOOL_MANAGED, &m_buffer.vb, 0))) return hres;
	}
	m_buffer.ib->Lock(0, m_buffer.ilength << 1, (VOID**)&pindices, 0); 
	m_buffer.vb->Lock(0, m_buffer.vlength << 4, (VOID**)&pvertices, 0); 	
	// top circle
	pindices[0] = 0;		
	pvertices[0] = m_vtop - vec3; 
	pvertices[0].z *= app.stg._g_set.zcoef;
	pvertices[0].dw = m_dw;	

	D3DXVECTOR3 v;			
	float alpha = 2 * D3DX_PI;
	UCHAR i;

	for (i = 0; i < m_sectors; i++)
	{
		v = D3DXVECTOR3(cosf(alpha) * m_radius, sinf(alpha) * m_radius, 0.0f);
		
		pindices[i + 1] = i + 1;				
		pvertices[i + 1] = v + m_vtop - vec3;	
		pvertices[i + 1].z *= app.stg._g_set.zcoef;
		pvertices[i + 1].dw = m_dw;					
		
		alpha -= 2 * D3DX_PI/m_sectors;
	}
	pindices[i + 1] = 1;	
	// bottom circle
	pindices[m_sectors + 2] = m_sectors + 1;	
	pvertices[m_sectors + 1] = m_vbottom - vec3;
	pvertices[m_sectors + 1].z *= app.stg._g_set.zcoef;
	pvertices[m_sectors + 1].dw = m_dw;
	
	for (i = 0; i < m_sectors; i++)
	{
		v = D3DXVECTOR3(cosf(alpha) * m_radius, sinf(alpha) * m_radius, 0.0f);		
		
		pindices[m_sectors + 2 + (i + 1)] = (m_sectors << 1) - (i - 1);
		pvertices[m_sectors + 1 + (i + 1)] = v + m_vbottom - vec3;
		pvertices[m_sectors + 1 + (i + 1)].z *= app.stg._g_set.zcoef;
		pvertices[m_sectors + 1 + (i + 1)].dw = m_dw;		
		
		alpha -= 2 * D3DX_PI/m_sectors;
	}
	pindices[m_sectors + 2 + (i + 1)] = (m_sectors << 1) + 1;	
	// side
	m_buffer.ilength = (m_sectors + 2) << 1;
	for (i = 0; i < m_sectors; i++)
	{		
		pindices[m_buffer.ilength] = i + 1;
		m_buffer.ilength++;		
		pindices[m_buffer.ilength] = m_sectors + 1 + (i + 1);
		m_buffer.ilength++;			
	}
	pindices[m_buffer.ilength] = 1;
	m_buffer.ilength++;			
	pindices[m_buffer.ilength] = m_sectors + 2;
	m_buffer.ilength++;			

	m_buffer.ib->Unlock();
	m_buffer.vb->Unlock();	

	return hres;
}
// CConeMesh
void CConeMesh::init(const FLOAT xc, const FLOAT yc, bool brev)
{	
	m_brev = brev;	
	m_vcenter.x = xc;
	m_vcenter.y = yc;
}
bool CConeMesh::intersect(const D3DXVECTOR3* ppos, const D3DXVECTOR3* pdir, D3DXVECTOR3* pv)
{	
	WORD* pindices;	
	D3DVERTEX* pvertices;	
	D3DXVECTOR3 v0, v1, v2;	
	UCHAR i;
		
	m_buffer.ib->Lock(0, 0, (VOID**)&pindices, 0);
	m_buffer.vb->Lock(0, 0, (VOID**)&pvertices, 0);	
	m_buffer.ib->Unlock();
	m_buffer.vb->Unlock();

	if (m_brev)
	{
		v0.x = pvertices[0].x;
		v0.y = pvertices[0].y;
		v0.z = pvertices[0].z;
	}
	else 
	{
		v0.x = pvertices[m_sectors + 1].x;
		v0.y = pvertices[m_sectors + 1].y;
		v0.z = pvertices[m_sectors + 1].z;
	}
		
	for (i = 1; i < m_sectors; i++)
	{
		v1.x = pvertices[i].x;
		v1.y = pvertices[i].y;
		v1.z = pvertices[i].z;
		
		v2.x = pvertices[i + 1].x;
		v2.y = pvertices[i + 1].y;
		v2.z = pvertices[i + 1].z;

		if (hitTri(&v0, &v1, &v2, ppos, pdir, pv)) return 1;	
	}
	// last triangle
	v1.x = pvertices[i].x;
	v1.y = pvertices[i].y;
	v1.z = pvertices[i].z;
		
	v2.x = pvertices[1].x;
	v2.y = pvertices[1].y;
	v2.z = pvertices[1].z;	
	
	if (hitTri(&v0, &v1, &v2, ppos, pdir, pv)) return 1;		
	
	return 0;
}
HRESULT CConeMesh::update()
{
	D3DVERTEX* pvertices;		
	WORD* pindices;	
	
	HRESULT hres = S_OK;	
	
	if (uw)
	{
		_free();

		m_buffer.ilength = (m_sectors + 2) << 1;
		m_buffer.vlength = (m_sectors + 2);		

		if (FAILED(hres = lpd3d->CreateIndexBuffer(m_buffer.ilength << 1, 0, 
			D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_buffer.ib, 0))) return hres;
		if (FAILED(hres = lpd3d->CreateVertexBuffer(m_buffer.vlength << 4, 0, 
			VERTEX_FVF, D3DPOOL_MANAGED, &m_buffer.vb, 0))) return hres;
	}
	m_buffer.ib->Lock(0, m_buffer.ilength << 1, (VOID**)&pindices, 0); 
	m_buffer.vb->Lock(0, m_buffer.vlength << 4, (VOID**)&pvertices, 0); 		
	// circles
	pindices[0] = 0;		
	pvertices[0] = m_vcenter - vec3; 
	pvertices[0].z *= app.stg._g_set.zcoef;
	pvertices[0].dw = m_dw;	

	pindices[m_sectors + 2] = m_sectors + 1;	
	pvertices[m_sectors + 1] = m_vcenter - vec3;
	pvertices[m_sectors + 1].z *= app.stg._g_set.zcoef;
	pvertices[m_sectors + 1].z += m_h;
	pvertices[m_sectors + 1].dw = m_dw;

	float alpha = 0.0f;	
	D3DXVECTOR3 v;			
	UCHAR i = 0;

	for (i; i < m_sectors; i++)
	{
		v = D3DXVECTOR3(cosf(alpha) * m_radius, sinf(alpha) * m_radius, 0.0f);
		
		pindices[i + 1] = i + 1;				
		pvertices[i + 1] = v + m_vcenter - vec3;	
		pvertices[i + 1].z *= app.stg._g_set.zcoef;
		if (m_brev) pvertices[i + 1].z += m_h;
		pvertices[i + 1].dw = m_dw;	
		
		pindices[m_sectors + 2 + (i + 1)] = m_sectors - i;			
		alpha += 2 * D3DX_PI / m_sectors;
	}
	pindices[i + 1] = 1;	
	pindices[m_sectors + 2 + (i + 1)] = m_sectors;
	
	m_buffer.ib->Unlock();
	m_buffer.vb->Unlock();
	
	return hres;
}
// CDXWell
CDXWell::CDXWell(const char* wellname)
{
	brev = 0;	
	// injection well or not 
	UCHAR len = (UCHAR)strlen(wellname);
	if (wellname[len - 1] == 0x2a)
	{
		brev = 1;
		len--;
	}	
	memset(name, 0, 9);
	strncpy_s(name, 9, wellname, len);
}
CDXWell::~CDXWell()
{	
	m_cyls.RemoveAll();
	m_hparams.clear();	
}
bool CDXWell::pick(const D3DXVECTOR3* ppos, const D3DXVECTOR3* pdir, D3DXVECTOR3* pv)
{	
	D3DXVECTOR3 v;
	UCHAR i;
	std::vector<D3DXVECTOR3> vectors;
	
	if (m_cone.intersect(ppos, pdir, &v)) vectors.push_back(v); 	
	
	if ((surf & VS_TOP) && m_cyls[0]->isectTop(ppos, pdir, &v)) vectors.push_back(v);				
	if ((surf & VS_BOTTOM) && m_cyls[m_cyls.GetSize() - 1]->isectBottom(ppos, pdir, &v)) 
		vectors.push_back(v);
	
	if (surf > VS_BOTTOM)
	{
		for (i = 0; i < m_cyls.GetSize(); i++)
		{
			if (m_cyls[i]->isectSide(ppos, pdir, &v)) 
			{
				vectors.push_back(v);		
				break;
			}
		}
	}
	
	UCHAR index, nsize = (UCHAR)vectors.size();	
	if (nsize == 0) return 0;
	// check for min intersection
	D3DXVECTOR3 vlen;
	float f0 = FLT_MAX, f1;
	for (i = 0; i < nsize; i++)
	{		
		vlen = *ppos - vectors[i];
		f1 = D3DXVec3Length(&vlen);
		if (f1 < f0) 
		{
			f0 = f1; 
			index = i;		
		}
	}		
	pv->x = vectors[index].x;
	pv->y = vectors[index].y;
	pv->z = vectors[index].z;
	vectors.clear();

	return 1;
}
HRESULT CDXWell::render()
{
	HRESULT hres = S_OK;	
	UCHAR nstartIndex = 0;

	lpd3d->SetIndices(m_cone.m_buffer.ib);
	lpd3d->SetStreamSource(0, m_cone.m_buffer.vb, 0, 16);
	// cone
	if FAILED(hres = lpd3d->DrawIndexedPrimitive(D3DPT_TRIANGLEFAN, 0, 0,
			m_cone.m_buffer.vlength, nstartIndex, m_cone.m_sectors)) return hres;			
	nstartIndex += m_cone.m_sectors + 2;
	if FAILED(hres = lpd3d->DrawIndexedPrimitive(D3DPT_TRIANGLEFAN, 0, 0, 
			m_cone.m_buffer.vlength, nstartIndex, m_cone.m_sectors)) return hres;		
	// cyls
	CCylMesh* pCyl;
	for (UCHAR i = 0; i < m_cyls.GetSize(); i++)
	{
		pCyl = m_cyls[i];
		lpd3d->SetIndices(pCyl->m_buffer.ib);
		lpd3d->SetStreamSource(0, pCyl->m_buffer.vb, 0, 16);
		// top circle
		nstartIndex = 0;
		if FAILED(hres = lpd3d->DrawIndexedPrimitive(D3DPT_TRIANGLEFAN, 0, 0,
			pCyl->m_buffer.vlength, nstartIndex, pCyl->m_sectors)) return hres;		
		// bottom circle
		nstartIndex += pCyl->m_sectors + 2;
		if FAILED(hres = lpd3d->DrawIndexedPrimitive(D3DPT_TRIANGLEFAN, 0, 0, 
			pCyl->m_buffer.vlength, nstartIndex, pCyl->m_sectors)) return hres;		
		// side
		nstartIndex += pCyl->m_sectors + 2;
		if FAILED(hres = lpd3d->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0,
			pCyl->m_buffer.vlength, nstartIndex, pCyl->m_sectors << 1)) return hres;				
	}
	lpfont->DrawText(0, name, -1, &rc, DT_CENTER, app.stg._w_set.dw_font);
	return hres;
}
HRESULT CDXWell::update(const FLOAT top)
{
	HRESULT hres = S_OK;	
	ftop = top;		
	// cone
	m_cone.m_h = app.stg._w_set.wh;
	m_cone.m_radius = app.stg._w_set.wr1;	
	m_cone.m_dw = dw_color;	
	m_cone.m_vcenter.z = ftop;	
	if FAILED(hres = m_cone.update()) return hres; 
	// up offset
	CCylMesh* pCyl = m_cyls[0];
	pCyl->m_radius = app.stg._w_set.wr2;	
	pCyl->m_dw = dw_color;	
	pCyl->m_vtop.z = ftop;
	if FAILED(hres = pCyl->update()) return hres;	
	for (int i = 1; i < m_cyls.GetSize(); i++)
	{
		pCyl = m_cyls[i];		
		pCyl->m_radius = app.stg._w_set.wr2;
		pCyl->m_dw = dw_color;		
		if FAILED(hres = pCyl->update()) break;		
	}
	return hres;
}
void CDXWell::init()
{		
	horzparam hparam = m_hparams[0];	
	FLOAT fvalue = hparam.depth;
	// cone		
	m_cone.init((FLOAT)hparam.xc, (FLOAT)hparam.yc, brev);
	// up offset
	CCylMesh* pCyl = new CCylMesh();		
	pCyl->init(&D3DXVECTOR3((FLOAT)hparam.xc, (FLOAT)hparam.yc, fvalue),
			&D3DXVECTOR3((FLOAT)hparam.xc, (FLOAT)hparam.yc, hparam.depth));
	m_cyls.Add(pCyl);
	for (UCHAR i = 1; i < m_hparams.size(); i++)
	{
		hparam = m_hparams[i];
		
		pCyl = new CCylMesh();		
		pCyl->init(&D3DXVECTOR3((FLOAT)hparam.xc, (FLOAT)hparam.yc, fvalue),
			&D3DXVECTOR3((FLOAT)hparam.xc, (FLOAT)hparam.yc, hparam.depth));
		m_cyls.Add(pCyl);
		fvalue = hparam.depth;
	}	
}
void CDXWell::screen(D3DXMATRIX* pProj, D3DXMATRIX* pView, D3DXMATRIX* pWorld)
{	
	D3DVIEWPORT9 vport;
	D3DXVECTOR3 v0, v1;
	
	D3DXFONT_DESC desc = app.stg._w_set.ds;			
	
	lpd3d->GetViewport(&vport);	
	
	horzparam hparam = m_hparams[0];
	v0.x = hparam.xc - vec3.x;
	v0.y = hparam.yc - vec3.y;
	v0.z = ftop - vec3.z;			// top from well pike	
	v0.z *= app.stg._g_set.zcoef;
	v0.z += app.stg._w_set.wh;
	
	D3DXVec3Project(&v1, &v0, &vport, pProj, pView, pWorld);  	
	UCHAR move = (UCHAR)((desc.Height * strlen(name)) >> 1);
	
	rc.left = (LONG)v1.x - move;
	rc.right = (LONG)v1.x + move;
	rc.top = (LONG)v1.y - desc.Height;
	rc.bottom = (LONG)v1.y + desc.Height;
	OffsetRect(&rc, app.stg._w_set.wxo, app.stg._w_set.wyo);
}
// CGraph
IMPLEMENT_DYNCREATE(CGraph, CCmdTarget)
BEGIN_DISPATCH_MAP(CGraph, CCmdTarget)
	DISP_FUNCTION_ID(CGraph, "BackView", dispidbackView, BackView, VT_EMPTY, VTS_NONE)	
	DISP_FUNCTION_ID(CGraph, "BottomView", dispidbottomView, BottomView, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION_ID(CGraph, "FrontView", dispidfrontView, FrontView, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION_ID(CGraph, "LeftView", dispidleftView, LeftView, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION_ID(CGraph, "ResetView", dispidresetView, ResetView, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION_ID(CGraph, "RightView", dispidrightView, RightView, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION_ID(CGraph, "TopView", dispidtopView, TopView, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION_ID(CGraph, "ResetFilter", dispidresetFilter, ResetFilter, VT_ERROR, VTS_NONE)
	DISP_FUNCTION_ID(CGraph, "SetDims", dispidsetDims, SetDims, VT_ERROR, VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_I4)
	DISP_FUNCTION_ID(CGraph, "ShowCCW", dispidshowCCW, ShowCCW, VT_EMPTY, VTS_BOOL VTS_BOOL VTS_BOOL)
END_DISPATCH_MAP()
// {3ECB516B-24CD-494B-8676-1EB3645EDE16}
static const IID			IID_IGraph = {0x3ECB516B, 0x24CD, 0x494B, 
										 {0x86, 0x76, 0x1E, 0xB3, 0x64, 0x5E, 0xDE, 0x16}};

BEGIN_INTERFACE_MAP(CGraph, CCmdTarget)
	INTERFACE_PART(CGraph, IID_IGraph, Dispatch)
END_INTERFACE_MAP()
// {66FCF56C-26B5-4689-9CC0-A7AA9C78B99B}
IMPLEMENT_OLECREATE_FLAGS(CGraph, "Model.Graph", afxRegApartmentThreading, 0x66fcf56c, 0x26b5, 0x4689, 0x9c, 0xc0, 0xa7, 0xaa, 0x9c, 0x78, 0xb9, 0x9b)

CGraph::CGraph()
{
	m_plegend = 0;		
	memset(&m_frustrum, 0, 16);	
	D3DXMatrixIdentity(&m_mProj);			
	vec3 = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	
	
	EnableAutomation();
}
CGraph::~CGraph()
{
	clear();	
}
bool CGraph::initDeviceObject()
{		
	szParam.cx = 0;
	szParam.cy = -1;
	size = space.square();
	
	iniline* piniline;
	for (UCHAR i = 0; i < m_inilines.GetSize(); i++)
	{
		piniline = m_inilines[i];				
		if (!piniline->initlegend()) return 0;		
	}	
	if (!readStruct()) return 0;	
	readWell();
//	if (!readCont()) return 0;		

	createView();	
	::resetFilter();
	ResetView();	

	m_selBuffer.vlength = TEX_VERTEX_NUM;	
	if FAILED(lpd3d->CreateVertexBuffer(m_selBuffer.vlength * 20, 0, 
		VERTEX_FVFT, D3DPOOL_MANAGED, &m_selBuffer.vb, 0)) return 0;
	if FAILED(updateDeviceObject()) return 0;				

	m_binit = 1;
	return 1;	
}
bool CGraph::readStruct()
{
	bool bres = 1;				
	
	app.m_pFrame->m_infobar.m_progress.SetWindowText(_T("Загрузка структуры"));	
	app.m_pFrame->m_infobar.m_progress.SetRange(0, space.nZ << 1);
	app.m_pFrame->m_infobar.m_progress.SetPos(0);
	app.m_pFrame->m_infobar.m_progress.ShowWindow(SW_SHOW);

	try
	{	
		FILE *stream;
		fopen_s(&stream, m_structfile.fext, "rb");	
		if (!stream) throw 0;
		
		float fzmin, fzmax;		
		if (fread(&fzmin, 4, 1, stream) != 1) throw -1; 		
		if (fread(&fzmax, 4, 1, stream) != 1) throw -1; 				
				
		m_plegend = new legend(fzmin, fzmax);	
		m_plegend->setTemplate(&app.stg._c_set);

		vec3.x = space.xLen() / 2.0f;
		vec3.y = space.yLen() / 2.0f;
		vec3.z = fzmin + (fzmax - fzmin) / 2.0f;		

		int dsize = space.square(), iz;
		for (iz = 0; iz < space.nZ; iz++)
		{
			CDXSlice* pdxSlice = new CDXSlice(iz); 				
			// roof
			if (fread(pdxSlice->m_depths[0], 4, dsize, stream) != dsize) throw - 1;
			if (fread(pdxSlice->m_colors[0], 4, dsize, stream) != dsize) throw - 1;				
			app.m_pFrame->m_infobar.m_progress.StepIt();
			// sole
			if (fread(pdxSlice->m_depths[1], 4, dsize, stream) != dsize) throw - 1;
			if (fread(pdxSlice->m_colors[1], 4, dsize, stream) != dsize) throw - 1;				
			app.m_pFrame->m_infobar.m_progress.StepIt();
			m_dxSlices.Add(pdxSlice);			
		}		
		fclose(stream);				
	}	
	catch (int)
	{	
		memset(msg, 0, MAX_PATH);
		sprintf_s(msg, MAX_PATH, "Не удалось открыть %s.", m_structfile.fext);
		bres = 0;
	}
	app.m_pFrame->m_infobar.m_progress.ShowWindow(SW_HIDE);					
	return bres;	
}
bool CGraph::readWell()
{
	bool bvalid;
	char name[9];
	uw = 1;
	horzparam hparam;
	std::vector<horzparam> hparams;
	CDXWell* pdxWell;	
	
	FILE* stream; 
	fopen_s(&stream, m_wellfile, "r");	
	if (!stream) return 0;
	while (fscanf_s(stream, "%s\n", name, 9) == 1)
	{	
		bvalid = 0;		
		while (fscanf_s(stream, "%u %u %f\n", &hparam.xc, &hparam.yc, &hparam.depth) == 3)
		{			
			hparam.xc -= space.xMin;
			hparam.yc -= space.yMin;
			if (space.ptBelong1(hparam.xc, hparam.yc))
			{
				hparams.push_back(hparam);
				bvalid = 1;
			}
		}			
		fscanf_s(stream, "/\n");
		if (bvalid)
		{
			pdxWell = new CDXWell(name);				
			pdxWell->m_hparams.swap(hparams);
			pdxWell->init();
			pdxWell->dw_color = app.stg._w_set.dw_main;
			if FAILED(pdxWell->update(m_plegend->m_fmax + app.stg._w_set.wi)) return 0;
			
			m_dxWells.Add(pdxWell);	
			m_nwells++;
		}	
		hparams.clear();
	}	
	fclose(stream);		
	return 1;
}
D3DXVECTOR3 CGraph::mouseOnSphere(const POINT pt)
{
	D3DXVECTOR3 v;
	v.x = (pt.x - m_frustrum.w / 2.0f)  / m_radius;
	v.y = (m_frustrum.h / 2.0f - pt.y) / m_radius;	
	v.z = 0.0f;	
	float sqlen = D3DXVec3LengthSq(&v);
	
	if (sqlen > 1.0f) D3DXVec3Normalize(&v, &v);	
	else v.z = sqrt(1.0f - sqlen);				
	
	return v;
}
HRESULT CGraph::updateDeviceObject(BYTE upd)
{
	BeginWaitCursor();	
	app.m_pFrame->infoUpdate();	
	if (m_curwell == -1)
	{
		m_dim.Empty();
		m_param.Empty();			
	}
	ug = upd;
	
	bool bprevBottomDrawn = 1;		
	UINT rowx = _xyzDim.xLen(), rowy = _xyzDim.yLen(),
		 rowsperBuffer = dcaps.MaxPrimitiveCount / (rowx << 1);
	int i, nbuffSize = ((rowx * rowy) << 1) / dcaps.MaxPrimitiveCount + 1;	

	CDXSlice* pdxSlice;	
	// visible	
	if (ug & UG_CREATE) 
	{
		m_rows.clear();
		for (i = 0; i < nbuffSize - 1; i++)
		{
			m_rows.push_back(rowsperBuffer);
		}
		m_rows.push_back(rowy - i * rowsperBuffer);		
	
		for (i = 0; i < _xyzDim.zDim.cx; i++)
		{			
			m_dxSlices[i]->setBuff(0);			
		}
		for (i; i < _xyzDim.zDim.cy - 1; i++)
		{			
			pdxSlice = m_dxSlices[i];			
			pdxSlice->setBuff(nbuffSize);			
			pdxSlice->m_bdraw.bdrawTopNeeded = bprevBottomDrawn;
			if (arrayVal(&m_layers, i + 1)) 
			{
				pdxSlice->m_bdraw.bdrawBottomNeeded = 1;
				bprevBottomDrawn = 1;
			}
			else 
			{
				pdxSlice->m_bdraw.bdrawBottomNeeded = 0;
				bprevBottomDrawn = 0;										
			}	
		}		
		// last slice		
		pdxSlice = m_dxSlices[i];			
		pdxSlice->setBuff(nbuffSize);
		pdxSlice->m_bdraw.bdrawTopNeeded = bprevBottomDrawn;
		pdxSlice->m_bdraw.bdrawBottomNeeded = 1;

		for (i = _xyzDim.zDim.cy; i < space.nZ; i++)
		{
			pdxSlice = m_dxSlices[i];
			pdxSlice->setBuff(0);			
		}
	}		
	HRESULT sres = S_OK;
	for (i = _xyzDim.zDim.cx; i < _xyzDim.zDim.cy; i++)
	{		
		pdxSlice = m_dxSlices[i];		
		if FAILED(sres = pdxSlice->updateBuffer()) break;						
	}	
	EndWaitCursor();

	return sres;
}
HRESULT CGraph::updateDeviceWell(bool buw)
{
	HRESULT hres = S_OK;	
	uw = buw;
	CDXWell* pdxWell;
	for (int i = 0; i < m_nwells; i++)
	{
		pdxWell	= m_dxWells[i];	
		pdxWell->dw_color = app.stg._w_set.dw_main;
		if FAILED(hres = pdxWell->update(m_plegend->m_fmax + app.stg._w_set.wi)) break;
	}
	return hres;
}
void CGraph::BackView()
{
	m_down = m_now = D3DXQUATERNION(0.0f, -0.707107f, -0.707107f, 0.0f);
	
	m_mWorld._11 = -1.0f;
	m_mWorld._23 = m_mWorld._32 = m_mWorld._44 = 1.0f;
	m_mWorld._12 = m_mWorld._13 = m_mWorld._14 = 0.0f;
	m_mWorld._21 = m_mWorld._22 = m_mWorld._24 = 0.0f;
	m_mWorld._31 = m_mWorld._33 = m_mWorld._34 = 0.0f;
	m_mWorld._43 = 0.0f;
		
	surf = VS_TOP | VS_BOTTOM | VS_BACK;
	updateFonts();	
}
void CGraph::BottomView()
{
	m_down = m_now = D3DXQUATERNION(-1.0f, 0.0f, 0.0f, 0.0f);

	m_mWorld._11 = m_mWorld._44 = 1.0f;
	m_mWorld._22 = m_mWorld._33 = -1.0f;
	m_mWorld._12 = m_mWorld._13 = m_mWorld._14 = 0.0f;
	m_mWorld._21 = m_mWorld._23 = m_mWorld._24 = 0.0f;
	m_mWorld._31 = m_mWorld._32 = m_mWorld._34 = 0.0f;
	m_mWorld._43 = 0.0f;	
		
	surf = VS_BOTTOM;
	updateFonts();	
}
void CGraph::clear()
{
	m_binit = 0;
	m_curwell = -1;	
	m_nwells = 0;
	memset(_actn, 0, MAX_PATH);
	
	m_dim.Empty();
	m_param.Empty();
	m_title.Empty();
	
	memset(&m_show, 0, 3);
	memset(&m_3dscale, 0, 12);	
	memset(&space, 0, 36);	

	m_layers.clear();
	m_rows.clear();
	m_inilines.RemoveAll();
	
	m_dxSlices.RemoveAll();
	m_dxWells.RemoveAll();
	
	SAFE_RELEASE(m_selBuffer.vb);
	SAFE_DELETE(m_plegend);
}
void CGraph::createView()
{
	WORD div0 = space.xLen(),
		 div1 = space.yLen(),	
		 div2 = (WORD)((m_plegend->m_fmax - m_plegend->m_fmin + app.stg._w_set.wi) * app.stg._g_set.zcoef) + 
						app.stg._w_set.wh;	
		
	D3DXVECTOR3 v(0.0f, 0.0f, 0.0f);
	v.z = max(div0, div1);
	v.z = max(v.z, div2);	
	// create view matrix		
	D3DXMatrixLookAtRH(&m_mView, &v, &D3DXVECTOR3(0.0f, 0.0f, 0.0f), &D3DXVECTOR3(0.0f, 1.0f, 0.0f));				
}
void CGraph::FrontView()
{	
	m_down = m_now = D3DXQUATERNION(-0.707107f, 0.0f, 0.0f, 0.707107f);
	
	m_mWorld._11 = m_mWorld._32 = m_mWorld._44 = 1.0f;
	m_mWorld._23 = -1.0f;	
	m_mWorld._12 = m_mWorld._13 = m_mWorld._14 = 0.0f;
	m_mWorld._21 = m_mWorld._22 = m_mWorld._24 = 0.0f;
	m_mWorld._31 = m_mWorld._33 = m_mWorld._34 = 0.0f;
	m_mWorld._43 = 0.0f;
		
	surf = VS_TOP | VS_BOTTOM | VS_FRONT;
	updateFonts();	
}
void CGraph::LeftView()
{
	m_down = m_now = D3DXQUATERNION(-0.5f, 0.5f, 0.5f, 0.5f);
	
	m_mWorld._13 = m_mWorld._21 = -1.0f;
	m_mWorld._32 = m_mWorld._44 = 1.0f;
	m_mWorld._11 = m_mWorld._12 = m_mWorld._14 = 0.0f;
	m_mWorld._22 = m_mWorld._23 = m_mWorld._24 = 0.0f;
	m_mWorld._31 = m_mWorld._33 = m_mWorld._34 = 0.0f;
	m_mWorld._43 = 0.0f;
		
	surf = VS_TOP | VS_BOTTOM | VS_LEFT;
	updateFonts();	
}
void CGraph::mouseClick(const POINT pt)
{
	m_from = mouseOnSphere(pt);
}
void CGraph::mouseDrag(const POINT pt)
{
	D3DXVECTOR3 vangle, vprod, vto = mouseOnSphere(pt);
	D3DXVec3Cross(&vprod, &m_from, &vto); 		

	D3DXQUATERNION q;
	q.x = vprod.x;
	q.y = vprod.y;
	q.z = vprod.z;
	q.w = D3DXVec3Dot(&m_from, &vto);	
	D3DXQuaternionMultiply(&m_now, &m_down, &q);    // multiply the quaternions.														  	
	// rotation	
	m_mWorld._11 = 1.0f - 2 * (m_now.y * m_now.y + m_now.z * m_now.z);
	m_mWorld._12 = 2 * (m_now.x * m_now.y + m_now.w * m_now.z);
	m_mWorld._13 = 2 * (m_now.x * m_now.z - m_now.w * m_now.y);
	m_mWorld._21 = 2 * (m_now.x * m_now.y - m_now.w * m_now.z);
	m_mWorld._22 = 1.0f - 2 * (m_now.x * m_now.x + m_now.z * m_now.z);
	m_mWorld._23 = 2 * (m_now.y* m_now.z + m_now.w* m_now.x);
	m_mWorld._31 = 2 * (m_now.x* m_now.z + m_now.w* m_now.y);
	m_mWorld._32 = 2 * (m_now.y* m_now.z - m_now.w* m_now.x);
	m_mWorld._33 = 1.0f - 2 * (m_now.x * m_now.x + m_now.y * m_now.y);	
	
	vangle = eulerFromMatrix(&m_mWorld);
	surf = VS_TOP | VS_BOTTOM;			

	if ((vangle.x > 0) && (vangle.x < D3DX_PI)) surf |= VS_BACK;
	else if ((vangle.x > -D3DX_PI) && (vangle.x < 0)) surf |= VS_FRONT;	

	if ((vangle.y > 0) && (vangle.y < D3DX_PI)) surf |= VS_LEFT;
	else if ((vangle.y > -D3DX_PI) && (vangle.y < 0)) surf |= VS_RIGHT;	

	updateFonts();	
}
void CGraph::mousePick(const POINT pt)
{
	if (!m_binit) return;	

	m_dim.Empty();
	m_param.Empty();	

	D3DXVECTOR3 v, vdir, vpos;
	v.x = (2.0f * pt.x / m_frustrum.w - 1) / m_mProj._11;
    v.y = -(2.0f * pt.y / m_frustrum.h - 1) / m_mProj._22;
    v.z = 1.0f;
	
	D3DXMATRIX m;    
	D3DXMatrixInverse(&m, 0, &m_mView);
    // Transform the screen space pick ray into 3D space
	vdir.x = v.x * m._11 + v.y * m._21 + v.z * m._31;
    vdir.y = v.x * m._12 + v.y * m._22 + v.z * m._32;
	vdir.z = v.x * m._13 + v.y * m._23 + v.z * m._33;	
	vpos = D3DXVECTOR3(vdir.x, vdir.y, m._43);    
	
	D3DXMatrixInverse(&m, 0, &m_mWorld);	
	D3DXVec3TransformCoord(&vdir, &vdir, &m);
	D3DXVec3TransformCoord(&vpos, &vpos, &m);	

	int i, nsize;
	UINT index;
	intersection isection;
	std::vector<intersection> isections;

	CDXSlice* pdxSlice;	
	CDXWell* pdxWell;
	if (m_curwell != -1)
	{
		pdxWell	= m_dxWells[m_curwell];	
		pdxWell->dw_color = app.stg._w_set.dw_main;
		pdxWell->update(m_plegend->m_fmax + app.stg._w_set.wi);
	}

	if (m_show.bshowWell)
	{
		for (i = 0; i < m_nwells; i++)
		{
			pdxWell = m_dxWells[i];
			if (pdxWell->pick(&vpos, &vdir, &isection.v)) 
			{
				isection.bwell = 1;
				isection.nwell = i;
				isections.push_back(isection);
			}
		}	
	}	
	D3DLOCKED_RECT lrect;		
	lpsurf->LockRect(&lrect, 0, D3DLOCK_READONLY);	
	WORD* pbits = (WORD*)lrect.pBits;
	WORD wpix = pbits[pt.y * (lrect.Pitch >> 1) + pt.x];	
	lpsurf->UnlockRect();
	
	if (wpix != 0xffff)
	{	
		pdxSlice = m_dxSlices[wpix];
		isection = pdxSlice->pick(&vpos, &vdir);		
		isections.push_back(isection);		
	}
	nsize = (int)isections.size();	
	if (nsize == 0) return;
	// check for min intersection	
	float flength = FLT_MAX, f3dlength;
	for (i = 0; i < nsize; i++)
	{
		v = vpos - isections[i].v;
		f3dlength = D3DXVec3LengthSq(&v);
		if (f3dlength < flength) 
		{
			flength = f3dlength; 
			index = i;		
		}
	}	
	isection = isections[index];
	isections.clear();	

	if (isection.bwell)
	{		
		m_curwell = isection.nwell;
		
		pdxWell = m_dxWells[m_curwell];
		pdxWell->dw_color = app.stg._w_set.dw_sel; 
		pdxWell->update(m_plegend->m_fmax + app.stg._w_set.wi);
		
		m_dim.Format("Скважина %s", pdxWell->name);				
	}
	else
	{	
		m_curwell = -1;	
		isection.v.z /= app.stg._g_set.zcoef;
		isection.v.z += vec3.z;

		D3DTEX vtex[TEX_VERTEX_NUM];		
		float fvalue,
			  fx = (isection.v.x + space.xLen() / 2) / space.xStep,
			  fy = (isection.v.y + space.yLen() / 2) / space.yStep;		
		
		UINT nx, ny,
			 nx0 = (UINT)((isection.v.x + space.xLen() / 2) / space.xStep),
			 ny0 = (UINT)((isection.v.y + space.yLen() / 2) / space.yStep),
			 nx1 = nx0 + 1,
			 ny1 = ny0 + 1;		
		
		switch (isection.surf)
		{
			case VS_TOP: 			
			{	
				vtex[0].x = nx0 * space.xStep - space.xLen() / 2.0f;
				vtex[0].y = ny0 * space.yStep - space.yLen() / 2.0f;
				vtex[0].z = pdxSlice->m_depths[0][ny0 * space.nX + nx0];				

				vtex[1].x = nx0 * space.xStep - space.xLen() / 2.0f;
				vtex[1].y = ny1 * space.yStep - space.yLen() / 2.0f;
				vtex[1].z = pdxSlice->m_depths[0][ny1 * space.nX + nx0];				

				vtex[2].x = nx1 * space.xStep - space.xLen() / 2.0f;
				vtex[2].y = ny0 * space.yStep - space.yLen() / 2.0f;
				vtex[2].z = pdxSlice->m_depths[0][ny0 * space.nX + nx1];
				
				vtex[3].x = nx1 * space.xStep - space.xLen() / 2.0f;
				vtex[3].y = ny1 * space.yStep - space.yLen() / 2.0f;
				vtex[3].z = pdxSlice->m_depths[0][ny1 * space.nX + nx1];				
				// tex 0
				if ((fx - floor(fx) <= 0.5f) && (fy - floor(fy) <= 0.5f))
				{								
					nx = nx0;
					ny = ny0;				
					fvalue = vtex[0].z;
					tex0(vtex);					
				}
				// tex 1
				else if ((fx - floor(fx) <= 0.5f) && (fy - floor(fy) > 0.5f))
				{
					nx = nx0;
					ny = ny1;				
					fvalue = vtex[1].z;
					tex1(vtex);										
				}
				// tex 2
				else if ((fx - floor(fx) > 0.5f) && (fy - floor(fy) <= 0.5f))
				{
					nx = nx1;
					ny = ny0;			
					fvalue = vtex[2].z;
					tex2(vtex);					
				}				
				// tex 3
				else if ((fx - floor(fx) > 0.5f) && (fy - floor(fy) > 0.5f))
				{			
					nx = nx1;
					ny = ny1;	
					fvalue = vtex[3].z;
					tex3(vtex);					
				}
				break;
			}
			case VS_BOTTOM: 
			{	
				vtex[0].x = nx0 * space.xStep - space.xLen() / 2.0f;
				vtex[0].y = ny1 * space.yStep - space.yLen() / 2.0f;
				vtex[0].z = pdxSlice->m_depths[1][ny1 * space.nX + nx0];				

				vtex[1].x = nx0 * space.xStep - space.xLen() / 2.0f;
				vtex[1].y = ny0 * space.yStep - space.yLen() / 2.0f;
				vtex[1].z = pdxSlice->m_depths[1][ny0 * space.nX + nx0];				

				vtex[2].x = nx1 * space.xStep - space.xLen() / 2.0f;
				vtex[2].y = ny1 * space.yStep - space.yLen() / 2.0f;
				vtex[2].z = pdxSlice->m_depths[1][ny1 * space.nX + nx1];
				
				vtex[3].x = nx1 * space.xStep - space.xLen() / 2.0f;
				vtex[3].y = ny0 * space.yStep - space.yLen() / 2.0f;
				vtex[3].z = pdxSlice->m_depths[1][ny0 * space.nX + nx1];				
				// tex 0
				if ((fx - floor(fx) <= 0.5f) && (fy - floor(fy) > 0.5f))
				{					
					nx = nx0;
					ny = ny1;	
					fvalue = vtex[0].z;
					tex0(vtex);
				}
				// tex 1
				else if ((fx - floor(fx) <= 0.5f) && (fy - floor(fy) <= 0.5f))
				{
					nx = nx0;
					ny = ny0;		
					fvalue = vtex[1].z;
					tex1(vtex);					
				}
				// tex 2
				else if ((fx - floor(fx) > 0.5f) && (fy - floor(fy) > 0.5f))
				{
					nx = nx1;
					ny = ny1;				
					fvalue = vtex[2].z;
					tex2(vtex);
				}				
				// tex 3
				else if ((fx - floor(fx) > 0.5f) && (fy - floor(fy) <= 0.5f))
				{			
					nx = nx1;
					ny = ny0;				
					fvalue = vtex[3].z;
					tex3(vtex);
				}
				break;
			}
			case VS_LEFT:
			{
				nx = _xyzDim.xDim.cx;				
				vtex[0].x = nx * space.xStep - space.xLen() / 2.0f;
				vtex[0].y = ny0 * space.yStep - space.yLen() / 2.0f;
				vtex[0].z = pdxSlice->m_depths[0][ny0 * space.nX + nx];				

				vtex[1].x = nx * space.xStep - space.xLen() / 2.0f;
				vtex[1].y = ny0 * space.yStep - space.yLen() / 2.0f;
				vtex[1].z = pdxSlice->m_depths[1][ny0 * space.nX + nx];				

				vtex[2].x = nx * space.xStep - space.xLen() / 2.0f;
				vtex[2].y = ny1 * space.yStep - space.yLen() / 2.0f;
				vtex[2].z = pdxSlice->m_depths[0][ny1 * space.nX + nx];
				
				vtex[3].x = nx * space.xStep - space.xLen() / 2.0f;
				vtex[3].y = ny1 * space.yStep - space.yLen() / 2.0f;
				vtex[3].z = pdxSlice->m_depths[1][ny1 * space.nX + nx];	
				// tex 0
				if ((fy - floor(fy) <= 0.5f) && (fabs(isection.v.z - vtex[0].z) < fabs(isection.v.z - vtex[1].z)))
				{					
					ny = ny0;				
					fvalue = vtex[0].z;
					tex0(vtex);
				}
				// tex 1
				else if ((fy - floor(fy) <= 0.5f) && (fabs(isection.v.z - vtex[0].z) >= fabs(isection.v.z - vtex[1].z)))
				{
					ny = ny0;				
					fvalue = vtex[1].z;
					tex1(vtex);
				}
				// tex 2
				else if ((fy - floor(fy) > 0.5f) && (fabs(isection.v.z - vtex[2].z) < fabs(isection.v.z - vtex[3].z)))
				{
					ny = ny1;				
					fvalue = vtex[2].z;
					tex2(vtex);
				}				
				// tex 3
				else if ((fy - floor(fy) > 0.5f) && (fabs(isection.v.z - vtex[2].z) >= fabs(isection.v.z - vtex[3].z)))
				{			
					ny = ny1;			
					fvalue = vtex[3].z;
					tex3(vtex);
				}	
				break;
			}
			case VS_RIGHT:
			{
				nx = _xyzDim.xDim.cy;			
				vtex[0].x = nx * space.xStep - space.xLen() / 2.0f;
				vtex[0].y = ny0 * space.yStep - space.yLen() / 2.0f;
				vtex[0].z = pdxSlice->m_depths[1][ny0 * space.nX + nx];				

				vtex[1].x = nx * space.xStep - space.xLen() / 2.0f;
				vtex[1].y = ny0 * space.yStep - space.yLen() / 2.0f;
				vtex[1].z = pdxSlice->m_depths[0][ny0 * space.nX + nx];				

				vtex[2].x = nx * space.xStep - space.xLen() / 2.0f;
				vtex[2].y = ny1 * space.yStep - space.yLen() / 2.0f;
				vtex[2].z = pdxSlice->m_depths[1][ny1 * space.nX + nx];
				
				vtex[3].x = nx * space.xStep - space.xLen() / 2.0f;
				vtex[3].y = ny1 * space.yStep - space.yLen() / 2.0f;
				vtex[3].z = pdxSlice->m_depths[0][ny1 * space.nX + nx];	
				// tex 0
				if ((fy - floor(fy) <= 0.5f) && (fabs(isection.v.z - vtex[0].z) < fabs(isection.v.z - vtex[1].z)))
				{								
					ny = ny0;				
					fvalue = vtex[0].z;
					tex0(vtex);
				}
				// tex 1
				else if ((fy - floor(fy) <= 0.5f) && (fabs(isection.v.z - vtex[0].z) >= fabs(isection.v.z - vtex[1].z)))
				{
					ny = ny0;				
					fvalue = vtex[1].z;
					tex1(vtex);
				}
				// tex 2
				else if ((fy - floor(fy) > 0.5f) && (fabs(isection.v.z - vtex[2].z) < fabs(isection.v.z - vtex[3].z)))
				{
					ny = ny1;				
					fvalue = vtex[2].z;
					tex2(vtex);
				}				
				// tex 3
				else if ((fy - floor(fy) > 0.5f) && (fabs(isection.v.z - vtex[2].z) >= fabs(isection.v.z - vtex[3].z)))
				{				
					ny = ny1;			
					fvalue = vtex[3].z;
					tex3(vtex);
				}	
				break;
			}
			case VS_FRONT:
			{
				ny = _xyzDim.yDim.cx;			
				vtex[0].x = nx0 * space.xStep - space.xLen() / 2.0f;
				vtex[0].y = ny * space.yStep - space.yLen() / 2.0f;
				vtex[0].z = pdxSlice->m_depths[1][ny * space.nX + nx0];				

				vtex[1].x = nx0 * space.xStep - space.xLen() / 2.0f;
				vtex[1].y = ny * space.yStep - space.yLen() / 2.0f;
				vtex[1].z = pdxSlice->m_depths[0][ny * space.nX + nx0];				

				vtex[2].x = nx1 * space.xStep - space.xLen() / 2.0f;
				vtex[2].y = ny * space.yStep - space.yLen() / 2.0f;
				vtex[2].z = pdxSlice->m_depths[1][ny * space.nX + nx1];
				
				vtex[3].x = nx1 * space.xStep - space.xLen() / 2.0f;
				vtex[3].y = ny * space.yStep - space.yLen() / 2.0f;
				vtex[3].z = pdxSlice->m_depths[0][ny * space.nX + nx1];	
				// tex 0
				if ((fx - floor(fx) <= 0.5f) && (fabs(isection.v.z - vtex[0].z) < fabs(isection.v.z - vtex[1].z)))
				{								
					nx = nx0; 				
					fvalue = vtex[0].z;
					tex0(vtex);
				}
				// tex 1
				else if ((fx - floor(fx) <= 0.5f) && (fabs(isection.v.z - vtex[0].z) >= fabs(isection.v.z - vtex[1].z)))
				{
					nx = nx0; 				
					fvalue = vtex[1].z;
					tex1(vtex);
				}
				// tex 2
				else if ((fx - floor(fx) > 0.5f) && (fabs(isection.v.z - vtex[2].z) < fabs(isection.v.z - vtex[3].z)))
				{
					nx = nx1; 					
					fvalue = vtex[2].z;
					tex2(vtex);
				}
				// tex 3
				else if ((fx - floor(fx) > 0.5f) && (fabs(isection.v.z - vtex[2].z) >= fabs(isection.v.z - vtex[3].z)))
				{				
					nx = nx1; 					
					fvalue = vtex[3].z;
					tex3(vtex);
				}	
				break;
			}
			case VS_BACK:
			{
				ny = _xyzDim.yDim.cy; 		
				vtex[0].x = nx0 * space.xStep - space.xLen() / 2.0f;
				vtex[0].y = ny * space.yStep - space.yLen() / 2.0f;
				vtex[0].z = pdxSlice->m_depths[0][ny * space.nX + nx0];				

				vtex[1].x = nx0 * space.xStep - space.xLen() / 2.0f;
				vtex[1].y = ny * space.yStep - space.yLen() / 2.0f;
				vtex[1].z = pdxSlice->m_depths[1][ny * space.nX + nx0];				

				vtex[2].x = nx1 * space.xStep - space.xLen() / 2.0f;
				vtex[2].y = ny * space.yStep - space.yLen() / 2.0f;
				vtex[2].z = pdxSlice->m_depths[0][ny * space.nX + nx1];
				
				vtex[3].x = nx1 * space.xStep - space.xLen() / 2.0f;
				vtex[3].y = ny * space.yStep - space.yLen() / 2.0f;
				vtex[3].z = pdxSlice->m_depths[1][ny * space.nX + nx1];	
				// tex 0
				if ((fx - floor(fx) <= 0.5f) && (fabs(isection.v.z - vtex[0].z) < fabs(isection.v.z - vtex[1].z)))
				{								
					nx = nx0;					
					fvalue = vtex[0].z;
					tex0(vtex);
				}
				// tex 1
				else if ((fx - floor(fx) <= 0.5f) && (fabs(isection.v.z - vtex[0].z) >= fabs(isection.v.z - vtex[1].z)))
				{
					nx = nx0;					
					fvalue = vtex[1].z;
					tex1(vtex);
				}
				// tex 2
				else if ((fx - floor(fx) > 0.5f) && (fabs(isection.v.z - vtex[2].z) < fabs(isection.v.z - vtex[3].z)))
				{
					nx = nx1;				
					fvalue = vtex[2].z;
					tex2(vtex);
				}
				// tex 3
				else if ((fx - floor(fx) > 0.5f) && (fabs(isection.v.z - vtex[2].z) >= fabs(isection.v.z - vtex[3].z)))
				{		
					nx = nx1;				
					fvalue = vtex[3].z;
					tex3(vtex);
				}	
				break;
			}			
		}
		updateSel(vtex);
		if (szParam.cx > 0) fvalue = m_inilines[szParam.cx - 1]->getparam(nx, ny, wpix);											
		
		m_dim.Format("i, j, k (%d, %d, %d)", nx, ny, wpix + 1);				
		m_param.Format("%.*f", fabs(fvalue) > 1.0f ? 2 : 3, fvalue);
	}	
}
void CGraph::mouseRelease()
{
	m_down = m_now;
}
void CGraph::move(const CPoint pt)
{
	float fmove = MIN_CELL_PERSCREEN * max(space.xStep, space.yStep) / m_3dscale.curscale;	

	if (pt.x != 0) m_mWorld._41 += (pt.x < 0) ? fmove : -fmove;		
	if (pt.y != 0) m_mWorld._42 += (pt.y > 0) ? fmove : -fmove;						
	updateFonts();
}
void CGraph::ResetView()
{
	m_3dscale.curscale = 1.0f;
	m_mProj._11 = (float)2 / (m_frustrum.w * m_3dscale.begscale);
	m_mProj._22 = (float)2 / (m_frustrum.h * m_3dscale.begscale);
	m_mProj._41 = m_mProj._42 = 0.0f;	
	m_mWorld._41 = m_mWorld._42 = 0.0f;		
	
	TopView();	
}
void CGraph::RightView()
{
	m_down = m_now = D3DXQUATERNION(-0.5f, -0.5f, -0.5f, 0.5f);

	m_mWorld._13 = m_mWorld._21 = m_mWorld._32 = m_mWorld._44 = 1.0f;
	m_mWorld._11 = m_mWorld._12 = m_mWorld._14 = 0.0f;
	m_mWorld._22 = m_mWorld._23 = m_mWorld._24 = 0.0f;
	m_mWorld._31 = m_mWorld._33 = m_mWorld._34 = 0.0f;
	m_mWorld._43 = 0.0f;
		
	surf = VS_TOP | VS_BOTTOM | VS_RIGHT;
	updateFonts();	
}
void CGraph::scale(const int ny, bool bwheel)
{
	float fscale = (ny > 0) ? 0.95f : 1 / 0.95f;								
	if (bwheel) fscale *= fscale;
	
	if ((m_3dscale.curscale * fscale > 1.0f) && 
		(m_3dscale.curscale * fscale < m_3dscale.maxscale))
	{			
		m_mProj._11 *= fscale;
		m_mProj._22 *= fscale;				
		
		m_3dscale.curscale *= fscale;		
	}
	updateFonts();
}
HRESULT CGraph::ResetFilter()
{
	HRESULT hres = S_OK;
	
	::resetFilter();
	if SUCCEEDED(hres = updateDeviceObject(UG_CREATE | UG_READ)) 
		app.m_pFrame->m_infobar.setFilter();
	
	return hres;
}
HRESULT CGraph::SetDims(LONG x0, LONG x1, LONG y0, LONG y1, LONG z0, LONG z1)
{
	HRESULT hres = S_OK;
	// xDim
	x0 = max(0, x0);
	x0 = min(x0, space.nX - 1);		
	_xyzDim.xDim.cx = x0;

	x1 = max(0, x1);
	x1 = min(x1, space.nX - 1);		
	_xyzDim.xDim.cy = x1;
	
	if (x0 == x1)
	{
		if (x0 == 0) _xyzDim.xDim.cy++;
		else _xyzDim.xDim.cx--;		
	}	
	// yDim
	y0 = max(0, y0);
	y0 = min(y0, space.nY - 1);	
	_xyzDim.yDim.cx = y0;

	y1 = max(0, y1);
	y1 = min(y1, space.nY - 1);		
	_xyzDim.yDim.cy = y1;

	if (y0 == y1)
	{
		if (y0 == 0) _xyzDim.yDim.cy++;
		else _xyzDim.yDim.cx--;		
	}
	// zDim
	z0 = max(0, z0);
	z0 = min(z0, space.nZ);	
	_xyzDim.zDim.cx = z0;
	
	z1 = max(0, z1);
	z1 = min(z1, space.nZ);		
	_xyzDim.zDim.cy = z1;

	if (z0 == z1)
	{
		if (z0 == 0) _xyzDim.zDim.cy++;
		else _xyzDim.zDim.cx--;		
	}

	if SUCCEEDED(hres = updateDeviceObject()) app.m_pFrame->m_infobar.setFilter();
	
	return hres;
}
void CGraph::setProj(bool bfirst)
{	
	WORD div0 = space.xLen() / m_frustrum.w,
		 div1 = space.yLen() / m_frustrum.h,
		 div2 = (WORD)((m_plegend->m_fmax + app.stg._w_set.wi - m_plegend->m_fmin) * 
				app.stg._g_set.zcoef / min(m_frustrum.w, m_frustrum.h));	
	
	div0 = max(div0, div1);
	div0 = max(div0, div2);	
	div0 += 2;
	if (div0 > 3) div0 += 3;

	m_frustrum.zn = -2 * m_mView._43;
	m_frustrum.zf = -1 * m_frustrum.zn;
	
	m_3dscale.begscale = div0;			
	m_3dscale.maxscale = (FLOAT)(max(space.nX, space.nY) / MIN_CELL_PERSCREEN + 3);	
	m_radius = sqrtf((FLOAT)(m_frustrum.w * m_frustrum.w + m_frustrum.h * m_frustrum.h)) / 2;	
	// create OrthoRH Projection
	if (bfirst)
	{
		m_mProj._11 = (FLOAT)2 / (m_frustrum.w * m_3dscale.begscale);
		m_mProj._22 = (FLOAT)2 / (m_frustrum.h * m_3dscale.begscale);		
	}
	else 
	{
		m_mProj._11 = (FLOAT)2 / (m_frustrum.w * m_3dscale.viewscale());
		m_mProj._22 = (FLOAT)2 / (m_frustrum.h * m_3dscale.viewscale());
	}
	m_mProj._33 = (FLOAT)1 / (m_frustrum.zf - m_frustrum.zn);
	m_mProj._43 = (FLOAT)m_frustrum.zn / (m_frustrum.zn - m_frustrum.zf);			

	lpd3d->SetTransform(D3DTS_VIEW, &m_mView);	
	lpd3d->CreateRenderTarget(m_frustrum.w, m_frustrum.h, D3DFMT_R5G6B5, 
		D3DMULTISAMPLE_NONE, 0, 1, &lpsurf, 0);	

	updateFonts();
}
void CGraph::ShowCCW(BOOL showCell, BOOL showCont, BOOL showWell)
{
	m_show.bshowCell = showCell;
	m_show.bshowCont = showCont;
	m_show.bshowWell = showWell;
}
void CGraph::TopView()
{	
	m_down = m_now = D3DXQUATERNION(0.0f, 0.0f, 0.0f, 1.0f);
	
	m_mWorld._11 = m_mWorld._22 = m_mWorld._33 = m_mWorld._44 = 1.0f;
	m_mWorld._12 = m_mWorld._13 = m_mWorld._14 = 0.0f;
	m_mWorld._21 = m_mWorld._23 = m_mWorld._24 = 0.0f;
	m_mWorld._31 = m_mWorld._32 = m_mWorld._34 = 0.0f;
	m_mWorld._43 = 0.0f;	
	
	surf = VS_TOP;
	updateFonts();
}
void CGraph::updateFonts()
{	
	for (int i = 0; i < m_nwells; i++)
	{		
		m_dxWells[i]->screen(&m_mProj, &m_mView, &m_mWorld);		
	}	
}
void CGraph::updateSel(D3DTEX* pvt)
{	
	D3DTEX* pvx;	
	
	m_selBuffer.vb->Lock(0, m_selBuffer.vlength * 20, (VOID**)&pvx, 0);	
	
	for (UCHAR i = 0; i < TEX_VERTEX_NUM; i++)
	{
		pvt[i].z = (pvt[i].z - vec3.z) * app.stg._g_set.zcoef;	
	}
	memcpy(pvx, pvt, m_selBuffer.vlength * 20);
	m_selBuffer.vb->Unlock();	
}