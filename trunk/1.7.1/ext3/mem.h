// mem.h
#pragma once

#define _me 2000

#ifdef _exp
#define _dll __declspec(dllexport)
#else
#define _dll __declspec(dllimport)
#endif

void _dll writelog(const char* err, char* log);
// ini
class _dll ini
{
public:
	ini(LPCTSTR lpFileName);
	bool getValue(LPCTSTR lpSection, LPCTSTR lpKeyName, bool& bValue);
	bool getValue(LPCTSTR lpSection, LPCTSTR lpKeyName, float& fValue);
	bool getValue(LPCTSTR lpSection, LPCTSTR lpKeyName, int& iValue);
	bool getValue(LPCTSTR lpSection, LPCTSTR lpKeyName, LPPOINT lpPoint);			
	bool getValue(LPCTSTR lpSection, LPCTSTR lpKeyName, LPRECT lpRect);				
	bool getValue(LPCTSTR lpSection, LPCTSTR lpKeyName, tm& tValue);
	bool getValue(LPCTSTR lpSection, LPCTSTR lpKeyName, CString& sValue);			
	bool getWide(LPCTSTR lpSection, LPCTSTR lpKeyName, CStringArray* psaValue);			

	void setValue(LPCTSTR lpSection, LPCTSTR lpKeyName, bool bValue);
	void setValue(LPCTSTR lpSection, LPCTSTR lpKeyName, float fValue);
	void setValue(LPCTSTR lpSection, LPCTSTR lpKeyName, int nValue);	
	void setValue(LPCTSTR lpSection, LPCTSTR lpKeyName, POINT point);			
	void setValue(LPCTSTR lpSection, LPCTSTR lpKeyName, RECT rect);				
	void setValue(LPCTSTR lpSection, LPCTSTR lpKeyName, tm tValue);
	void setValue(LPCTSTR lpSection, LPCTSTR lpKeyName, CString &Value);
	void setWide(LPCTSTR lpSection, LPCTSTR lpKeyName, CStringArray* psaValue);			
// Attributes
public:
	TCHAR	fileName[MAX_PATH];
};