

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


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


#ifdef __cplusplus
extern "C"{
#endif 


#include <rpc.h>
#include <rpcndr.h>

#ifdef _MIDL_USE_GUIDDEF_

#ifndef INITGUID
#define INITGUID
#include <guiddef.h>
#undef INITGUID
#else
#include <guiddef.h>
#endif

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        DEFINE_GUID(name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8)

#else // !_MIDL_USE_GUIDDEF_

#ifndef __IID_DEFINED__
#define __IID_DEFINED__

typedef struct _IID
{
    unsigned long x;
    unsigned short s1;
    unsigned short s2;
    unsigned char  c[8];
} IID;

#endif // __IID_DEFINED__

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        const type name = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}

#endif !_MIDL_USE_GUIDDEF_

MIDL_DEFINE_GUID(IID, LIBID_Model,0xBAC9CDCE,0xFBE5,0x4B4D,0x8A,0x91,0xB8,0xAC,0x61,0xFB,0xEA,0x9D);


MIDL_DEFINE_GUID(IID, DIID_IApplication,0x69DF4038,0x7FDA,0x447B,0xBD,0x0D,0x2E,0x09,0xDD,0x5D,0x29,0x8E);


MIDL_DEFINE_GUID(CLSID, CLSID_Application,0x1024D545,0xC833,0x40A5,0x92,0x81,0x95,0x01,0x7A,0x38,0x49,0x73);


MIDL_DEFINE_GUID(IID, DIID_IGraph,0x3ECB516B,0x24CD,0x494B,0x86,0x76,0x1E,0xB3,0x64,0x5E,0xDE,0x16);


MIDL_DEFINE_GUID(CLSID, CLSID_Graph,0x66FCF56C,0x26B5,0x4689,0x9C,0xC0,0xA7,0xAA,0x9C,0x78,0xB9,0x9B);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



