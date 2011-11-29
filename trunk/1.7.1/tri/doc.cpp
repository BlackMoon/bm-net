// doc.cpp
#include "stdafx.h"
#include "direct.h"
#include "doc.h"

BOOL						bread = 0;

extern bool					bmove;
extern char					_log[MAX_PATH], msg[MAX_PATH];
extern SPACE				space;
extern stockline			_stockline;
extern LPDIRECT3DDEVICE9	lpd3d;	
// C3dDoc
IMPLEMENT_DYNCREATE(C3dDoc, CDocument)

BEGIN_DISPATCH_MAP(C3dDoc, CDocument)						
	DISP_PROPERTY_EX_ID(C3dDoc, "Auto", dispidAuto, GetAuto, SetAuto, VT_BOOL)
	DISP_PROPERTY_EX_ID(C3dDoc, "LegendGray", dispidLegendGray, GetLegendGray, SetLegendGray, VT_BOOL)
	DISP_PROPERTY_EX_ID(C3dDoc, "LegendInv", dispidLegendInv, GetLegendInv, SetLegendInv, VT_BOOL)
	DISP_PROPERTY_EX_ID(C3dDoc, "WellFacenameFont", dispidWellFacenameFont, GetWellFacenameFont, SetWellFacenameFont, VT_BSTR)			
	DISP_PROPERTY_EX_ID(C3dDoc, "ZCoef", dispidZCoef, GetZCoef, SetZCoef, VT_R4)
	DISP_PROPERTY_EX_ID(C3dDoc, "BackColor", dispidBackColor, GetBackColor, SetBackColor, VT_I4)
	DISP_PROPERTY_EX_ID(C3dDoc, "CellColor", dispidCellColor, GetCellColor, SetCellColor, VT_I4)
	DISP_PROPERTY_EX_ID(C3dDoc, "LabelColor", dispidLabelColor, GetLabelColor, SetLabelColor, VT_I4)	
	DISP_PROPERTY_EX_ID(C3dDoc, "WellColor", dispidWellColor, GetWellColor, SetWellColor, VT_I4)
	DISP_PROPERTY_EX_ID(C3dDoc, "WellColorFont", dispidWellColorFont, GetWellColorFont, SetWellColorFont, VT_I4)	
	DISP_PROPERTY_EX_ID(C3dDoc, "WellColorSel", dispidWellColorSel, GetWellColorSel, SetWellColorSel, VT_I4)
	DISP_PROPERTY_EX_ID(C3dDoc, "WellHFont", dispidWellHFont, GetWellHFont, SetWellHFont, VT_I4)	
	DISP_PROPERTY_EX_ID(C3dDoc, "WellLabelXOffset", dispidWellLabelXOffset, GetWellLabelXOffset, SetWellLabelXOffset, VT_I4)
	DISP_PROPERTY_EX_ID(C3dDoc, "WellLabelYOffset", dispidWellLabelYOffset, GetWellLabelYOffset, SetWellLabelYOffset, VT_I4)
	DISP_PROPERTY_EX_ID(C3dDoc, "WellH", dispidWellH, GetWellH, SetWellH, VT_UI4)
	DISP_PROPERTY_EX_ID(C3dDoc, "WellI", dispidWellI, GetWellI, SetWellI, VT_UI4)
	DISP_PROPERTY_EX_ID(C3dDoc, "WellR1", dispidWellR1, GetWellR1, SetWellR1, VT_UI4)
	DISP_PROPERTY_EX_ID(C3dDoc, "WellR2", dispidWellR2, GetWellR2, SetWellR2, VT_UI4)
	DISP_PROPERTY_EX_ID(C3dDoc, "WellCharsetFont", dispidWellCharsetFont, GetWellCharsetFont, SetWellCharsetFont, VT_VARIANT)	
	DISP_FUNCTION_ID(C3dDoc, "Show", dispidShow, Show, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION_ID(C3dDoc, "GetGraph", dispidgetGraph, GetGraph, VT_DISPATCH, VTS_NONE)
	DISP_FUNCTION_ID(C3dDoc, "Open", dispidOpen, Open, VT_BOOL, VTS_BSTR VTS_BOOL)		
	DISP_FUNCTION_ID(C3dDoc, "SetLegend", dispidSetLegend, SetLegend, VT_BOOL, VTS_PVARIANT)		
	DISP_FUNCTION_ID(C3dDoc, "Save", dispidSave, Save, VT_EMPTY, VTS_BSTR VTS_PVARIANT)
END_DISPATCH_MAP()
// {69DF4038-7FDA-447B-BD0D-2E09DD5D298E}
static const IID			IID_IApplication = {0x69DF4038, 0x7FDA, 0x447B, 
											   {0xBD, 0xD, 0x2E, 0x9, 0xDD, 0x5D, 0x29, 0x8E}};

BEGIN_INTERFACE_MAP(C3dDoc, CDocument)
	INTERFACE_PART(C3dDoc, IID_IApplication, Dispatch)
END_INTERFACE_MAP()

C3dDoc::C3dDoc()
{
	pgraph = &app.m_graph;

	EnableAutomation();
	AfxOleLockApp();
}
C3dDoc::~C3dDoc()
{
	AfxOleUnlockApp();
}
BOOL C3dDoc::GetAuto()
{
	return app.stg._g_set.bauto;
}
BOOL C3dDoc::GetLegendGray()
{
	return app.stg._c_set.bgray;
}
BOOL C3dDoc::GetLegendInv()
{
	return app.stg._c_set.binv;
}
BOOL C3dDoc::OnOpenDocument(LPCTSTR lpszPathName)
{		
	CFile* pFile = GetFile(lpszPathName,
			CFile::modeRead|CFile::shareDenyWrite, 0);
	try
	{				
		if (pFile == 0)
		{
			memset(msg, 0, MAX_PATH);
			sprintf_s(msg, MAX_PATH, "Не удалось открыть %s.", lpszPathName);		
			throw msg;		
		}
		DeleteContents();
		SetModifiedFlag(0);  	
		ReleaseFile(pFile, 0);

		app.m_pFrame->m_p3dview->Reset3D();
		app.m_pFrame->infoUpdate();		
	
		ini _ini(lpszPathName);			
		_ini.getValue("in", "title", pgraph->m_title);
	
		app.m_pFrame->comboClearData();	
		app.m_pFrame->SetWindowText(pgraph->m_title);		
		
		int xmin, ymin, nx, ny, xstep, ystep;		
		if (!_ini.getValue("dim", "xmin", xmin)) 
		{
			memset(msg, 0, MAX_PATH);
			sprintf_s(msg, MAX_PATH, "Не удалось прочитать значение %s, [dim], xmin.", lpszPathName);		
			throw msg;
		}
		if (!_ini.getValue("dim", "ymin", ymin)) 
		{
			memset(msg, 0, MAX_PATH);
			sprintf_s(msg, MAX_PATH, "Не удалось прочитать значение %s, [dim], ymin.", lpszPathName);					
			throw msg;
		}
		if (!_ini.getValue("dim", "nx", nx)) 
		{
			memset(msg, 0, MAX_PATH);
			sprintf_s(msg, MAX_PATH, "Не удалось прочитать значение %s, [dim], nx.", lpszPathName);					
			throw msg;
		}
		if (!_ini.getValue("dim", "ny", ny)) 
		{
			memset(msg, 0, MAX_PATH);
			sprintf_s(msg, MAX_PATH, "Не удалось прочитать значение %s, [dim], ny.", lpszPathName);					
			throw msg;
		}
		if (!_ini.getValue("dim", "xstep", xstep)) 
		{
			memset(msg, 0, MAX_PATH);
			sprintf_s(msg, MAX_PATH, "Не удалось прочитать значение %s, [dim], ystep.", lpszPathName);					
			throw msg;
		}
		if (!_ini.getValue("dim", "ystep", ystep)) 
		{
			memset(msg, 0, MAX_PATH);
			sprintf_s(msg, MAX_PATH, "Не удалось прочитать значение %s, [dim], ystep.", lpszPathName);					
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
		
		int i;
		UCHAR nLayer;				
		CStringArray sa;		
		if (!_ini.getWide("dim", "layer", &sa)) 
		{
			memset(msg, 0, MAX_PATH);
			sprintf_s(msg, MAX_PATH, "Не удалось прочитать значение %s, [dim], layer.", lpszPathName);					
			throw msg;
		}		
		for (i = 0; i < sa.GetSize(); i++)
		{
			nLayer = atoi(sa[i]);
			pgraph->m_layers.push_back(nLayer);
			space.nZ += nLayer;
		}
		sa.RemoveAll();
		// binary
		bool bbin = 0;
		_ini.getValue("out", "bin", bbin);		
		
		if (!bbin || bread)
		{
			PROCESS_INFORMATION pi; 			
			STARTUPINFO si;	
			
			memset(&pi, 0, 16);
			memset(&si, 0, 68);
			
			si.cb = 68; 					
			
			char cmd[MAX_PATH];			
			memset(cmd, 0, MAX_PATH);			
			getDir(cmd, MAX_PATH, __argv[0]);
			_chdir(cmd);

			sprintf_s(cmd, MAX_PATH, "cbc.exe \"%s\"", lpszPathName);			
			
			if (!CreateProcess(0, cmd, 0, 0, 0, 0, 0, 0, &si, &pi)) 
			{
				memset(msg, 0, MAX_PATH);
				strcpy_s(msg, MAX_PATH, "Не удалось выполнить cbc.exe.");
				throw msg;			
			}				
			WaitForSingleObject(pi.hProcess, INFINITE); 			

			DWORD dw_code;
			GetExitCodeProcess(pi.hProcess, &dw_code);
			CloseHandle(pi.hProcess);
			CloseHandle(pi.hThread);			
			app.m_pFrame->UpdateWindow();
			
			if (dw_code != 0) 
			{
				memset(msg, 0, MAX_PATH);
				sprintf_s(msg, MAX_PATH, "Процесс 0x%x завершился с кодом 0x%x.", pi.dwProcessId, dw_code);
				throw msg;				
			}
		}
		_ini.getValue("files", "cont", pgraph->m_contfile);
		if (!_ini.getValue("files", "struct", pgraph->m_structfile.file))
		{
			memset(msg, 0, MAX_PATH);
			sprintf_s(msg, MAX_PATH, "Не удалось прочитать значение %s, [files], struct.", lpszPathName);					
			throw msg;
		};
		_ini.getValue("files", "wells", pgraph->m_wellfile);			
		
		char dir[MAX_PATH];
		memset(dir, 0, MAX_PATH);
		getDir(dir, MAX_PATH, lpszPathName);
		
		strcpy_s(pgraph->_actn, MAX_PATH, dir);
		strcat_s(pgraph->_actn, MAX_PATH, "actn.bin");		
		
		pgraph->m_contfile.Insert(0, dir);		
		pgraph->m_wellfile.Insert(0, dir);						
		pgraph->m_structfile.file.Insert(0, dir);
		pgraph->m_structfile.chext(".bin");			
		// params
		int params;
		_ini.getValue("params", "pcount", params);
		
		UCHAR j;
		CString line, l, r;	
		iniline* piniline;
		_stockline.init();
		for (i = 0; i < params; i++)
		{
			l.Format("p%u", i);
			if (!_ini.getValue("params", l, line))
			{
				memset(msg, 0, MAX_PATH);
				sprintf_s(msg, MAX_PATH, "Не удалось прочитать значение %s, [params], %s.", lpszPathName, l);					
				throw msg;
			}			
			j = line.Find(61);			
			l = line.Left(j);
			r = line.Right(line.GetLength() - ++j);	
			// stock
			if (r.Right(9) == "stock.txt")
			{
				_stockline.bvalid = 1;
				_stockline.num = i;
			}
			app.m_pFrame->comboAddData(l);
			
			piniline = new iniline(r, l);			
			piniline->file.Insert(0, dir);
			piniline->chext(".bin");			
			pgraph->m_inilines.Add(piniline);	
		}		
		// roxar
		bool bhistory = 0;
		_ini.getValue("in", "history", bhistory);
		if (bhistory)
		{	// roxar dates
			int periods;
			tm date;			
			char buf[32], month[9], year[5];			
			if (!_ini.getValue("date", "periods", periods))
			{
				memset(msg, 0, MAX_PATH);
				sprintf_s(msg, MAX_PATH, "Не удалось прочитать значение %s, [date], periods.", lpszPathName);					
				throw msg;
			}
			for (i = 0; i < periods; i++)
			{				
				l.Format("%u", i);
				_ini.getValue("date", l, date);
				getMonth(month, 9, &date);				
				strftime(year, 5, "%Y", &date);
				
				sprintf_s(buf, 32, "%s %d, %s", month, date.tm_mday, year);
				app.m_pFrame->comboAddData(buf, 0, 1);				
			}			
			// roxar lines
			if (!_ini.getValue("roxar", "rcount", params))
			{
				memset(msg, 0, MAX_PATH);
				sprintf_s(msg, MAX_PATH, "Не удалось прочитать значение %s, [roxar], rcount.", lpszPathName);					
				throw msg;
			}		
			iniwline* piniwline;
			for (i = 0; i < params; i++)
			{
				l.Format("r%u", i);
				if (!_ini.getValue("roxar", l, line))
				{
					memset(msg, 0, MAX_PATH);
					sprintf_s(msg, MAX_PATH, "Не удалось прочитать значение %s, [roxar], %s.", lpszPathName, l);					
					throw msg;
				}							
				j = line.Find(61);			// 61 - '='			
				l = line.Left(j);
				r = line.Right(line.GetLength() - ++j);				
				
				piniwline = new iniwline(r, l);				
				piniwline->file.Insert(0, dir);
				piniwline->chext(".bin");					
				piniwline->periods = periods;
				
				pgraph->m_inilines.Add(piniwline);	
				app.m_pFrame->comboAddData(l, (LPVOID)piniwline);
			}										
		}				

		if (!pgraph->initDeviceObject()) throw msg;
		pgraph->setProj(1);
		
		app.m_pFrame->setParam();		
		app.m_pFrame->m_infobar.setFilter();	
	}
	catch (char*)
	{
		writelog(msg, _log);		
		ReleaseFile(pFile, 1);
		return 0;
	}	
	return 1;
}
BOOL C3dDoc::Open(LPCTSTR FileName, BOOL Read)
{
	bmove = 0;		
	bread = Read;
	return OnOpenDocument(FileName);	
}
BOOL C3dDoc::SetLegend(VARIANT* VariantArray)
{
	BOOL bres = 0;
	
	DWORD* pvdata = 0;	
	SAFEARRAY* psa = VariantArray->parray;
		
	if (psa)
	{
		UINT size = psa->rgsabound[0].cElements;  
		pvdata = (DWORD*)psa->pvData;		
		
		app.stg._c_set._size = size;
		app.stg._c_set.setArr();

		for (UINT i = 0; i < size; i++)
		{
			app.stg._c_set.colors[i] = pvdata[i];
		}	
		bres = 1;
	}	
	return bres;
}
BSTR C3dDoc::GetWellFacenameFont()
{
	CString strResult = app.stg._w_set.ds.FaceName;
	return strResult.AllocSysString();
}
FLOAT C3dDoc::GetZCoef()
{
	return app.stg._g_set.zcoef;
}
LONG C3dDoc::GetBackColor()
{
	return app.stg._g_set.dw_bkgnd;
}
LONG C3dDoc::GetCellColor()
{
	return app.stg._g_set.dw_cell;
}
LONG C3dDoc::GetLabelColor()
{	
	return app.stg._g_set.cl_lfont;
}
LONG C3dDoc::GetWellColor()
{
	return app.stg._w_set.dw_main;
}
LONG C3dDoc::GetWellColorFont()
{
	return app.stg._w_set.dw_font;
}
LONG C3dDoc::GetWellColorSel()
{
	return app.stg._w_set.dw_sel;	
}
LONG C3dDoc::GetWellHFont()
{
	return app.stg._w_set.ds.Height;
}
LONG C3dDoc::GetWellLabelXOffset()
{
	return app.stg._w_set.wxo;
}
LONG C3dDoc::GetWellLabelYOffset()
{
	return app.stg._w_set.wyo;	
}
ULONG C3dDoc::GetWellH()
{
	return app.stg._w_set.wh;
}
ULONG C3dDoc::GetWellI()
{
	return app.stg._w_set.wi;
}
ULONG C3dDoc::GetWellR1()
{
	return app.stg._w_set.wr1;
}
ULONG C3dDoc::GetWellR2()
{
	return app.stg._w_set.wr2;
}
VARIANT C3dDoc::GetWellCharsetFont()
{
	return COleVariant(app.stg._w_set.ds.CharSet);
}
IDispatch* C3dDoc::GetGraph()
{
	return pgraph->GetIDispatch(1);	
}
void C3dDoc::DeleteContents()
{		
	pgraph->clear();		
}
void C3dDoc::Save(LPCTSTR FileName, VARIANT* DestFormat)
{		
	app.m_pFrame->dxsave(FileName, (D3DXIMAGE_FILEFORMAT)DestFormat->lVal);
}
void C3dDoc::Show()
{
	app.main();	
	CDocTemplate* pTemplate = GetDocTemplate();
	pTemplate->InitialUpdateFrame(app.m_pFrame, this, 1);
	if (app.stg._g_set.bauto) Open(app.lpszPath, 0);

	SetModifiedFlag(0);
}
void C3dDoc::SetAuto(BOOL newVal)
{
	app.stg._g_set.bauto = newVal;
	SetModifiedFlag(0);
}
void C3dDoc::SetBackColor(LONG newVal)
{
	app.stg._g_set.dw_bkgnd = newVal;
	SetModifiedFlag(0);
}
void C3dDoc::SetCellColor(LONG newVal)
{	
	app.stg._g_set.dw_cell = newVal;
	SetModifiedFlag(0);
}
void C3dDoc::SetLabelColor(LONG newVal)
{
	app.stg._g_set.cl_lfont = newVal;
	SetModifiedFlag(0);
}
void C3dDoc::SetLegendGray(BOOL newVal)
{
	app.stg._c_set.bgray = newVal;
	SetModifiedFlag(0);
}
void C3dDoc::SetLegendInv(BOOL newVal)
{
	app.stg._c_set.binv = newVal;
	SetModifiedFlag(0);
}
void C3dDoc::SetPathName(LPCTSTR lpszPathName, BOOL)
{
	lstrcpy(app.lpszPath, lpszPathName);
}
void C3dDoc::SetWellCharsetFont(VARIANT newVal)
{
	app.stg._w_set.ds.CharSet = newVal.bVal;
	SetModifiedFlag(0);
}
void C3dDoc::SetWellColor(LONG newVal)
{
	app.stg._w_set.dw_main = newVal;
	SetModifiedFlag(0);
}
void C3dDoc::SetWellColorFont(LONG newVal)
{
	app.stg._w_set.dw_font = newVal;	
	SetModifiedFlag(0);
}
void C3dDoc::SetWellColorSel(LONG newVal)
{
	app.stg._w_set.dw_sel = newVal;	
	SetModifiedFlag(0);
}
void C3dDoc::SetWellLabelXOffset(LONG newVal)
{
	app.stg._w_set.wxo = newVal;
	SetModifiedFlag(0);
}
void C3dDoc::SetWellLabelYOffset(LONG newVal)
{
	app.stg._w_set.wyo = newVal;	
	SetModifiedFlag(0);
}
void C3dDoc::SetWellH(ULONG newVal)
{
	app.stg._w_set.wh = newVal;
	SetModifiedFlag(0);
}
void C3dDoc::SetWellHFont(LONG newVal)
{	
	app.stg._w_set.ds.Height = newVal;
	SetModifiedFlag(0);
}
void C3dDoc::SetWellI(ULONG newVal)
{
	app.stg._w_set.wi = newVal;
	SetModifiedFlag(0);
}
void C3dDoc::SetWellR1(ULONG newVal)
{	
	app.stg._w_set.wr1 = newVal;
	SetModifiedFlag(0);
}
void C3dDoc::SetWellR2(ULONG newVal)
{
	app.stg._w_set.wr2 = newVal;
	SetModifiedFlag(0);
}

void C3dDoc::SetWellFacenameFont(LPCTSTR newVal)
{
	lstrcpy(app.stg._w_set.ds.FaceName, newVal);
	SetModifiedFlag(0);
}

void C3dDoc::SetZCoef(FLOAT newVal)
{	
	app.stg._g_set.zcoef = newVal;
	SetModifiedFlag(0);
}