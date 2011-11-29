// Machine generated IDispatch wrapper class(es) created with Add Class from Typelib Wizard

// CApplication wrapper class

class CApplication : public COleDispatchDriver
{
public:
	CApplication(){} // Calls COleDispatchDriver default constructor
	CApplication(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	CApplication(const CApplication& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

	// Attributes
public:

	// Operations
public:


	// IApplication methods
public:
	void Show()
	{
		InvokeHelper(0x14, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	LPDISPATCH GetGraph()
	{
		LPDISPATCH result;
		InvokeHelper(0x15, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, NULL);
		return result;
	}
	BOOL Open(LPCTSTR FileName, BOOL Read)
	{
		BOOL result;
		static BYTE parms[] = VTS_BSTR VTS_BOOL ;
		InvokeHelper(0x16, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms, FileName, Read);
		return result;
	}
	void Save(LPCTSTR FileName, VARIANT * DestFormat)
	{
		static BYTE parms[] = VTS_BSTR VTS_PVARIANT ;
		InvokeHelper(0x17, DISPATCH_METHOD, VT_EMPTY, NULL, parms, FileName, DestFormat);
	}
	BOOL SetLegend(VARIANT * VariantArray)
	{
		BOOL result;
		static BYTE parms[] = VTS_PVARIANT ;
		InvokeHelper(0x18, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms, VariantArray);
		return result;
	}

	// IApplication properties
public:
BOOL GetAuto()
{
	BOOL result;
	GetProperty(0x1, VT_BOOL, (void*)&result);
	return result;
}
void SetAuto(BOOL propVal)
{
	SetProperty(0x1, VT_BOOL, propVal);
}
BOOL GetLegendGray()
{
	BOOL result;
	GetProperty(0x2, VT_BOOL, (void*)&result);
	return result;
}
void SetLegendGray(BOOL propVal)
{
	SetProperty(0x2, VT_BOOL, propVal);
}
BOOL GetLegendInv()
{
	BOOL result;
	GetProperty(0x3, VT_BOOL, (void*)&result);
	return result;
}
void SetLegendInv(BOOL propVal)
{
	SetProperty(0x3, VT_BOOL, propVal);
}
CString GetWellFacenameFont()
{
	CString result;
	GetProperty(0x4, VT_BSTR, (void*)&result);
	return result;
}
void SetWellFacenameFont(CString propVal)
{
	SetProperty(0x4, VT_BSTR, propVal);
}
float GetZCoef()
{
	float result;
	GetProperty(0x5, VT_R4, (void*)&result);
	return result;
}
void SetZCoef(float propVal)
{
	SetProperty(0x5, VT_R4, propVal);
}
long GetBackColor()
{
	long result;
	GetProperty(0x6, VT_I4, (void*)&result);
	return result;
}
void SetBackColor(long propVal)
{
	SetProperty(0x6, VT_I4, propVal);
}
long GetCellColor()
{
	long result;
	GetProperty(0x7, VT_I4, (void*)&result);
	return result;
}
void SetCellColor(long propVal)
{
	SetProperty(0x7, VT_I4, propVal);
}
long GetLabelColor()
{
	long result;
	GetProperty(0x8, VT_I4, (void*)&result);
	return result;
}
void SetLabelColor(long propVal)
{
	SetProperty(0x8, VT_I4, propVal);
}
long GetWellColor()
{
	long result;
	GetProperty(0x9, VT_I4, (void*)&result);
	return result;
}
void SetWellColor(long propVal)
{
	SetProperty(0x9, VT_I4, propVal);
}
long GetWellColorFont()
{
	long result;
	GetProperty(0xa, VT_I4, (void*)&result);
	return result;
}
void SetWellColorFont(long propVal)
{
	SetProperty(0xa, VT_I4, propVal);
}
long GetWellColorSel()
{
	long result;
	GetProperty(0xb, VT_I4, (void*)&result);
	return result;
}
void SetWellColorSel(long propVal)
{
	SetProperty(0xb, VT_I4, propVal);
}
long GetWellHFont()
{
	long result;
	GetProperty(0xc, VT_I4, (void*)&result);
	return result;
}
void SetWellHFont(long propVal)
{
	SetProperty(0xc, VT_I4, propVal);
}
long GetWellLabelXOffset()
{
	long result;
	GetProperty(0xd, VT_I4, (void*)&result);
	return result;
}
void SetWellLabelXOffset(long propVal)
{
	SetProperty(0xd, VT_I4, propVal);
}
long GetWellLabelYOffset()
{
	long result;
	GetProperty(0xe, VT_I4, (void*)&result);
	return result;
}
void SetWellLabelYOffset(long propVal)
{
	SetProperty(0xe, VT_I4, propVal);
}
unsigned long GetWellH()
{
	unsigned long result;
	GetProperty(0xf, VT_UI4, (void*)&result);
	return result;
}
void SetWellH(unsigned long propVal)
{
	SetProperty(0xf, VT_UI4, propVal);
}
unsigned long GetWellI()
{
	unsigned long result;
	GetProperty(0x10, VT_UI4, (void*)&result);
	return result;
}
void SetWellI(unsigned long propVal)
{
	SetProperty(0x10, VT_UI4, propVal);
}
unsigned long GetWellR1()
{
	unsigned long result;
	GetProperty(0x11, VT_UI4, (void*)&result);
	return result;
}
void SetWellR1(unsigned long propVal)
{
	SetProperty(0x11, VT_UI4, propVal);
}
unsigned long GetWellR2()
{
	unsigned long result;
	GetProperty(0x12, VT_UI4, (void*)&result);
	return result;
}
void SetWellR2(unsigned long propVal)
{
	SetProperty(0x12, VT_UI4, propVal);
}
VARIANT GetWellCharsetFont()
{
	VARIANT result;
	GetProperty(0x13, VT_VARIANT, (void*)&result);
	return result;
}
void SetWellCharsetFont(VARIANT propVal)
{
	SetProperty(0x13, VT_VARIANT, propVal);
}

};
