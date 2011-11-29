// Machine generated IDispatch wrapper class(es) created with Add Class from Typelib Wizard

// Catl wrapper class

class Catl : public COleDispatchDriver
{
public:
	Catl(){} // Calls COleDispatchDriver default constructor
	Catl(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	Catl(const Catl& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

	// Attributes
public:

	// Operations
public:


	// Iatl methods
public:
	BOOL get_Visible()
	{
		BOOL result;
		InvokeHelper(0x1, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
		return result;
	}
	void put_Visible(BOOL newValue)
	{
		static BYTE parms[] = VTS_BOOL ;
		InvokeHelper(0x1, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	void show()
	{
		InvokeHelper(0x2, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}

	// Iatl properties
public:

};
