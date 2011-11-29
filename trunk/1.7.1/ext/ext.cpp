// ext.cpp
#include "ext.h"

BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH: break;
	}
    return 1;
}
void _dll getDir(char* dir, rsize_t _DstSize, const char* file)
{	
	char i, symb;
	UCHAR len = (UCHAR)strlen(file);	
	for (i = --len; i >= 0; i--)
	{
		symb = file[i];
		if (symb == '\\') break;
	}	
	strncpy_s(dir, _DstSize, file, ++i);	
}
void _dll getMonth(char* buf, rsize_t _DstSize, const struct tm* timeptr)
{	
	switch (timeptr->tm_mon)
	{
		case 0:  
		{
			strcpy_s(buf, _DstSize, "Январь");
			break;
		}
		case 1:  
		{
			strcpy_s(buf, _DstSize, "Февраль");
			break;
		}
		case 2:  
		{
			strcpy_s(buf, _DstSize, "Март");
			break;
		}
		case 3:  
		{
			strcpy_s(buf, _DstSize, "Апрель");
			break;
		}
		case 4:  
		{
			strcpy_s(buf, _DstSize, "Май");
			break;
		}
		case 5:  
		{
			strcpy_s(buf, _DstSize, "Июнь");
			break;
		}
		case 6:  
		{
			strcpy_s(buf, _DstSize, "Июль");
			break;
		}
		case 7:  
		{
			strcpy_s(buf, _DstSize, "Август");
			break;
		}
		case 8:  
		{
			strcpy_s(buf, _DstSize, "Сентябрь");
			break;
		}
		case 9:  
		{
			strcpy_s(buf, _DstSize, "Октябрь");
			break;
		}
		case 10:  
		{
			strcpy_s(buf, _DstSize, "Ноябрь");
			break;
		}
		case 11:  
		{
			strcpy_s(buf, _DstSize, "Декабрь");
			break;
		}
	}
}
void _dll getName(char* name, rsize_t _DstSize, const char* file)
{
	char symb;
	UCHAR i, len = (UCHAR)strlen(file);	
	for (i = --len; i > 0; i--)
	{		
		symb = file[i];		
		if (symb == '\\') break;
	}
	if (i != 0) i++;
	
	strncpy_s(name, _DstSize, file + i, ++len - i);		
}