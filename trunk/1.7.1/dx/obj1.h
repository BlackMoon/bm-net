// obj1.h
#pragma once
#pragma comment(lib, "../ext/release/ext")
#include "obj.h"
#include "../ext/ext.h"
#include <vector>

#ifdef _exp
#define _dll __declspec(dllexport)
#else
#define _dll __declspec(dllimport)
#endif
// iniext
struct _dll iniext
{	
	CString fext, file;
	inline iniext()
	{		
		fext = file = "";
	}
	inline iniext(const char* key)
	{
		file = (CString)key;
	}
	inline void chext(LPCTSTR lpszExt)
	{					
		UCHAR len = file.GetLength();
		char* pext = new char[++len];		
		strcpy_s(pext, len, file);

		PathRenameExtension(pext, lpszExt);			
		fext = CString(pext);		
		delete [] pext;		
	}
};
// color_set
struct _dll c_set
{
	BOOL					bgray, binv;	
	UINT					_size;
	DWORD*					colors;	
	inline c_set()
	{
		bgray = binv = 0;
		_size = 14;
		colors = new DWORD[_size];		
		
		colors[0]  = 0xffff99ff;
		colors[1]  = 0xffcc00ff;
		colors[2]  = 0xff6a5acc;
		colors[3]  = 0xff0000ff;
		colors[4]  = 0xff6595ed;
		colors[5]  = 0xffccffff;
		colors[6]  = 0xff00ff00;
		colors[7]  = 0xffadff2f;
		colors[8]  = 0xffffff00;
		colors[9]  = 0xffffd700;
		colors[10] = 0xffff9900;
		colors[11] = 0xffff7f50;
		colors[12] = 0xffff4500;
		colors[13] = 0xffff0000;
	}
	inline void setArr()
	{		
		SAFE_DELETE_ARRAY(colors);
		colors = new DWORD[_size];		
	}
	c_set& operator = (const c_set& c)
	{
		bgray = c.bgray;
		binv = c.binv;
		_size = c._size;
		return *this; 
	}
};
// legend
class _dll legend
{
private:
	void createzones();
public:
	legend(float fmin, float fmax);
	~legend();	
	DWORD getcolor(float fvalue);	
	void draw(CDC* pDC, COLORREF cl);
	void setTemplate(c_set* pc_set); 
// Attributes
private:				
	float					m_flabRange, m_fstep;
	c_set*					pc_set;
	std::vector<DWORD>		m_colors;
public:
	float					m_fmin, m_fmax;
};