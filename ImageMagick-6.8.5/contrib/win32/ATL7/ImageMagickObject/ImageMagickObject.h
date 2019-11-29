

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 7.00.0555 */
/* at Sun Jun 24 11:51:08 2012
 */
/* Compiler settings for ImageMagickObject.idl:
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

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __ImageMagickObject_h__
#define __ImageMagickObject_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IMagickImage_FWD_DEFINED__
#define __IMagickImage_FWD_DEFINED__
typedef interface IMagickImage IMagickImage;
#endif 	/* __IMagickImage_FWD_DEFINED__ */


#ifndef __MagickImage_FWD_DEFINED__
#define __MagickImage_FWD_DEFINED__

#ifdef __cplusplus
typedef class MagickImage MagickImage;
#else
typedef struct MagickImage MagickImage;
#endif /* __cplusplus */

#endif 	/* __MagickImage_FWD_DEFINED__ */


/* header files for imported files */
#include "prsht.h"
#include "mshtml.h"
#include "mshtmhst.h"
#include "exdisp.h"
#include "objsafe.h"
#include "comadmin.h"
#include "transact.h"
#include "txcoord.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __IMagickImage_INTERFACE_DEFINED__
#define __IMagickImage_INTERFACE_DEFINED__

/* interface IMagickImage */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IMagickImage;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("7F670536-00AE-4EDF-B06F-13BD22B25624")
    IMagickImage : public IDispatch
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE OnStartPage( 
            /* [in] */ IUnknown *piUnk) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnEndPage( void) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [id][vararg] */ HRESULT STDMETHODCALLTYPE Add( 
            /* [out][in] */ SAFEARRAY * *pArrayVar,
            /* [retval][out] */ VARIANT *pVar) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Remove( 
            /* [in] */ VARIANT varIndex) = 0;
        
        virtual /* [id][vararg] */ HRESULT STDMETHODCALLTYPE Compare( 
            /* [out][in] */ SAFEARRAY * *pArrayVar,
            /* [retval][out] */ VARIANT *pVar) = 0;
        
        virtual /* [id][vararg] */ HRESULT STDMETHODCALLTYPE Composite( 
            /* [out][in] */ SAFEARRAY * *pArrayVar,
            /* [retval][out] */ VARIANT *pVar) = 0;
        
        virtual /* [id][vararg] */ HRESULT STDMETHODCALLTYPE Convert( 
            /* [out][in] */ SAFEARRAY * *pArrayVar,
            /* [retval][out] */ VARIANT *pVar) = 0;
        
        virtual /* [id][vararg] */ HRESULT STDMETHODCALLTYPE Identify( 
            /* [out][in] */ SAFEARRAY * *pArrayVar,
            /* [retval][out] */ VARIANT *pVar) = 0;
        
        virtual /* [id][vararg] */ HRESULT STDMETHODCALLTYPE Mogrify( 
            /* [out][in] */ SAFEARRAY * *pArrayVar,
            /* [retval][out] */ VARIANT *pVar) = 0;
        
        virtual /* [id][vararg] */ HRESULT STDMETHODCALLTYPE Montage( 
            /* [out][in] */ SAFEARRAY * *pArrayVar,
            /* [retval][out] */ VARIANT *pVar) = 0;
        
        virtual /* [id][vararg] */ HRESULT STDMETHODCALLTYPE Stream( 
            /* [out][in] */ SAFEARRAY * *pArrayVar,
            /* [retval][out] */ VARIANT *pVar) = 0;
        
        virtual /* [id][vararg] */ HRESULT STDMETHODCALLTYPE TestHarness( 
            /* [out][in] */ SAFEARRAY * *pArrayVar,
            /* [retval][out] */ VARIANT *pVar) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ LPUNKNOWN *pVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Item( 
            /* [in] */ VARIANT varIndex,
            /* [retval][out] */ VARIANT *pVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Messages( 
            /* [retval][out] */ VARIANT *pVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IMagickImageVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IMagickImage * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IMagickImage * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IMagickImage * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IMagickImage * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IMagickImage * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IMagickImage * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IMagickImage * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        HRESULT ( STDMETHODCALLTYPE *OnStartPage )( 
            IMagickImage * This,
            /* [in] */ IUnknown *piUnk);
        
        HRESULT ( STDMETHODCALLTYPE *OnEndPage )( 
            IMagickImage * This);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Count )( 
            IMagickImage * This,
            /* [retval][out] */ long *pVal);
        
        /* [id][vararg] */ HRESULT ( STDMETHODCALLTYPE *Add )( 
            IMagickImage * This,
            /* [out][in] */ SAFEARRAY * *pArrayVar,
            /* [retval][out] */ VARIANT *pVar);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *Remove )( 
            IMagickImage * This,
            /* [in] */ VARIANT varIndex);
        
        /* [id][vararg] */ HRESULT ( STDMETHODCALLTYPE *Compare )( 
            IMagickImage * This,
            /* [out][in] */ SAFEARRAY * *pArrayVar,
            /* [retval][out] */ VARIANT *pVar);
        
        /* [id][vararg] */ HRESULT ( STDMETHODCALLTYPE *Composite )( 
            IMagickImage * This,
            /* [out][in] */ SAFEARRAY * *pArrayVar,
            /* [retval][out] */ VARIANT *pVar);
        
        /* [id][vararg] */ HRESULT ( STDMETHODCALLTYPE *Convert )( 
            IMagickImage * This,
            /* [out][in] */ SAFEARRAY * *pArrayVar,
            /* [retval][out] */ VARIANT *pVar);
        
        /* [id][vararg] */ HRESULT ( STDMETHODCALLTYPE *Identify )( 
            IMagickImage * This,
            /* [out][in] */ SAFEARRAY * *pArrayVar,
            /* [retval][out] */ VARIANT *pVar);
        
        /* [id][vararg] */ HRESULT ( STDMETHODCALLTYPE *Mogrify )( 
            IMagickImage * This,
            /* [out][in] */ SAFEARRAY * *pArrayVar,
            /* [retval][out] */ VARIANT *pVar);
        
        /* [id][vararg] */ HRESULT ( STDMETHODCALLTYPE *Montage )( 
            IMagickImage * This,
            /* [out][in] */ SAFEARRAY * *pArrayVar,
            /* [retval][out] */ VARIANT *pVar);
        
        /* [id][vararg] */ HRESULT ( STDMETHODCALLTYPE *Stream )( 
            IMagickImage * This,
            /* [out][in] */ SAFEARRAY * *pArrayVar,
            /* [retval][out] */ VARIANT *pVar);
        
        /* [id][vararg] */ HRESULT ( STDMETHODCALLTYPE *TestHarness )( 
            IMagickImage * This,
            /* [out][in] */ SAFEARRAY * *pArrayVar,
            /* [retval][out] */ VARIANT *pVar);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get__NewEnum )( 
            IMagickImage * This,
            /* [retval][out] */ LPUNKNOWN *pVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Item )( 
            IMagickImage * This,
            /* [in] */ VARIANT varIndex,
            /* [retval][out] */ VARIANT *pVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Messages )( 
            IMagickImage * This,
            /* [retval][out] */ VARIANT *pVal);
        
        END_INTERFACE
    } IMagickImageVtbl;

    interface IMagickImage
    {
        CONST_VTBL struct IMagickImageVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMagickImage_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IMagickImage_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IMagickImage_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IMagickImage_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IMagickImage_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IMagickImage_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IMagickImage_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IMagickImage_OnStartPage(This,piUnk)	\
    ( (This)->lpVtbl -> OnStartPage(This,piUnk) ) 

#define IMagickImage_OnEndPage(This)	\
    ( (This)->lpVtbl -> OnEndPage(This) ) 

#define IMagickImage_get_Count(This,pVal)	\
    ( (This)->lpVtbl -> get_Count(This,pVal) ) 

#define IMagickImage_Add(This,pArrayVar,pVar)	\
    ( (This)->lpVtbl -> Add(This,pArrayVar,pVar) ) 

#define IMagickImage_Remove(This,varIndex)	\
    ( (This)->lpVtbl -> Remove(This,varIndex) ) 

#define IMagickImage_Compare(This,pArrayVar,pVar)	\
    ( (This)->lpVtbl -> Compare(This,pArrayVar,pVar) ) 

#define IMagickImage_Composite(This,pArrayVar,pVar)	\
    ( (This)->lpVtbl -> Composite(This,pArrayVar,pVar) ) 

#define IMagickImage_Convert(This,pArrayVar,pVar)	\
    ( (This)->lpVtbl -> Convert(This,pArrayVar,pVar) ) 

#define IMagickImage_Identify(This,pArrayVar,pVar)	\
    ( (This)->lpVtbl -> Identify(This,pArrayVar,pVar) ) 

#define IMagickImage_Mogrify(This,pArrayVar,pVar)	\
    ( (This)->lpVtbl -> Mogrify(This,pArrayVar,pVar) ) 

#define IMagickImage_Montage(This,pArrayVar,pVar)	\
    ( (This)->lpVtbl -> Montage(This,pArrayVar,pVar) ) 

#define IMagickImage_Stream(This,pArrayVar,pVar)	\
    ( (This)->lpVtbl -> Stream(This,pArrayVar,pVar) ) 

#define IMagickImage_TestHarness(This,pArrayVar,pVar)	\
    ( (This)->lpVtbl -> TestHarness(This,pArrayVar,pVar) ) 

#define IMagickImage_get__NewEnum(This,pVal)	\
    ( (This)->lpVtbl -> get__NewEnum(This,pVal) ) 

#define IMagickImage_get_Item(This,varIndex,pVal)	\
    ( (This)->lpVtbl -> get_Item(This,varIndex,pVal) ) 

#define IMagickImage_get_Messages(This,pVal)	\
    ( (This)->lpVtbl -> get_Messages(This,pVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IMagickImage_INTERFACE_DEFINED__ */



#ifndef __ImageMagickObject_LIBRARY_DEFINED__
#define __ImageMagickObject_LIBRARY_DEFINED__

/* library ImageMagickObject */
/* [uuid][helpstring][version] */ 


EXTERN_C const IID LIBID_ImageMagickObject;

EXTERN_C const CLSID CLSID_MagickImage;

#ifdef __cplusplus

class DECLSPEC_UUID("5630BE5A-3F5F-4BCA-A511-AD6A6386CAC1")
MagickImage;
#endif
#endif /* __ImageMagickObject_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  LPSAFEARRAY_UserSize(     unsigned long *, unsigned long            , LPSAFEARRAY * ); 
unsigned char * __RPC_USER  LPSAFEARRAY_UserMarshal(  unsigned long *, unsigned char *, LPSAFEARRAY * ); 
unsigned char * __RPC_USER  LPSAFEARRAY_UserUnmarshal(unsigned long *, unsigned char *, LPSAFEARRAY * ); 
void                      __RPC_USER  LPSAFEARRAY_UserFree(     unsigned long *, LPSAFEARRAY * ); 

unsigned long             __RPC_USER  VARIANT_UserSize(     unsigned long *, unsigned long            , VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserMarshal(  unsigned long *, unsigned char *, VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserUnmarshal(unsigned long *, unsigned char *, VARIANT * ); 
void                      __RPC_USER  VARIANT_UserFree(     unsigned long *, VARIANT * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


