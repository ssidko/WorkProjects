

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 7.00.0555 */
/* at Fri Oct 30 18:03:15 2015
 */
/* Compiler settings for Number.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 7.00.0555 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__


#ifndef __Number_h_h__
#define __Number_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __INumber_FWD_DEFINED__
#define __INumber_FWD_DEFINED__
typedef interface INumber INumber;
#endif 	/* __INumber_FWD_DEFINED__ */


#ifndef __Number_FWD_DEFINED__
#define __Number_FWD_DEFINED__

#ifdef __cplusplus
typedef class Number Number;
#else
typedef struct Number Number;
#endif /* __cplusplus */

#endif 	/* __Number_FWD_DEFINED__ */


#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __Number_LIBRARY_DEFINED__
#define __Number_LIBRARY_DEFINED__

/* library Number */
/* [version][uuid] */ 


EXTERN_C const IID LIBID_Number;

#ifndef __INumber_DISPINTERFACE_DEFINED__
#define __INumber_DISPINTERFACE_DEFINED__

/* dispinterface INumber */
/* [uuid] */ 


EXTERN_C const IID DIID_INumber;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("27ED1D89-F8CF-4A66-86DE-75307C3B2C00")
    INumber : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct INumberVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            INumber * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            INumber * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            INumber * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            INumber * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            INumber * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            INumber * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            INumber * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } INumberVtbl;

    interface INumber
    {
        CONST_VTBL struct INumberVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INumber_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define INumber_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define INumber_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define INumber_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define INumber_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define INumber_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define INumber_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __INumber_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_Number;

#ifdef __cplusplus

class DECLSPEC_UUID("A47A38F5-D31D-49AC-B4BE-6070C4444757")
Number;
#endif
#endif /* __Number_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


