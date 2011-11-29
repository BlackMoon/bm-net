// doc.h
#pragma once
#pragma comment(lib, "../ext3/release/ext3")

#include "graph.h"
#include "mem.h"
// C3dDoc
class C3dDoc : public CDocument
{
	DECLARE_DYNCREATE(C3dDoc)
protected:
	C3dDoc();		
	void DeleteContents();		
	void SetPathName(LPCTSTR lpszPathName, BOOL);
public:
	BOOL OnOpenDocument(LPCTSTR lpszPathName);	
	~C3dDoc();
// Attributes
private:	
	CGraph*					pgraph;		
protected:
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()				
		
	BOOL GetAuto();
	BOOL GetLegendGray();
	BOOL GetLegendInv();
	BOOL Open(LPCTSTR FileName, BOOL Read);	
	BOOL SetLegend(VARIANT* VariantArray);
	BSTR GetWellFacenameFont();
	FLOAT GetZCoef();		
	LONG GetLabelColor();	
	LONG GetBackColor();	
	LONG GetCellColor();	
	LONG GetWellColor();	
	LONG GetWellColorFont();	
	LONG GetWellColorSel();	
	LONG GetWellHFont();	
	LONG GetWellLabelXOffset();	
	LONG GetWellLabelYOffset();	
	ULONG GetWellH();	
	ULONG GetWellI();	
	ULONG GetWellR1();	
	ULONG GetWellR2();		
	VARIANT GetWellCharsetFont();			
	IDispatch* GetGraph();	
	
	void Save(LPCTSTR FileName, VARIANT* DestFormat);	
	void SetAuto(BOOL newVal);	
	void SetBackColor(LONG newVal);
	void SetCellColor(LONG newVal);	
	void SetLabelColor(LONG newVal);
	void SetLegendGray(BOOL newVal);	
	void SetLegendInv(BOOL newVal);
	void SetWellCharsetFont(VARIANT newVal);
	void SetWellColor(LONG newVal);
	void SetWellColorFont(LONG newVal);	
	void SetWellColorSel(LONG newVal);
	void SetWellFacenameFont(LPCTSTR newVal);	
	void SetWellH(ULONG newVal);
	void SetWellHFont(LONG newVal);
	void SetWellI(ULONG newVal);
	void SetWellLabelXOffset(LONG newVal);
	void SetWellLabelYOffset(LONG newVal);	
	void SetWellR1(ULONG newVal);
	void SetWellR2(ULONG newVal);	
	void SetZCoef(FLOAT newVal);	
	void Show();	

	enum 
	{
		dispidSetLegend = 24L,		dispidSave = 23L,		dispidOpen = 22L,		dispidgetGraph = 21L,			dispidShow = 20L,			dispidWellCharsetFont = 19,		dispidWellR2 = 18,		dispidWellR1 = 17,		dispidWellI = 16,			dispidWellH = 15,			dispidWellLabelYOffset = 14,		dispidWellLabelXOffset = 13,		dispidWellHFont = 12,		dispidWellColorSel = 11,		dispidWellColorFont = 10L,			dispidWellColor = 9,		dispidLabelColor = 8L,			dispidCellColor = 7L,		dispidBackColor = 6L,		dispidZCoef = 5L,		dispidWellFacenameFont = 4L,		dispidLegendInv = 3,		dispidLegendGray = 2,		dispidAuto = 1
	};			
};