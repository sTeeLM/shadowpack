#include "StdAfx.h"
#include "Pack.h"
#include "Resource.h"
#include "CorBuffer.h"
#include <stdio.h>
#include <stdlib.h>

void CPack::JStegErrorExit(j_common_ptr cinfo)
{
	if(cinfo->client_data != NULL && cinfo->client_data != NULL) {
		JStegParam * pParam = (JStegParam * )cinfo->client_data;
		pParam->bError = TRUE;
	}
}

int rc,wc;

void CPack::JStegReadData(j_common_ptr cinfo, JCOEF data)
{
	BYTE cor;
	if(cinfo->client_data != NULL && cinfo->client_data != NULL) {
		JStegParam * pParam = (JStegParam * )cinfo->client_data;
		if(NULL != pParam && !pParam->bError) {
			CCorBuffer * pBuffer = pParam->buffer;
			if(NULL != pBuffer) {
				cor = (BYTE)(data & 0x0F);
				if(!pBuffer->WriteCor(cor)) {
					pParam->bError = TRUE;
				}
			}
		}
	}

	if(rc < 10) {
		rc ++;
		TRACE(_T("rc : %08x\n"), data);
	}
}

JCOEF CPack::JStegWriteData(j_common_ptr cinfo, JCOEF data)
{
	if(cinfo->client_data != NULL && cinfo->client_data != NULL) {
		JStegParam * pParam = (JStegParam * )cinfo->client_data;
		BYTE b;
		INT ret = 0;
		if(NULL != pParam && !pParam->bError) {
			CCorBuffer * pBuffer = pParam->buffer;
			if(NULL != pBuffer) {
				if(pBuffer->ReadCor(&b)) {
					if(pParam->dwFormatParam == PFP_1PJ) {
						data &= ~0x01;
						data |= b&0x01;
					} else if(pParam->dwFormatParam == PFP_2PJ) {
						data &= ~0x03;
						data |= b&0x03;
					} else if(pParam->dwFormatParam == PFP_4PJ) {
						data &= ~0x0F;
						data |= b&0x0F;
					} else {
						pParam->bError = TRUE;
					}
				}
			}
		}
	}

	if(wc < 10) {
		wc ++;
		TRACE(_T("wc : %08x\n"), data);
	}
	return data;
}

BOOL CPack::RawPJReadDataInternal(CCorBuffer & sbuf, LPBYTE pRet ,size_t size,
			DWORD dwFormatParam, BOOL * bCancel, CB_SET_PROGRESS fnSetProgress)
{
	return sbuf.GetData(pRet, size, (CCorBuffer::CorFormat)dwFormatParam);
}

CPack::PackHeader * CPack::JStegReadImage(LPCTSTR szSrc, LPCTSTR szExt, CSBitmap & bmp, CString & szError, 
		BOOL * bCancel, CB_SET_PROGRESS fnSetProgress)
{
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
	struct jpeg_steg_mgr jsteg;
	FILE * infile = NULL;
	PackHeader * pRet = NULL, header;
	CCorBuffer sbuf;
	JStegParam param;
	JSAMPARRAY buffer;		/* Output row buffer */
	INT row_stride;		/* physical row width in output buffer */
	INT nProgress = 0;

	/* check ext */

	rc = wc = 0;

	memset(&cinfo, 0, sizeof(cinfo));

	memset(&param, 0, sizeof(param));
	param.bError = FALSE;
	param.buffer = &sbuf;
	cinfo.client_data = &param;

	if((infile = _tfopen(szSrc, _T("rb"))) == NULL ) {
		INT err = ferror(infile);
		szError.LoadString(IDS_ERROR_READ_FILE);
		goto error;
	}

	cinfo.err = jpeg_std_error(&jerr);
	jerr.error_exit = JStegErrorExit;

	jpeg_create_decompress(&cinfo);

	cinfo.steg = &jsteg;
	jsteg.steg_read = JStegReadData;
	jsteg.steg_write = JStegWriteData;

	jpeg_stdio_src(&cinfo, infile);

	(void) jpeg_read_header(&cinfo, TRUE);

	if(param.bError) {
		szError.LoadString(IDS_ERROR_UNSUPPORT_FILE);
		goto error;
	}

	(void) jpeg_start_decompress(&cinfo);

	if(cinfo.output_components != 3) {
		szError.LoadString(IDS_ERROR_UNSUPPORT_FILE);
		goto error;
	}

	if(!bmp.ReAllocBuffer(cinfo.output_width, cinfo.output_height)) {
		szError.LoadString(IDS_ERROR_INTERNAL);
		goto error;
	}

	/*
	img = (char*)malloc(cinfo.output_width * cinfo.output_height *
				 cinfo.output_components);
	*/

	row_stride = cinfo.output_width * cinfo.output_components;

	buffer = (*cinfo.mem->alloc_sarray)
		((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

	while (cinfo.output_scanline < cinfo.output_height) {

		(void) jpeg_read_scanlines(&cinfo, buffer, 1);

		/*memcpy(&img[(cinfo.output_scanline-1)*row_stride], buffer[0],
			row_stride);*/

		if(!bmp.SetBufferData((cinfo.output_scanline-1)*row_stride, row_stride, buffer[0])) {
			param.bError = TRUE;
			break;
		}

		if(param.bError) {
			break;
		}

		if(NULL != bCancel && *bCancel) {
			param.bError = TRUE;
			break;
		}
	}

	(void) jpeg_finish_decompress(&cinfo);

	jpeg_destroy_decompress(&cinfo);

	fclose(infile);

	infile = NULL;

	if(param.bError || (NULL != bCancel && *bCancel)) {
		if(NULL != bCancel && *bCancel) {
			szError.LoadString(IDS_ERROR_UNSUPPORT_FILE);
		} else {
			szError.LoadString(IDS_ERROR_CANCELED);
		}
		goto error;
	}

	{
		INT size = JStegGetCap(bmp.Columns() , bmp.Rows()); /* num of cors */
		TRACE(_T("JStegGetCap = %d, real size = %d\n"), size, sbuf.Size());
	}

	// test proper format from sbuf
	// is PFP_1PJ??
	if(sbuf.Size() / 8 < sizeof(PackHeader)) {
		szError.LoadString(IDS_ERROR_UNSUPPORT_FILE);
		goto error;
	}

	if(!RawPJReadDataInternal(sbuf, (LPBYTE)&header, sizeof(header), PFP_1PJ)) {
		szError.LoadString(IDS_ERROR_INTERNAL);
		goto error;
	}

	if(IsValidHeader(header)) {
		if(NULL != pRet) {
			free(pRet);
			pRet = NULL;
		}
		pRet = (PackHeader *)malloc(sizeof(header) + header.dwDataSize);
		if(NULL == pRet || !RawPJReadDataInternal(sbuf, (LPBYTE)pRet , sizeof(header) + header.dwDataSize,
			PFP_1PJ, bCancel, fnSetProgress)) {
			szError.LoadString(IDS_ERROR_INTERNAL);
			goto error;
		}
		return pRet;
	}

	// is PFP_2PJ??
	if(sbuf.Size() * 2 / 8 < sizeof(PackHeader)) {
		szError.LoadString(IDS_ERROR_UNSUPPORT_FILE);
		goto error;
	}
	if(!RawPJReadDataInternal(sbuf, (LPBYTE)&header, sizeof(header), PFP_2PJ)) {
		szError.LoadString(IDS_ERROR_INTERNAL);
		goto error;
	}

	if(IsValidHeader(header)) {
		pRet = (PackHeader *)malloc(sizeof(header) + header.dwDataSize);
		if(NULL == pRet || !RawPJReadDataInternal(sbuf, (LPBYTE)pRet , sizeof(header) + header.dwDataSize,
			PFP_2PJ, bCancel, fnSetProgress)) {
			szError.LoadString(IDS_ERROR_INTERNAL);
			goto error;
		}
		return pRet;
	}

	// is PFP_4PJ??
	if(sbuf.Size() * 4 / 8 < sizeof(PackHeader)) {
		szError.LoadString(IDS_ERROR_UNSUPPORT_FILE);
		goto error;
	}

	if(!RawPJReadDataInternal(sbuf, (LPBYTE)&header, sizeof(header), PFP_4PJ)) {
		szError.LoadString(IDS_ERROR_INTERNAL);
		goto error;
	}

	if(IsValidHeader(header)) {
		pRet = (PackHeader *)malloc(sizeof(header) + header.dwDataSize);
		if(NULL == pRet || !RawPJReadDataInternal(sbuf, (LPBYTE)pRet , sizeof(header) + header.dwDataSize,
			PFP_4PJ, bCancel, fnSetProgress)) {
			szError.LoadString(IDS_ERROR_INTERNAL);
			goto error;
		}
		return pRet;
	}

error:
	if(NULL != infile) {
		fclose(infile);
		infile = NULL;
	}
	if(NULL != pRet) {
		free(pRet);
		pRet = NULL;
	}
	return pRet;
}

BOOL CPack::JStegWriteImage(const PackHeader * data, const CSBitmap & bmp, LPCTSTR szSrc, LPCTSTR szExt,CString & szError, 
		BOOL * bCancel, CB_SET_PROGRESS fnSetProgress)
{
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;
	JSAMPROW row_pointer[1];
	int row_stride;
	FILE * outfile = NULL;
	CCorBuffer sbuf;
	JStegParam param;
	struct jpeg_steg_mgr jsteg;
	INT nProgress = 0, nNewProgress;
	LPBYTE pBuffer = NULL;

	/* check ext */

	rc = wc = 0;

	memset(&cinfo, 0, sizeof(cinfo));

	if((pBuffer = (LPBYTE)malloc(sizeof(PackHeader) + data->dwCapicity)) == NULL) {
		szError.LoadString(IDS_ERROR_INTERNAL);
		goto error;
	}

	memcpy(pBuffer, data, data->dwDataSize + sizeof(PackHeader));

	if(data->dwCapicity > data->dwDataSize) {
		PadMemory(pBuffer + sizeof(PackHeader) + data->dwDataSize, data->dwCapicity - data->dwDataSize);
	}

	/* progress 50% */
	if(!sbuf.SetData((LPBYTE)pBuffer, (data->dwCapicity + sizeof(PackHeader)), (CCorBuffer::CorFormat)data->dwFormatParam, 
		bCancel, fnSetProgress)) {
		if(bCancel && *bCancel) {
			szError.LoadString(IDS_ERROR_CANCELED);
		} else {
			szError.LoadString(IDS_ERROR_INTERNAL);
		}
		goto error;
	}
	sbuf.Rewind();

	if(NULL != pBuffer) {
		free(pBuffer);
		pBuffer = NULL;
	}

	memset(&param, 0, sizeof(param));
	param.bError = FALSE;
	param.buffer = &sbuf;
	param.dwFormatParam = data->dwFormatParam;
	cinfo.client_data = &param;

	cinfo.err = jpeg_std_error(&jerr);
	jerr.error_exit = JStegErrorExit;

	jpeg_create_compress(&cinfo);

	cinfo.steg = &jsteg;
	jsteg.steg_read = JStegReadData;
	jsteg.steg_write = JStegWriteData;

	if((outfile = _tfopen(szSrc, _T("wb"))) == NULL ) {
		INT err = ferror(outfile);
		szError.LoadString(IDS_ERROR_READ_FILE);
		goto error;
	}
	jpeg_stdio_dest(&cinfo, outfile);

	cinfo.image_width = bmp.Columns(); 	/* image width and height, in pixels */
	cinfo.image_height = bmp.Rows();
	cinfo.input_components = 3;         /* # of color components per pixel */
	cinfo.in_color_space = JCS_RGB; 	/* colorspace of input image */

	jpeg_set_defaults(&cinfo);

	jpeg_set_quality(&cinfo, 100, TRUE /* limit to baseline-JPEG values */);

	jpeg_start_compress(&cinfo, TRUE);

	row_stride = bmp.Columns() * 3;	/* JSAMPLEs per row in image_buffer */

	while (cinfo.next_scanline < cinfo.image_height) {
		/* row_pointer[0] = (JSAMPROW)& img[cinfo.next_scanline * row_stride]; */
		row_pointer[0] = (JSAMPROW)bmp.GetBufferPointer(cinfo.next_scanline * row_stride);
		if(NULL == row_pointer[0]) {
			param.bError = TRUE;
			break;
		}

		(void) jpeg_write_scanlines(&cinfo, row_pointer, 1);

		if(NULL != bCancel && *bCancel) {
			param.bError = TRUE;
			break;
		}

		if(fnSetProgress) {
			nNewProgress = 50 + cinfo.next_scanline * 50 / cinfo.image_height;
			if(nNewProgress != nProgress) {
				fnSetProgress(50 + cinfo.next_scanline * 50 / cinfo.image_height);
				nProgress = nNewProgress;
			}
		}
	}

	jpeg_finish_compress(&cinfo);

	jpeg_destroy_compress(&cinfo);

	if(NULL != outfile) {
		fclose(outfile);
		outfile = NULL;
	}

	if(param.bError) {
		if(NULL != bCancel && *bCancel) {
			szError.LoadString(IDS_ERROR_CANCELED);
		} else {
			szError.LoadString(IDS_ERROR_INTERNAL);
		}
		goto error;
	}

	if(!param.buffer->Eof()) {
		szError.LoadString(IDS_ERROR_INTERNAL);
		goto error;
	}

	if(fnSetProgress) {
			fnSetProgress(100);
	}

	return TRUE;

error:
	if(NULL != outfile) {
		fclose(outfile);
		outfile = NULL;
	}
	if(NULL != pBuffer) {
		free(pBuffer);
		pBuffer = NULL;
	}
	return FALSE;
}

BOOL CPack::JStegCanSetFormat( DWORD dwParam, const PackHeader * data, const CSBitmap & bmp)
{
	INT size, cap;

	size = JStegGetCap(bmp.Columns() , bmp.Rows()); /* num of cors */

	if(dwParam == PFP_1PJ) {
		cap = size / 8 - sizeof(PackHeader);
	} else if(dwParam == PFP_2PJ) {
		cap = size * 2 / 8 - sizeof(PackHeader);
	} else if(dwParam == PFP_4PJ) {
		cap = size * 4 / 8 - sizeof(PackHeader);		
	}

	return cap >= data->dwDataSize;
}

BOOL CPack::JStegSetFormat(DWORD dwParam, PackHeader * data, const CSBitmap & bmp)
{
	INT size;

	size = JStegGetCap(bmp.Columns() , bmp.Rows()); /* num of cors */
	if(dwParam != data->dwFormatParam || data->dwFormat != PF_JSTEG) {
		if(JStegCanSetFormat(dwParam, data, bmp)) {
			data->dwFormat = PF_JSTEG;
			data->dwFormatParam = dwParam;
			if(dwParam == PFP_1PJ) {
				data->dwCapicity = size / 8 - sizeof(PackHeader);
			} else if(dwParam == PFP_2PJ) {
				data->dwCapicity = size * 2 / 8 - sizeof(PackHeader);
			} else if(dwParam == PFP_4PJ) {
				data->dwCapicity = size * 4 / 8 - sizeof(PackHeader);	
			}
			return TRUE;
		} else {
			return FALSE;
		}
	} else {
		return TRUE;
	}
	return FALSE;
}

void CPack::JStegGetSaveFilter(CString & szFilter)
{
	szFilter = _T("JPEG (*.jpg; jpeg)|*.jpg; jpeg||");
}

void CPack::JStegSaveDefaultExt(CString & szExt)
{
	szExt = _T("JPEG");
}

INT CPack::JStegGetCap(INT x, INT y)
{
	INT m = x / 8; // scale up to 8
	INT n = y / 16;
	INT s  = 0;
	INT m1 = 0;
	if(x % 8 != 0) {
		m ++;
	}

	if(y % 16 != 0) {
		if(y % 16 > 8) {
			n ++;
			m *= 3;
			if((m & 0x1) != 0) {
				m++;
			}
			s = m * 64 * n;
		} else {
			m1 = m*3;
			if((m1 & 0x1) != 0) {
				m1++;
			}
			s = m1 * 64 * n;
			m *= 2;
			m++;
			s += m * 64;
		}
	} else {
			m *= 3;
			if((m & 0x1) != 0) {
				m++;
			}
			s = m * 64 * n;
	}

	return s;
}