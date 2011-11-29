#include "stdAfx.h"
#include "obj1.h"
// legend
legend::legend(float fmin, float fmax)
{
	m_fmin = fmin;
	m_fmax = fmax;
}
legend::~legend()
{
	m_colors.clear();
}
DWORD legend::getcolor(float fvalue)
{		
	fvalue = max(m_fmin, fvalue);
	fvalue = min(m_fmax, fvalue);	
	
	if (m_colors.size() == 1) return m_colors[0];
		
	USHORT index = (USHORT)((fvalue - m_fmin) / m_fstep);	
	float fkey0 = m_fmin + index * m_fstep,
	      fkey1 = m_fmin + (index + 1) * m_fstep;		
	DWORD dw_color0 = m_colors[index],
		  dw_color1 = m_colors[index + 1];
				  
	BYTE r0 = (BYTE)((dw_color0 >> 0x10) & 0xff),
		 g0 = (BYTE)((dw_color0 >> 0x8) & 0xff),
		 b0 = (BYTE)(dw_color0 & 0xff),
		 r1 = (BYTE)((dw_color1 >> 0x10) & 0xff),
		 g1 = (BYTE)((dw_color1 >> 0x8) & 0xff),
		 b1 = (BYTE)(dw_color1 & 0xff),
		 r = (BYTE)(r0 + (r1 - r0) * fabs((fvalue - fkey0) / (fkey1 - fkey0))),
		 g = (BYTE)(g0 + (g1 - g0) * fabs((fvalue - fkey0) / (fkey1 - fkey0))),
		 b = (BYTE)(b0 + (b1 - b0) * fabs((fvalue - fkey0) / (fkey1 - fkey0)));	
		
	return 0xff000000 | (r & 0xff) << 16 | (g & 0xff) << 8 | b & 0xff;
}
void legend::createzones()
{	// only one color
	bool bone = (pc_set->_size == 1);	
	float frange = fabs(m_fmax - m_fmin);		
	// min = max
	if (frange == 0) 
	{		
		m_colors.push_back(pc_set->colors[0]);
		return;
	}
	m_fstep = frange / 50;
	
	short div0 = (short)(m_fmin / m_fstep - 1),
		  div1 = (short)(m_fmax / m_fstep + 1);		  	
	
	if (fabs(m_fmin) < 1e-10f) div0 = 0;	

	float fvalue,
		  frange0 = m_fstep * div0,
		  frange1 = m_fstep * div1;		
		  
	m_flabRange = frange1 - frange0;		
	
	float fstepc = m_flabRange / (pc_set->_size - 1 + bone); 		 
	
	BYTE r, g, b, gray,
		 r0, g0, b0,
		 r1, g1, b1;	
	
	DWORD dw;	
	int i;
	UINT j, nsize = pc_set->_size;

	if (pc_set->binv)
	{
		for (i = 0; i < abs(div1 - div0) + 1;  i++)
		{		
			fvalue = i * m_fstep - (fabs(m_fstep) > 1 ? 1e-3f : 1e-5f);		 
			
			if (bone) 
			{
				r = (BYTE)((pc_set->colors[0] >> 0x10) & 0xff);
				g = (BYTE)((pc_set->colors[0] >> 0x8) & 0xff);
				b = (BYTE)(pc_set->colors[0] & 0xff);
			}
			else
			{			 
				j = (UINT)(fvalue / fstepc);		
				// convert D3DCOLOR (XRGB)
				r0 = (BYTE)((pc_set->colors[nsize - (j + 1)] >> 0x10) & 0xff),
				g0 = (BYTE)((pc_set->colors[nsize - (j + 1)] >> 0x8) & 0xff),
				b0 = (BYTE)(pc_set->colors[nsize - (j + 1)] & 0xff),
		
				r1 = (BYTE)((pc_set->colors[nsize - (j + 2)] >> 0x10) & 0xff),
				g1 = (BYTE)((pc_set->colors[nsize - (j + 2)] >> 0x8) & 0xff),
				b1 = (BYTE)(pc_set->colors[nsize - (j + 2)] & 0xff),
		
				r = (BYTE)(r0 + (r1 - r0) * fabs((i * m_fstep - j * fstepc) / fstepc));
				g = (BYTE)(g0 + (g1 - g0) * fabs((i * m_fstep - j * fstepc) / fstepc));
				b = (BYTE)(b0 + (b1 - b0) * fabs((i * m_fstep - j * fstepc) / fstepc));
			}

			if (pc_set->bgray)
			{
				gray = (BYTE)ROUND(0.3f * r + 0.59f * g + 0.11f * b);
				dw = 0xff000000 | (gray & 0xff) << 16 | (gray & 0xff) << 8 | gray & 0xff;
					
			}
			else dw = 0xff000000 | (r & 0xff) << 16 | (g & 0xff) << 8 | b & 0xff;
			m_colors.push_back(dw);
		}		
	}
	else
	{	
		for (i = 0; i < abs(div1 - div0) + 1; i++)
		{		
			fvalue = i * m_fstep - (fabs(m_fstep) > 1 ? 1e-3f : 1e-5f);
		 
			if (bone)
			{
				r = (BYTE)((pc_set->colors[0] >> 0x10) & 0xff);
				g = (BYTE)((pc_set->colors[0] >> 0x8) & 0xff);
				b = (BYTE)(pc_set->colors[0] & 0xff);
			}
			else
			{
				j = (UINT)(fvalue / fstepc);		
				// convert D3DCOLOR (XRGB)
				r0 = (BYTE)((pc_set->colors[j] >> 0x10) & 0xff),
				g0 = (BYTE)((pc_set->colors[j] >> 0x8) & 0xff),
				b0 = (BYTE)(pc_set->colors[j] & 0xff),			
			
				r1 = (BYTE)((pc_set->colors[j + 1] >> 0x10) & 0xff),
				g1 = (BYTE)((pc_set->colors[j + 1] >> 0x8) & 0xff),
				b1 = (BYTE)(pc_set->colors[j + 1] & 0xff),
		
				r = (BYTE)(r0 + (r1 - r0) * fabs((i * m_fstep - j * fstepc) / fstepc));
				g = (BYTE)(g0 + (g1 - g0) * fabs((i * m_fstep - j * fstepc) / fstepc));
				b = (BYTE)(b0 + (b1 - b0) * fabs((i * m_fstep - j * fstepc) / fstepc));
			}

			if (pc_set->bgray)
			{
				gray = (BYTE)ROUND(0.3f * r + 0.59f * g + 0.11f * b);
				dw = 0xff000000 | (gray & 0xff) << 16 | (gray & 0xff) << 8 | gray & 0xff;				
			}
			else dw = 0xff000000  | (r & 0xff) << 16 | (g & 0xff) << 8 | b & 0xff;						
			m_colors.push_back(dw);
		}		
	}
}
void legend::draw(CDC* pDC, COLORREF cl)
{
	RECT rcClr, rcFrame;	
	CBrush brClr, brFrame(cl);	
	CPen pen(PS_SOLID | PS_COSMETIC, 1, cl);			

	SetRect(&rcFrame, 0, 0, 290, 22);	
	OffsetRect(&rcFrame, 20, 2);	
	pDC->FrameRect(&rcFrame, &brFrame);	
	
	BYTE r, g, b;	
	int i, nsize = (int)m_colors.size();	
	for (i = 0; i < nsize; i++)
	{
		SetRect(&rcClr, i * (rcFrame.right - rcFrame.left - 2) / nsize, rcFrame.top + 1, 
			(i + 1) * (rcFrame.right - rcFrame.left - 2) / nsize, rcFrame.bottom - 1);
		OffsetRect(&rcClr, 1 + rcFrame.left, 0);		
		// Convert D3DCOLOR (XRGB)
		r = (BYTE)((m_colors[i] >> 0x10) & 0xff),
		g = (BYTE)((m_colors[i] >> 0x8) & 0xff),
		b = (BYTE)(m_colors[i] & 0xff); 			
		
		brClr.CreateSolidBrush(RGB(r, g, b));
		pDC->FillRect(&rcClr, &brClr);
		brClr.DeleteObject();		
	}		

	char* pchar = new char[16];			
	int nlab = min(nsize, 5), nmove = 0;		 
	POINT pt;
	
	CPen* pold = pDC->SelectObject(&pen);
	if (nlab == 1) 
	{
		sprintf_s(pchar, 16, "%.*f", fabs(m_fmin) > 1 ? 2 : 3, m_fmin);	
		pDC->TextOut(rcFrame.left + (rcFrame.right - rcFrame.left) / 2 - nmove, 26, pchar);
		// labsteps
		pt.x = rcFrame.left + (rcFrame.right - rcFrame.left) / 2;
		pt.y = rcFrame.bottom - 3;
		pDC->MoveTo(pt);
		pDC->LineTo(pt.x, pt.y + 6);						
	}
	else
	{		
		for (i = 0; i < nlab; i++)
		{
			sprintf_s(pchar, 16, "%.*f", fabs(m_flabRange) > 1 ? 2 : 3, m_fmin + i * m_flabRange / nlab);			
			nmove = 9;					
			pDC->TextOut(rcFrame.left + i * (rcFrame.right - rcFrame.left) / nlab - nmove, 26, pchar);						
			// labsteps
			pt.x = rcFrame.left + i * (rcFrame.right - rcFrame.left) / nlab;
			pt.y = rcFrame.bottom - 3;
			pDC->MoveTo(pt);
			pDC->LineTo(pt.x, pt.y + 6);			
		}
		// remove if from circle
		sprintf_s(pchar, 16, "%.*f", fabs(m_flabRange) > 1 ? 2 : 3, m_fmax);	
		pDC->TextOut(rcFrame.left + i * (rcFrame.right - rcFrame.left) / nlab - nmove, 26, pchar);						
		// labsteps
		pt.x = rcFrame.right - 1;
		pt.y = rcFrame.bottom - 3;
		pDC->MoveTo(pt);
		pDC->LineTo(pt.x, pt.y + 6);		
	}
	pDC->SelectObject(pold);
	SAFE_DELETE_ARRAY(pchar);	
}
void legend::setTemplate(c_set* pc_set)
{
	ASSERT(pc_set);
	this->pc_set = pc_set;
	createzones();
}