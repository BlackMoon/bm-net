// template.h
#pragma once
#include <afxtempl.h>		
// delPtrArray
template <class BASE_CLASS, class TYPE>
class delPtrArray : public CTypedPtrArray<BASE_CLASS, TYPE>
{
public:
	void RemoveAll()
	{
		ASSERT_VALID(this);
		for (int i = 0; i < GetSize(); i++)
		{
			delete GetAt(i);
		}				
		BASE_CLASS::RemoveAll();
	}
};
// delPtrList
template <class BASE_CLASS, class TYPE>
class delPtrList : public CTypedPtrList<BASE_CLASS, TYPE>
{
public:		
	void RemoveAll()
	{		
		ASSERT_VALID(this);								
		POSITION pos = GetTailPosition();		
		while (pos)	delete GetPrev(pos);			
		BASE_CLASS::RemoveAll();				
	}
};