// cbc.cpp
#include "stdafx.h"
#include "cbc.h"

char						_log[MAX_PATH], msg[MAX_PATH];		
float						fvalue, incr;
int							periods, n, size;
DWORD						dw;
SPACE						space;
bool*						pbarr = 0;
DWORD*						pdval = 0;
float*						pfval = 0;	
c_set						_c_set;

bool paramConvert(iniext* piniext)
{	
	bool bres = 1;	
	
	FILE *istream, *ostream;	
	fopen_s(&istream, piniext->file, "r");
	fopen_s(&ostream, piniext->fext, "wb");
	legend* plegend = 0;

	try
	{				
		if (!istream) 
		{
			memset(msg, 0, MAX_PATH);
			sprintf_s(msg, MAX_PATH, "Не удалось открыть %s", piniext->file);		
			throw msg;				
		}

		float fzmin, fzmax;			
		if (fscanf_s(istream, "%f", &fzmin) != 1) 
		{
			memset(msg, 0, MAX_PATH);
			sprintf_s(msg, MAX_PATH, "Не удалось прочитать %s, %u", piniext->file, 1);		
			throw msg; 			
		}
		if (fscanf_s(istream, "%f", &fzmax) != 1) 
		{
			memset(msg, 0, MAX_PATH);
			sprintf_s(msg, MAX_PATH, "Не удалось прочитать %s, %u", piniext->file, 2);		
			throw msg; 			
		}
		// global limits		
		fwrite(&fzmin, 4, 1, ostream);
		fwrite(&fzmax, 4, 1, ostream);
		plegend = new legend(fzmin, fzmax);
		plegend->setTemplate(&_c_set);
		
		incr = 100.0f / space.cube();
		n = 0;
		int i = 0,
		    iz, jy, kx;
		for (iz = 0; iz < space.nZ; iz++)
		{		
			for (jy = 0; jy < space.nY; jy++)
			{
				for (kx = 0; kx < space.nX; kx++)
				{					
					if (fscanf_s(istream, "%f", &fvalue) != 1) 
					{						
						memset(msg, 0, 256);
						sprintf_s(msg, MAX_PATH, "Не удалось прочитать %s, %u", piniext->file, 
							(iz * space.square() + jy * space.nX + (kx + 3)));
						throw msg; 
					}					
					dw = plegend->getcolor(fvalue);						
					pdval[i] = dw;						
					pfval[i] = fvalue;						
					i++;				
				}
			}
			fwrite(pfval, 4, size, ostream);
			fwrite(pdval, 4, size, ostream);
			n += i;
			i = 0;
			printf("%.2f%s\r", n * incr, "%");		
		}					
		fclose(istream);	
	}
	catch (char*)
	{		
		writelog(msg, _log);
		bres = 0;
	}
	SAFE_DELETE(plegend);	
	fclose(ostream);	

	printf("\tok\n");
	return bres;
}
bool roxarConvert(iniext* piniext)
{		
	bool bres = 1;
	
	FILE *istream, *ostream; 
	fopen_s(&istream, piniext->file, "r");	
	fopen_s(&ostream, piniext->fext, "wb");	

	legend* plegend;
	try
	{
		if (!istream) 
		{
			memset(msg, 0, MAX_PATH);
			sprintf_s(msg, MAX_PATH, "Не удалось открыть %s", piniext->file);		
			throw msg;				
		}

		float fzmin, fzmax;			
		if (fscanf_s(istream, "%f", &fzmin) != 1) 
		{
			memset(msg, 0, MAX_PATH);
			sprintf_s(msg, MAX_PATH, "Не удалось прочитать %s, %u", piniext->file, 1);		
			throw msg; 			
		}
		if (fscanf_s(istream, "%f", &fzmax) != 1) 
		{
			memset(msg, 0, MAX_PATH);
			sprintf_s(msg, MAX_PATH, "Не удалось прочитать %s, %u", piniext->file, 2);		
			throw msg; 			
		}		
		
		plegend = new legend(fzmin, fzmax);
		plegend->setTemplate(&_c_set);		
		// global limits		
		fwrite(&fzmin, 4, 1, ostream);
		fwrite(&fzmax, 4, 1, ostream);		
		
		incr = 100.0f / (space.cube() * periods);
		n = 0;
		int i = 0, l,
		    iz, jy, kx, mp;
			 
		for (mp = 0; mp < periods; mp++)
		{
			if (fscanf_s(istream, "%f", &fzmin) != 1) 
			{
				memset(msg, 0, MAX_PATH);
				sprintf_s(msg, MAX_PATH, "Не удалось прочитать %s, %u", piniext->file, mp * (space.cube() + 2) + 3);		
				throw msg; 
			}
			if (fscanf_s(istream, "%f", &fzmax) != 1) 
			{
				memset(msg, 0, MAX_PATH);
				sprintf_s(msg, MAX_PATH, "Не удалось прочитать %s, %u", piniext->file, mp * (space.cube() + 2) + 4);		
				throw msg; 
			}
			fwrite(&fzmin, 4, 1, ostream);
			fwrite(&fzmax, 4, 1, ostream);

			for (iz = 0; iz < space.nZ; iz++)
			{
				for (jy = 0; jy < space.nY - 1; jy++)
				{
					for (kx = 0; kx < space.nX - 1; kx++)
					{							
						l = (space.nX - 1) * (iz * (space.nY - 1) + jy) + kx;
						if (fscanf_s(istream, "%f", &fvalue) != 1) 
						{						
							memset(msg, 0, MAX_PATH);
							sprintf_s(msg, MAX_PATH, "Не удалось прочитать %s, %u", piniext->file, 
								mp * (space.cube() + 2) + l + 5);								
							throw msg; 
						}						
						dw = pbarr[l] ? plegend->getcolor(fvalue) : 0xff7d7d7d;					
						pdval[i] = dw;						
						pfval[i] = fvalue;						
						i++;
					}
					// last value
					l = (space.nX - 1) * (iz * (space.nY - 1) + jy) + (kx - 1);
					if (fscanf_s(istream, "%f", &fvalue) != 1) 
					{						
						memset(msg, 0, MAX_PATH);
						sprintf_s(msg, MAX_PATH, "Не удалось прочитать %s, %u", piniext->file, 
							mp * (space.cube() + 2) + l + 5);
						throw msg; 
					}					
					dw = pbarr[l] ? plegend->getcolor(fvalue) : 0xff7d7d7d;					
					pdval[i] = dw;						
					pfval[i] = fvalue;											
					i++;
				}
				// last row
				for (kx = 0; kx < space.nX - 1; kx++)
				{							
					l = (space.nX - 1) * (iz * (space.nY - 1) + jy - 1) + 1;
					if (fscanf_s(istream, "%f", &fvalue) != 1) 
					{						
						memset(msg, 0, MAX_PATH);
						sprintf_s(msg, MAX_PATH, "Не удалось прочитать %s, %u", piniext->file, 
							mp * (space.cube() + 2) + l + 5);							
						throw msg; 
					}					
					dw = pbarr[l] ? plegend->getcolor(fvalue) : 0xff7d7d7d;					
					pdval[i] = dw;						
					pfval[i] = fvalue;												
					i++;				
				}
				l = (space.nX - 1) * (iz * (space.nY - 1) + jy - 1) + (kx - 1);
				if (fscanf_s(istream, "%f", &fvalue) != 1) 
				{						
					memset(msg, 0, MAX_PATH);
					sprintf_s(msg, MAX_PATH, "Не удалось прочитать %s, %u", piniext->file, 
						mp * (space.cube() + 2) + l + 5);							
					throw msg; 
				}				
				dw = pbarr[l] ? plegend->getcolor(fvalue) : 0xff7d7d7d;													
				pdval[i] = dw;						
				pfval[i] = fvalue;												
				i++;
				n += i;
				fwrite(pfval, 4, size, ostream);
				fwrite(pdval, 4, size, ostream);
				i = 0;
				printf("%.2f%s\r", n * incr, "%");					
			}
		}		
		fclose(istream);
	}
	catch (char*)
	{		
		writelog(msg, _log);
		bres = 0;
	}
	SAFE_DELETE(plegend);		
	fclose(ostream);
	
	printf("\tok\n");	
	return bres;
}
bool structConvert(iniext* piniext)
{	
	bool bres = 1;
	
	FILE *istream, *ostream;	
	fopen_s(&istream, piniext->file, "r");
	fopen_s(&ostream, piniext->fext, "wb");
	legend* plegend = 0; 

	try
	{				
		if (!istream) 
		{
			memset(msg, 0, MAX_PATH);
			sprintf_s(msg, MAX_PATH, "Не удалось открыть %s.", piniext->file);		
			throw msg;				
		}

		float fzmin, fzmax;			
		if (fscanf_s(istream, "%f", &fzmin) != 1) 
		{
			memset(msg, 0, MAX_PATH);
			sprintf_s(msg, MAX_PATH, "Не удалось прочитать %s, %u.", piniext->file, 1);		
			throw msg; 			
		}
		if (fscanf_s(istream, "%f", &fzmax) != 1) 
		{
			memset(msg, 0, MAX_PATH);
			sprintf_s(msg, MAX_PATH, "Не удалось прочитать %s, %u.", piniext->file, 2);		
			throw msg; 			
		}			
		// global limits		
		fwrite(&fzmin, 4, 1, ostream);
		fwrite(&fzmax, 4, 1, ostream);
		plegend = new legend(fzmin, fzmax);
		plegend->setTemplate(&_c_set);

		incr = 50.0f / space.cube();		
		
		n = 0;
		int i = 0, 
		    iz, jy, kx; 		
		
		for (iz = 0; iz < space.nZ << 1; iz++)
		{				
			for (jy = 0; jy < space.nY; jy++)
			{
				for (kx = 0; kx < space.nX; kx++)
				{					
					if (fscanf_s(istream, "%f", &fvalue) != 1) 
					{						
						memset(msg, 0, MAX_PATH);
						sprintf_s(msg, MAX_PATH, "Не удалось прочитать %s, %u.", piniext->file, 
							(iz * space.square() + jy * space.nX + (kx + 3)));
						throw msg; 
					}					
					dw = plegend->getcolor(fvalue);						
					pdval[i] = dw;
					pfval[i] = fvalue;
					i++;					
				}
			}
			fwrite(pfval, 4, size, ostream);
			fwrite(pdval, 4, size, ostream);
			n += i;
			i = 0;
			printf("%.2f%s\r", n * incr, "%");
		}		
		fclose(istream);
	}	
	catch (char*)
	{			
		writelog(msg, _log);
		bres = 0;
	}	
	SAFE_DELETE(plegend);	
	fclose(ostream);	
		
	printf("\tok\n");	
	return bres;
}
bool binConvert(const char* filename)
{
	bool bres = 1;
	char actn[MAX_PATH], dir[MAX_PATH];		
	
	ini _ini(filename);
	iniext iext;
	CStringArray sa;	

	memset(dir, 0, MAX_PATH);
	getDir(dir, MAX_PATH, filename); 
	try
	{		
		int xmin, ymin, nx, ny, xstep, ystep;		
		if (!_ini.getValue("dim", "xmin", xmin)) 
		{
			memset(msg, 0, MAX_PATH);
			sprintf_s(msg, MAX_PATH, "Не удалось прочитать значение %s, [dim], xmin.", filename);		
			throw msg;
		}
		if (!_ini.getValue("dim", "ymin", ymin)) 
		{
			memset(msg, 0, MAX_PATH);
			sprintf_s(msg, MAX_PATH, "Не удалось прочитать значение %s, [dim], ymin.", filename);					
			throw msg;
		}
		if (!_ini.getValue("dim", "nx", nx)) 
		{
			memset(msg, 0, MAX_PATH);
			sprintf_s(msg, MAX_PATH, "Не удалось прочитать значение %s, [dim], nx.", filename);					
			throw msg;
		}
		if (!_ini.getValue("dim", "ny", ny)) 
		{
			memset(msg, 0, MAX_PATH);
			sprintf_s(msg, MAX_PATH, "Не удалось прочитать значение %s, [dim], ny.", filename);					
			throw msg;
		}
		if (!_ini.getValue("dim", "xstep", xstep)) 
		{
			memset(msg, 0, MAX_PATH);
			sprintf_s(msg, MAX_PATH, "Не удалось прочитать значение %s, [dim], xstep.", filename);					
			throw msg;
		}
		if (!_ini.getValue("dim", "ystep", ystep)) 
		{
			memset(msg, 0, MAX_PATH);
			sprintf_s(msg, MAX_PATH, "Не удалось прочитать значение %s, [dim], ystep.", filename);					
			throw msg;
		}
		
		space.xMin = xmin;
		space.yMin = ymin;
		space.nX = nx;
		space.nY = ny;
		space.xStep = xstep;
		space.yStep = ystep;
		space.getxMax();
		space.getyMax();		
		
		CString line, path;		
		int i;
		if (!_ini.getWide("dim", "layer", &sa)) 
		{
			memset(msg, 0, MAX_PATH);
			sprintf_s(msg, MAX_PATH, "Не удалось прочитать значение %s, [dim], layer.", filename);					
			throw msg;
		}
		for (i = 0; i < sa.GetSize(); i++)
		{			
			space.nZ += atoi(sa[i]);
		}
		sa.RemoveAll();
		// read active cells		
		UINT total = (space.nX - 1) * (space.nY - 1) * space.nZ;
		// arrays
		pbarr = new bool[total];
		memset(pbarr, 1, total);			
		strcpy_s(actn, MAX_PATH, dir);
		strcat_s(actn, MAX_PATH, "actn.bin");
		
		FILE* stream; 
		fopen_s(&stream, actn, "rb");		
		if (stream)				
		{
			fread(pbarr, 1, total, stream);
			fclose(stream);	
		}		
		size = space.square();
		pdval = new DWORD[size];
		pfval = new float[size];
		// read struct				
		if (!_ini.getValue("files", "struct", path)) 
		{
			memset(msg, 0, MAX_PATH);
			sprintf_s(msg, MAX_PATH, "Не удалось прочитать значение %s, [files], struct.", filename);					
			throw msg;
		}
		printf("reading %s\n", path);		
		iext.file = path;
		iext.file.Insert(0, dir);
		iext.chext(".bin");			
		if (!structConvert(&iext)) throw 0;	
		// params
		int params;
		_ini.getValue("params", "pcount", params);		
		UCHAR j;
		for (i = 0; i < params; i++)
		{			
			path.Format("p%u", i);
			if (!_ini.getValue("params", path, line))
			{
				memset(msg, 0, MAX_PATH);
				sprintf_s(msg, MAX_PATH, "Не удалось прочитать значение %s, [params], %s.", filename, path);					
				throw msg;
			}
			j = line.Find("=");						
			path = line.Right(line.GetLength() - ++j);														
			printf("reading %s\n", path);			

			iext.file = path;						
			iext.file.Insert(0, dir);			
			iext.chext(".bin");				
			
			if (!paramConvert(&iext)) throw 0;			
		}	
		// roxar
		bool bhistory = 0;
		_ini.getValue("in", "history", bhistory);
		if (bhistory)
		{			
			if (!_ini.getValue("date", "periods", periods)) 
			{
				memset(msg, 0, MAX_PATH);
				sprintf_s(msg, MAX_PATH, "Не удалось прочитать значение %s, [date], periods.", filename);					
				throw msg;
			}
			if (!_ini.getValue("roxar", "rcount", params)) 
			{
				memset(msg, 0, MAX_PATH);
				sprintf_s(msg, MAX_PATH, "Не удалось прочитать значение %s, [roxar], rcount.", filename);					
				throw msg;
			}			
			for (i = 0; i < params; i++)
			{				
				path.Format("r%u", i);
				if (!_ini.getValue("roxar", path, line))
				{
					memset(msg, 0, MAX_PATH);
					sprintf_s(msg, MAX_PATH, "Не удалось прочитать значение %s, [roxar], %s.", filename, path);					
					throw msg;
				}
				j = line.Find("=");							
				path = line.Right(line.GetLength() - ++j);												
				printf("reading %s\n", path);		

				iext.file = path;				
				iext.file.Insert(0, dir);			
				iext.chext(".bin");	
			
				if (!roxarConvert(&iext)) throw 0;				
			}
		}
		_ini.setValue("out", "bin", 1);		
	}
	catch (int)
	{
		bres = 0;		
	}
	catch (char*)
	{
		writelog(msg, _log);
		bres = 0;
	}	
	SAFE_DELETE_ARRAY(pbarr);
	SAFE_DELETE_ARRAY(pdval);
	SAFE_DELETE_ARRAY(pfval);	
	
	return bres;
}
bool loadRegSettings()
{
	HKEY hKey;
	DWORD dwType, dwSize;
	char subKey[_MAX_PATH];
	// common
	memset(subKey, 0, _MAX_PATH);
	strcpy_s(subKey, MAX_PATH, "Software\\bm\\Model view\\legend"); 	
	if (ERROR_SUCCESS != RegOpenKeyEx(HKEY_CURRENT_USER, subKey, 0, KEY_READ, &hKey)) return 0; 		
	
	dwType = REG_DWORD;
	dwSize = 4;		

	dwType = REG_BINARY;	
	RegQueryValueEx(hKey, "legend", 0, &dwType, 0, &dwSize);  			
	if (dwSize != 0)	
	{
		_c_set._size = dwSize >> 2;		
		_c_set.setArr();
		RegQueryValueEx(hKey, "legend", 0, &dwType, (LPBYTE)_c_set.colors, &dwSize);  		
	}
	dwType = REG_DWORD;
	dwSize = 4;
	
	RegQueryValueEx(hKey, "gray", 0, &dwType, (LPBYTE)&_c_set.bgray, &dwSize);  		
	RegQueryValueEx(hKey, "inv", 0, &dwType, (LPBYTE)&_c_set.binv, &dwSize);  		
	RegCloseKey(hKey);	
	
	return 1;
}
int main(int argc, char* argv[])
{		
	if (argc == 1)
	{
		printf("usage: mkbin [filename]\n"); 	
		return 0;
	}
	// head
	printf("\n");
	printf("\t\t\t----------------------\n");
	printf("\t\t\tColor Binary Convertor\n");
	printf("\t\t\t----------------------\n");
	printf("\n");	

	bool bres = 1;	
	
	WIN32_FIND_DATA find;
	memset(&find, 0, 320);

	char file[6];			
	memset(file, 0, 6);		
	strcpy_s(file, 6, "*.log");	
	
	HANDLE hfile = FindFirstFile(file, &find);	
	memset(_log, 0, MAX_PATH);
	strcpy_s(_log, MAX_PATH, find.cFileName); 
	
	if (hfile == INVALID_HANDLE_VALUE) 
	{				
		strcpy_s(_log, MAX_PATH, argv[0]);
		PathRenameExtension(_log, ".log");		
	}
	FindClose(hfile);	

	try
	{		
		if (!AfxWinInit(::GetModuleHandle(0), 0, ::GetCommandLine(), 0)) 
			throw "Не удалось инициализировать библиотеку MFC";	
		if (!loadRegSettings()) writelog("Внимание! Невозможно считать настройки, будут использоваться настройки по умолчанию", _log);		
		if (!binConvert(argv[1])) bres = 0;					
	}	
	catch (char* msg)
	{
		writelog(msg, _log);			
		printf_s("errors found\n");				
		bres = 0;
	}
	SAFE_DELETE_ARRAY(_c_set.colors);
	
	if (bres)
	{
		printf_s("ok!\n");
		return 0;
	}
	else return 1;	
}