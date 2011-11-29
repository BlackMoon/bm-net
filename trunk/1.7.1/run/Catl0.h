// Machine generated IDispatch wrapper class(es) created with Add Class from Typelib Wizard

#import "D:\\Geology\\atl\\Debug\\atl.tlb" no_namespace
// Catl0 wrapper class

class Catl0 : public COleDispatchDriver
{
public:
	Catl0(){} // Calls COleDispatchDriver default constructor
	Catl0(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	Catl0(const Catl0& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

	// Attributes
public:

	// Operations
public:


	// Iatl methods
public:
	void m1(long p1, BOOL p2)
	{
		static BYTE parms[] = VTS_I4 VTS_BOOL ;
		InvokeHelper(0x1, DISPATCH_METHOD, VT_EMPTY, NULL, parms, p1, p2);
	}

	// Iatl properties
public:

};
