// mem.cpp
#include "stdafx.h"
#include <fstream>
#include "mem.h"

void _dll writelog(const char* err, char* log)
{
	char buf[32];
	struct tm now;
	time_t ltime;
	
	time(&ltime);
	localtime_s(&now, &ltime);	

	strftime(buf, 32, "%d.%m.%Y %H:%M:%S", &now);
	
	FILE* stream;
	fopen_s(&stream, log, "a"); 
	fprintf_s(stream, "%s: %s\n", &buf, err);
	fclose(stream);
}
// ini
ini::ini(LPCTSTR lpFileName)
{	
	lstrcpy(fileName, lpFileName);
}
bool ini::getValue(LPCTSTR lpSection, LPCTSTR lpKeyName, bool& bValue)
{
	TCHAR lpString[3];
	DWORD res = GetPrivateProfileString(lpSection, lpKeyName, 0, lpString, 2, fileName);	
	bValue = (lpString[0] == '1');

	return res != 0;
}
bool ini::getValue(LPCTSTR lpSection, LPCTSTR lpKeyName, float& fValue)
{	
	TCHAR lpString[16];	
	DWORD res = GetPrivateProfileString(lpSection, lpKeyName, 0, lpString, 15, fileName);
	fValue = (float)atof(lpString);
	
	return res != 0;
}
bool ini::getValue(LPCTSTR lpSection, LPCTSTR lpKeyName, int& iValue)
{	
	iValue = GetPrivateProfileInt(lpSection, lpKeyName, -1, fileName);	
	return iValue != -1;
}
bool ini::getValue(LPCTSTR lpSection, LPCTSTR lpKeyName, LPPOINT lpPoint)
{		
	TCHAR lpString[32];

	DWORD res = GetPrivateProfileString(lpSection, lpKeyName, 0, lpString, 31, fileName);	 
	sscanf_s(lpString, "point(%d, %d)", &lpPoint->x, &lpPoint->y);
	return res != 0;
}
bool ini::getValue(LPCTSTR lpSection, LPCTSTR lpKeyName, LPRECT lpRect)
{		
	TCHAR lpString[32];

	DWORD res = GetPrivateProfileString(lpSection, lpKeyName, 0, lpString, 31, fileName);	 
	sscanf_s(lpString, "rect(%d, %d, %d, %d)", &lpRect->left, &lpRect->top, &lpRect->right, &lpRect->bottom);
	return res != 0;
}
bool ini::getValue(LPCTSTR lpSection, LPCTSTR lpKeyName, CString& sValue)
{		
	TCHAR lpString[MAX_PATH];
	DWORD res = GetPrivateProfileString(lpSection, lpKeyName, 0, lpString, MAX_PATH - 1, fileName);	 	
	sValue = CString(lpString);
	return res != 0;
}
bool ini::getValue(LPCTSTR lpSection, LPCTSTR lpKeyName, tm& tValue)
{		
	TCHAR lpString[16];
	DWORD res = GetPrivateProfileString(lpSection, lpKeyName, 0, lpString, 31, fileName);	 
	sscanf_s(lpString, "%d.%d.%d", &tValue.tm_mday, &tValue.tm_mon, &tValue.tm_year);
	
	tValue.tm_mon--;
	tValue.tm_year += 100;
	if (tValue.tm_year > _me) tValue.tm_year -= _me;

	return res != 0;
}
bool ini::getWide(LPCTSTR lpSection, LPCTSTR lpKeyName, CStringArray* psaValue)
{			
	bool bres = 0;
	char lpString[SHRT_MAX];
	psaValue->RemoveAll();
	GetPrivateProfileString(lpSection, lpKeyName, 0, lpString, SHRT_MAX - 1, fileName);	 	
	
	CString line = CString(lpString);
	int i = line.Find('/');
	while (i >= 0)
	{					
		psaValue->Add(line.Left(i));
		line.Delete(0, ++i);
		i = line.Find('/');	
		bres = 1;
	}			
	return bres;
}
void ini::setValue(LPCTSTR lpSection, LPCTSTR lpKeyName, bool bValue)
{
	char lpString[2];
	sprintf_s(lpString, 2, "%u", bValue);
	WritePrivateProfileString(lpSection, lpKeyName, lpString, fileName);
}
void ini::setValue(LPCTSTR lpSection, LPCTSTR lpKeyName, float fValue)
{
	char lpString[16];
	sprintf_s(lpString, 32, "%f", fValue);
	WritePrivateProfileString(lpSection, lpKeyName, lpString, fileName);
}
void ini::setValue(LPCTSTR lpSection, LPCTSTR lpKeyName, int iValue)
{
	char lpString[16];
	sprintf_s(lpString, 16, "%d", iValue);
	WritePrivateProfileString(lpSection, lpKeyName, lpString, fileName);
}
void ini::setValue(LPCTSTR lpSection, LPCTSTR lpKeyName, POINT point)
{
	char lpString[16];
	sprintf_s(lpString, 16, "point(%d, %d)", point.x, point.y);
	WritePrivateProfileString(lpSection, lpKeyName, lpString, fileName);
}
void ini::setValue(LPCTSTR lpSection, LPCTSTR lpKeyName, RECT rect)
{
	char lpString[32];
	sprintf_s(lpString, 32, "rect(%d, %d, %d, %d)", rect.left, rect.top, rect.right, rect.bottom);
	WritePrivateProfileString(lpSection, lpKeyName, lpString, fileName);
}
void ini::setValue(LPCTSTR lpSection, LPCTSTR lpKeyName, CString& sValue)
{	
	WritePrivateProfileString(lpSection, lpKeyName, sValue, fileName);
}
void ini::setValue(LPCTSTR lpSection, LPCTSTR lpKeyName, tm tValue)
{
	char lpString[32];
	sprintf_s(lpString, 32, "%d.%d.%d", tValue.tm_mday, tValue.tm_mon, tValue.tm_year);
	WritePrivateProfileString(lpSection, lpKeyName, lpString, fileName);
}
void ini::setWide(LPCTSTR lpSection, LPCTSTR lpKeyName, CStringArray* psaValue)
{
	char lpString[SHRT_MAX];
	memset(lpString, 0, SHRT_MAX);
	for (int i = 0; i < psaValue->GetSize(); i++)
	{
		strcat_s(lpString, SHRT_MAX, psaValue->GetAt(i));
		strcat_s(lpString, SHRT_MAX, "/");
	}
	WritePrivateProfileString(lpSection, lpKeyName, lpString, fileName);
}