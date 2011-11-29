

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0366 */
/* at Sun Jun 17 10:12:48 2007
 */
/* Compiler settings for .\tri.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 440
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __tri_h_h__
#define __tri_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IApplication_FWD_DEFINED__
#define __IApplication_FWD_DEFINED__
typedef interface IApplication IApplication;
#endif 	/* __IApplication_FWD_DEFINED__ */


#ifndef __Application_FWD_DEFINED__
#define __Application_FWD_DEFINED__

#ifdef __cplusplus
typedef class Application Application;
#else
typedef struct Application Application;
#endif /* __cplusplus */

#endif 	/* __Application_FWD_DEFINED__ */


#ifndef __IGraph_FWD_DEFINED__
#define __IGraph_FWD_DEFINED__
typedef interface IGraph IGraph;
#endif 	/* __IGraph_FWD_DEFINED__ */


#ifndef __Graph_FWD_DEFINED__
#define __Graph_FWD_DEFINED__

#ifdef __cplusplus
typedef class Graph Graph;
#else
typedef struct Graph Graph;
#endif /* __cplusplus */

#endif 	/* __Graph_FWD_DEFINED__ */


#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 


#ifndef __Model_LIBRARY_DEFINED__
#define __Model_LIBRARY_DEFINED__

/* library Model */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_Model;

#ifndef __IApplication_DISPINTERFACE_DEFINED__
#define __IApplication_DISPINTERFACE_DEFINED__

/* dispinterface IApplication */
/* [uuid] */ 


EXTERN_C const IID DIID_IApplication;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("69DF4038-7FDA-447B-BD0D-2E09DD5D298E")
    IApplication : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct IApplicationVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IApplication * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IApplication * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IApplication * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IApplication * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IApplication * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IApplication * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IApplication * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } IApplicationVtbl;

    interface IApplication
    {
        CONST_VTBL struct IApplicationVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IApplication_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IApplication_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IApplication_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IApplication_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IApplication_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IApplication_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IApplication_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __IApplication_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_Application;

#ifdef __cplusplus

class DECLSPEC_UUID("1024D545-C833-40A5-9281-95017A384973")
Application;
#endif

#ifndef __IGraph_DISPINTERFACE_DEFINED__
#define __IGraph_DISPINTERFACE_DEFINED__

/* dispinterface IGraph */
/* [uuid] */ 


EXTERN_C const IID DIID_IGraph;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("3ECB516B-24CD-494B-8676-1EB3645EDE16")
    IGraph : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct IGraphVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IGraph * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IGraph * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IGraph * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IGraph * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IGraph * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IGraph * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IGraph * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } IGraphVtbl;

    interface IGraph
    {
        CONST_VTBL struct IGraphVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IGraph_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IGraph_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IGraph_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IGraph_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IGraph_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IGraph_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IGraph_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __IGraph_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_Graph;

#ifdef __cplusplus

class DECLSPEC_UUID("66FCF56C-26B5-4689-9CC0-A7AA9C78B99B")
Graph;
#endif
#endif /* __Model_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


