// Machine generated IDispatch wrapper class(es) created with Add Class from Typelib Wizard

// CGraph wrapper class

class CGraph : public COleDispatchDriver
{
public:
	CGraph(){} // Calls COleDispatchDriver default constructor
	CGraph(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	CGraph(const CGraph& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

	// Attributes
public:

	// Operations
public:


	// IGraph methods
public:
	void BackView()
	{
		InvokeHelper(0x1, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	void BottomView()
	{
		InvokeHelper(0x2, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	void FrontView()
	{
		InvokeHelper(0x3, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	void LeftView()
	{
		InvokeHelper(0x4, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	void ResetView()
	{
		InvokeHelper(0x5, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	void RightView()
	{
		InvokeHelper(0x6, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	void TopView()
	{
		InvokeHelper(0x7, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	SCODE ResetFilter()
	{
		SCODE result;
		InvokeHelper(0x8, DISPATCH_METHOD, VT_ERROR, (void*)&result, NULL);
		return result;
	}
	SCODE SetDims(long x0, long x1, long y0, long y1, long z0, long z1)
	{
		SCODE result;
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_I4 ;
		InvokeHelper(0x9, DISPATCH_METHOD, VT_ERROR, (void*)&result, parms, x0, x1, y0, y1, z0, z1);
		return result;
	}
	void ShowCCW(BOOL showCell, BOOL showCont, BOOL showWell)
	{
		static BYTE parms[] = VTS_BOOL VTS_BOOL VTS_BOOL ;
		InvokeHelper(0xa, DISPATCH_METHOD, VT_EMPTY, NULL, parms, showCell, showCont, showWell);
	}

	// IGraph properties
public:

};
