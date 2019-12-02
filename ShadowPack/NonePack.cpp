#include "StdAfx.h"
#include "Pack.h"
#include "Resource.h"

CPack::PackHeader * CPack::DefaultReadImage(LPCTSTR szSrc, LPCTSTR szExt, CSBitmap & bmp, CPackErrors & Error, 
		BOOL * bCancel, CB_SET_PROGRESS fnSetProgress)
{

	Magick::Image image;
	PackHeader * ret = NULL;

	// read bmp
	// load image
	try {
		// Convert a TCHAR string to a LPCSTR
		CT2CA strSrc (szSrc, CP_UTF8);
		// construct a std::string using the LPCSTR input
		std::string strStd (strSrc);
		// alloc memory
		TRACE("image file size is %s\n", strStd.c_str());
		image.read(strStd);
		bmp.FromImage(image);
	}
	catch (const Magick::Error & err ) {
		 Error.SetError(err);
		goto error;
    }

	if(image.columns() * image.rows() < sizeof(PackHeader)) {
		Error.SetError(CPackErrors::PE_UNSUPPORT_PACK);
		goto error;
	}

	ret = (PackHeader *)malloc(sizeof(PackHeader));

	if(NULL != ret) {
		ret->dwCapicity = image.columns() * image.rows()  - sizeof(PackHeader);
		ret->dwCount    = 0;
		ret->dwDataSize = 0;
		ret->dwSignature = PACKAGE_HEADER_SIG;
		ret->dwEncryptType = EM_NONE;
		ret->dwFormat    = PF_RAWPP;
		ret->dwFormatParam = 1;
	} else {
		Error.SetError(CPackErrors::PE_INTERNAL);
		return NULL;
	}

	return ret;
error:
	if(NULL != ret) {
		free(ret);
		ret = NULL;
	}
	return ret;
}
