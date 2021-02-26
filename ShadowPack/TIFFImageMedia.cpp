#include "StdAfx.h"
#include "TIFFImageMedia.h"

#include "tif_dir.h"
#include "tiffiop.h"

CTIFFImageMedia::CTIFFImageMedia(void):
m_nBlockCount(0),
m_pTiff(NULL),
m_pScanline(0)
{
}

CTIFFImageMedia::~CTIFFImageMedia(void)
{
}


LPCTSTR CTIFFImageMedia::m_szFilter = _T("TIFF Files (*.tif;*tiff)|*.tif;*.tiff|");
LPCTSTR CTIFFImageMedia::m_szExt = _T("TIF");


CString CTIFFImageMedia::m_strLastError = _T("");
void CTIFFImageMedia::ErrorHandler(const char *module, const char *fmt, va_list ap)
{
	CHAR Buffer[1024] = {0};
	vsprintf_s(Buffer, sizeof(Buffer) - 1, fmt, ap);
	Buffer[sizeof(Buffer) - 1] = 0;

	CA2CT szMessage(Buffer);
	TRACE(_T("CTIFFImageMedia::ErrorHandler %s\n"), szMessage);
	m_strLastError = szMessage;
}

BOOL CTIFFImageMedia::ReadBlock(CMediaBlock * pBlock, ULONGLONG nOffset, ULONGLONG nBlockCount, CPackErrors & Error)
{

	CPixelBlock * pPixel = (CPixelBlock *)pBlock;
	TRACE(_T("CTIFFImageMedia::ReadBlock: %lld, %lld\n"), nOffset, nBlockCount);

	if(!m_pTiff)
		return FALSE;

	if(nOffset + nBlockCount > GetBlockCount()) {
		Error.SetError(CPackErrors::PE_INTERNAL);
		return FALSE;
	}

	if(pPixel->GetCapicity() < nBlockCount) {
		if(!pPixel->ReAlloc(nBlockCount, Error)) {
			return FALSE;
		}
	}

	// 定位是

//	TIFFReadScanline
//	TIFFScanlineSize
	return FALSE;
}

BOOL CTIFFImageMedia::WriteBlock(const CMediaBlock * pBlock, ULONGLONG nOffset, ULONGLONG nBlockCount, CPackErrors & Error)
{
	return FALSE;
}

ULONGLONG CTIFFImageMedia::GetBlockCount()
{
	return m_nBlockCount;
}

BOOL CTIFFImageMedia::ReopenFileToRead(CPackErrors &Error)
{
	if(m_pTiff) {
		TIFFFlush(m_pTiff);
		TIFFClose(m_pTiff);
		m_pTiff = NULL;
	}
	#ifdef _UNICODE
		if((m_pTiff = TIFFOpenW(GetMediaFilePath(), "r")) != NULL) {
	#else
		if((m_pTiff = TIFFOpen(GetMediaFilePath(), "r")) != NULL) {
	#endif
		Error.SetError(CPackErrors::PE_IO, GetMediaFilePath(), m_strLastError);
		return FALSE;
	}
	return TRUE;
}



// Note that unlike the stdio library TIFF image files may not be opened for both reading and writing; 
// there is no support for altering the contents of a TIFF file.
BOOL CTIFFImageMedia::OpenFile(CMedia::media_mode_t mode, LPCTSTR szPath, BOOL & bCancel, CPackErrors & Error, CProgress & Progress)
{

	TIFFSetErrorHandler(ErrorHandler);
	TIFFSetWarningHandler(ErrorHandler);

	if(mode == MODE_READ) {

	#ifdef _UNICODE
		m_pTiff = TIFFOpenW(szPath, "r");
	#else
		m_pTiff = TIFFOpen(szPath, "r");
	#endif

		if(!m_pTiff) {
			Error.SetError(CPackErrors::PE_IO, szPath, m_strLastError);
			goto err;
		}


		if(!LoadTiffInfo(Error)) {
			goto err;
		}

	} else {
	#ifdef _UNICODE
		m_pTiff = TIFFOpenW(szPath, "w");
	#else
		m_pTiff = TIFFOpen(szPath, "w");
	#endif

		if(!m_pTiff) {
			Error.SetError(CPackErrors::PE_IO, szPath, m_strLastError);
			goto err;
		}
	}


	if(!AllocScanline()) {
		Error.SetError(CPackErrors::PE_NOMEM);
		goto err;
	}

	return TRUE;

err:
	if(m_pTiff) {
		TIFFClose(m_pTiff);
		m_pTiff = NULL;
	}
	FreeScanLine();
	FreeTiffInfo();
	return FALSE;
}

BOOL CTIFFImageMedia::SaveFile(BOOL &bCancel, CPackErrors &Error, CProgress &Progress)
{
	return TRUE;
}

void CTIFFImageMedia::CloseFile()
{
	if(m_pTiff) {
		TIFFFlush(m_pTiff);
		TIFFClose(m_pTiff);
		m_pTiff = NULL;
	}
	FreeScanLine();
	FreeTiffInfo();
}

CMedia * CTIFFImageMedia::CloneMedia(CPackErrors & Error)
{
	CTIFFImageMedia * pMedia = new(std::nothrow) CTIFFImageMedia();
	if(pMedia) {
		pMedia->CloneCore(this);
		if(!CloneTiffInfo(pMedia)) {
			delete pMedia;
			pMedia = NULL;
		}
	}
	if(!pMedia) {
		Error.SetError(CPackErrors::PE_NOMEM);
	}
	return pMedia;
}

LPCTSTR CTIFFImageMedia::GetFilter()
{
	return m_szFilter; 
}

LPCTSTR CTIFFImageMedia::GetDefaultExt()
{
	return m_szExt;
}

BOOL CTIFFImageMedia::TestExt(LPCTSTR szExt)
{
	return lstrcmpi(szExt, _T("TIFF")) == 0 || lstrcmpi(szExt, _T("TIF")) == 0 ;
}

LPCTSTR CTIFFImageMedia::GetExtFilter()
{
	return m_szFilter;
}

CMedia * CTIFFImageMedia::Factory()
{
	return new(std::nothrow) CTIFFImageMedia();
}

BOOL CTIFFImageMedia::AllocScanline()
{
	return FALSE;
}

void CTIFFImageMedia::FreeScanLine()
{
	
}

BOOL CTIFFImageMedia::LoadTiffInfo(CPackErrors & Error)
{
	uint16 nDirCnt = TIFFNumberOfDirectories(m_pTiff);

	TRACE(_T("Directory Count is %d\n"), nDirCnt);

	nDirCnt = 0;
	do {
		CTiffDirEntry * pDirEntry = CTiffDirEntry::LoadDirectory(m_pTiff, Error);
		if(!pDirEntry) {
			goto err;
		}
	} while(TIFFReadDirectory(m_pTiff));

	return TRUE;

err:
	FreeTiffInfo();
	return FALSE;
}


BOOL CTIFFImageMedia::CloneTiffInfo(CTIFFImageMedia * pMediaTo)
{
	return FALSE;
}

void CTIFFImageMedia::FreeTiffInfo()
{

}

BOOL CTIFFImageMedia::CTiffDirEntry::SaveDirectory(TIFF * tiff)
{
	return TRUE;
}

void CTIFFImageMedia::CTiffDirEntry::FreeDirectory()
{

}

#define TIFF_ADD_STAND_TAG_ENTRY(tag) \
		Tag.m_nTag = tag; \
		Tag.m_pField = TIFFFieldWithTag(tif, Tag.m_nTag); \
		if(Tag.m_pField) {\
			pEntry->m_TagInfo.Add(Tag); \
		} else {\
			Error.SetError(CPackErrors::PE_UNSUPPORT_MEDIA); \
			goto err; \
		}

#define TIFF_ADD_EXIF_TAG_ENTRY(tag) \
		Tag.m_nTag = tag; \
		Tag.m_pField = TIFFFieldWithTag(tif, Tag.m_nTag); \
		if(Tag.m_pField) {\
			pEntry->m_ExifTagInfo.Add(Tag); \
		} else {\
			Error.SetError(CPackErrors::PE_UNSUPPORT_MEDIA); \
			goto err; \
		}

CTIFFImageMedia::CTiffDirEntry * CTIFFImageMedia::CTiffDirEntry::LoadDirectory(TIFF * tif, CPackErrors & Error)
{
	CTiffDirEntry * pEntry = new(std::nothrow) CTiffDirEntry();
	TIFFDirectory *td = &tif->tif_dir;
	uint16 nCount;
	CTiffTagEntry Tag;
	uint64 exif_offset;

	if(!pEntry) {
		Error.SetError(CPackErrors::PE_NOMEM);
		return NULL;
	}

	if (TIFFFieldSet(tif,FIELD_IMAGEDIMENSIONS)) {
		TIFF_ADD_STAND_TAG_ENTRY(TIFFTAG_IMAGEWIDTH)
		TIFF_ADD_STAND_TAG_ENTRY(TIFFTAG_IMAGELENGTH)
	}
	if (TIFFFieldSet(tif,FIELD_TILEDIMENSIONS)) {
		TIFF_ADD_STAND_TAG_ENTRY(TIFFTAG_TILEWIDTH)
		TIFF_ADD_STAND_TAG_ENTRY(TIFFTAG_TILELENGTH)
	}
	if (TIFFFieldSet(tif,FIELD_RESOLUTION)) {
		TIFF_ADD_STAND_TAG_ENTRY(TIFFTAG_XRESOLUTION)
		TIFF_ADD_STAND_TAG_ENTRY(TIFFTAG_YRESOLUTION)
	}

	if (TIFFFieldSet(tif,FIELD_POSITION)) {
		TIFF_ADD_STAND_TAG_ENTRY(TIFFTAG_XPOSITION)
		TIFF_ADD_STAND_TAG_ENTRY(TIFFTAG_YPOSITION)
	}

	if (TIFFFieldSet(tif,FIELD_SUBFILETYPE)) {
		TIFF_ADD_STAND_TAG_ENTRY(TIFFTAG_SUBFILETYPE)
	}

	if (TIFFFieldSet(tif,FIELD_BITSPERSAMPLE)) {
		TIFF_ADD_STAND_TAG_ENTRY(TIFFTAG_BITSPERSAMPLE)
	}

	if (TIFFFieldSet(tif,FIELD_COMPRESSION)) {
		TIFF_ADD_STAND_TAG_ENTRY(TIFFTAG_COMPRESSION)
	}

	if (TIFFFieldSet(tif,FIELD_PHOTOMETRIC)) {
		TIFF_ADD_STAND_TAG_ENTRY(TIFFTAG_PHOTOMETRIC)
	}

	if (TIFFFieldSet(tif,FIELD_THRESHHOLDING)) {
		TIFF_ADD_STAND_TAG_ENTRY(TIFFTAG_THRESHHOLDING)
	}

	if (TIFFFieldSet(tif,FIELD_FILLORDER)) {
		TIFF_ADD_STAND_TAG_ENTRY(TIFFTAG_FILLORDER)
	}

	if (TIFFFieldSet(tif,FIELD_ORIENTATION)) {
		TIFF_ADD_STAND_TAG_ENTRY(TIFFTAG_ORIENTATION)
	}

	if (TIFFFieldSet(tif,FIELD_SAMPLESPERPIXEL)) {
		TIFF_ADD_STAND_TAG_ENTRY(TIFFTAG_SAMPLESPERPIXEL)
	}

	if (TIFFFieldSet(tif,FIELD_ROWSPERSTRIP)) {
		TIFF_ADD_STAND_TAG_ENTRY(TIFFTAG_ROWSPERSTRIP)
	}

	if (TIFFFieldSet(tif,FIELD_MINSAMPLEVALUE)) {
		TIFF_ADD_STAND_TAG_ENTRY(TIFFTAG_MINSAMPLEVALUE)
	}

	if (TIFFFieldSet(tif,FIELD_MAXSAMPLEVALUE)) {
		TIFF_ADD_STAND_TAG_ENTRY(TIFFTAG_MAXSAMPLEVALUE)
	}

	if (TIFFFieldSet(tif,FIELD_PLANARCONFIG)) {
		TIFF_ADD_STAND_TAG_ENTRY(TIFFTAG_PLANARCONFIG)
	}

	if (TIFFFieldSet(tif,FIELD_PAGENUMBER)) {
		TIFF_ADD_STAND_TAG_ENTRY(TIFFTAG_PAGENUMBER)
	}

	if (TIFFFieldSet(tif,FIELD_STRIPBYTECOUNTS)) {
		TIFF_ADD_STAND_TAG_ENTRY(TIFFTAG_STRIPBYTECOUNTS)
		TIFF_ADD_STAND_TAG_ENTRY(TIFFTAG_TILEBYTECOUNTS)
	}
	
	if (TIFFFieldSet(tif,FIELD_STRIPOFFSETS)) {
		TIFF_ADD_STAND_TAG_ENTRY(TIFFTAG_STRIPOFFSETS)
		TIFF_ADD_STAND_TAG_ENTRY(TIFFTAG_TILEOFFSETS)
	}

	if (TIFFFieldSet(tif,FIELD_COLORMAP)) {
		TIFF_ADD_STAND_TAG_ENTRY(TIFFTAG_COLORMAP)
	}

	if (TIFFFieldSet(tif,FIELD_EXTRASAMPLES) && td->td_extrasamples) {
		TIFF_ADD_STAND_TAG_ENTRY(TIFFTAG_EXTRASAMPLES)
	}

	if (TIFFFieldSet(tif,FIELD_SAMPLEFORMAT)) {
		TIFF_ADD_STAND_TAG_ENTRY(TIFFTAG_SAMPLEFORMAT)
	}
	
	if (TIFFFieldSet(tif,FIELD_SMINSAMPLEVALUE)) {
		TIFF_ADD_STAND_TAG_ENTRY(TIFFTAG_SMINSAMPLEVALUE)
	}

	if (TIFFFieldSet(tif,FIELD_SMAXSAMPLEVALUE)) {
		TIFF_ADD_STAND_TAG_ENTRY(TIFFTAG_SMAXSAMPLEVALUE)
	}

	if (TIFFFieldSet(tif,FIELD_IMAGEDEPTH)) {
		TIFF_ADD_STAND_TAG_ENTRY(TIFFTAG_IMAGEDEPTH)
	}

	if (TIFFFieldSet(tif,FIELD_TILEDEPTH)) {
		TIFF_ADD_STAND_TAG_ENTRY(TIFFTAG_TILEDEPTH)
	}
	

	if (TIFFFieldSet(tif,FIELD_HALFTONEHINTS)) {
		TIFF_ADD_STAND_TAG_ENTRY(TIFFTAG_HALFTONEHINTS)
	}

	if (TIFFFieldSet(tif,FIELD_YCBCRSUBSAMPLING)) {
		TIFF_ADD_STAND_TAG_ENTRY(TIFFTAG_YCBCRSUBSAMPLING)
	}

	if (TIFFFieldSet(tif,FIELD_YCBCRPOSITIONING)) {
		TIFF_ADD_STAND_TAG_ENTRY(TIFFTAG_YCBCRPOSITIONING)
	}

	if (TIFFFieldSet(tif,FIELD_REFBLACKWHITE)) {
		TIFF_ADD_STAND_TAG_ENTRY(TIFFTAG_REFERENCEBLACKWHITE)
	}

	if (TIFFFieldSet(tif,FIELD_TRANSFERFUNCTION)) {
		TIFF_ADD_STAND_TAG_ENTRY(TIFFTAG_TRANSFERFUNCTION)
	}

	if (TIFFFieldSet(tif,FIELD_INKNAMES)) {
		TIFF_ADD_STAND_TAG_ENTRY(TIFFTAG_INKNAMES)
	}

	if (TIFFFieldSet(tif,FIELD_SUBIFD)) {
		// not support subifd ()
		Error.SetError(CPackErrors::PE_UNSUPPORT_MEDIA);
		goto err;
	}

	// 装载 custom tag
	nCount = (short) TIFFGetTagListCount(tif);
	for(int i = 0; i < nCount; i++) {
		uint32 nTag = TIFFGetTagListEntry(tif, i);
		TIFF_ADD_STAND_TAG_ENTRY(nTag)
	}

	if(!pEntry->LoadTagValue(tif, pEntry->m_TagInfo, Error)) {
		goto err;
	}

	// 如果有exif，装载exif
	if(TIFFGetField(tif, TIFFTAG_EXIFIFD,&exif_offset)) {
		if(TIFFReadEXIFDirectory(tif,exif_offset)) {
			nCount = (short) TIFFGetTagListCount(tif);
			for(int i = 0; i < nCount; i++) {
				uint32 nTag = TIFFGetTagListEntry(tif, i);
				TIFF_ADD_EXIF_TAG_ENTRY(nTag)
			}
		}

	}

	if(!pEntry->LoadTagValue(tif, pEntry->m_TagInfo, Error)) {
		goto err;
	}


	return pEntry;
err:
	delete pEntry;
	pEntry = NULL;
	return NULL;
}

BOOL CTIFFImageMedia::CTiffDirEntry::LoadTagValue(TIFF * tif, CArray<CTiffTagEntry, CTiffTagEntry> & aTags, CPackErrors & Error)
{
	
	UINT nCount = aTags.GetCount();
	TIFFDirectory *td = &tif->tif_dir;
	uint16 dotrange[2];
	uint64 data;
	uint32 buff_size;
	for(UINT i = 0 ; i < nCount ; i ++) {
		const TIFFField *fip = aTags[i].m_pField;
		uint32 value_count;
		void *raw_data = NULL;
		aTags[i].m_pData = aTags[i].m_pRawData;
		aTags[i].m_bAlloc = FALSE;
		aTags[i].m_nCount = 1;

	switch (aTags[i].m_nTag) {
		case TIFFTAG_SUBFILETYPE:
		case TIFFTAG_IMAGEWIDTH:
		case TIFFTAG_IMAGELENGTH:
		case TIFFTAG_BITSPERSAMPLE:
		case TIFFTAG_COMPRESSION:
		case TIFFTAG_PHOTOMETRIC:
		case TIFFTAG_THRESHHOLDING:
		case TIFFTAG_FILLORDER:
		case TIFFTAG_ORIENTATION:
		case TIFFTAG_SAMPLESPERPIXEL:
		case TIFFTAG_ROWSPERSTRIP:
		case TIFFTAG_MINSAMPLEVALUE:
		case TIFFTAG_MAXSAMPLEVALUE:
		case TIFFTAG_XRESOLUTION:
		case TIFFTAG_YRESOLUTION:
		case TIFFTAG_PLANARCONFIG:
		case TIFFTAG_XPOSITION:
		case TIFFTAG_YPOSITION:
		case TIFFTAG_RESOLUTIONUNIT:
		case TIFFTAG_PAGENUMBER:
		case TIFFTAG_HALFTONEHINTS:
		case TIFFTAG_YCBCRPOSITIONING:
		case TIFFTAG_MATTEING:
		case TIFFTAG_TILEWIDTH:
		case TIFFTAG_TILELENGTH:
		case TIFFTAG_TILEDEPTH:
		case TIFFTAG_DATATYPE:
		case TIFFTAG_SAMPLEFORMAT:
		case TIFFTAG_IMAGEDEPTH:
			if(!TIFFGetField(tif, aTags[i].m_nTag, aTags[i].m_pData)) {
				Error.SetError(CPackErrors::PE_INTERNAL);
				goto err;
			}
			if(aTags[i].m_nTag == TIFFTAG_SAMPLESPERPIXEL && *((uint16*)aTags[i].m_pData) > 4) {
				Error.SetError(CPackErrors::PE_UNSUPPORT_MEDIA);
				goto err;
			}
		case TIFFTAG_SMINSAMPLEVALUE:
		case TIFFTAG_SMAXSAMPLEVALUE:
			uint16 nSamplesPerPixel;
			if(!TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &nSamplesPerPixel)) {
				Error.SetError(CPackErrors::PE_INTERNAL);
				goto err;
			}
			if(tif->tif_flags & TIFF_PERSAMPLE) {
				// aTags[i].m_pData is * double contain td_samplesperpixel's double
				if(!TIFFGetField(tif, aTags[i].m_nTag, &raw_data)) {
					Error.SetError(CPackErrors::PE_INTERNAL);
					goto err;
				}
				memcpy(aTags[i].m_pData, raw_data, nSamplesPerPixel * sizeof(double));
			} else {
				// aTags[i].m_pData is double
				if(!TIFFGetField(tif, aTags[i].m_nTag, aTags[i].m_pData)) {
					goto err;
				}
			}
			break;
		case TIFFTAG_COLORMAP:
			*va_arg(ap, uint16**) = td->td_colormap[0];
			*va_arg(ap, uint16**) = td->td_colormap[1];
			*va_arg(ap, uint16**) = td->td_colormap[2];
			break;
		case TIFFTAG_STRIPOFFSETS:
		case TIFFTAG_TILEOFFSETS:
			_TIFFFillStriles( tif );
			*va_arg(ap, uint64**) = td->td_stripoffset;
			break;
		case TIFFTAG_STRIPBYTECOUNTS:
		case TIFFTAG_TILEBYTECOUNTS:
			_TIFFFillStriles( tif );
			*va_arg(ap, uint64**) = td->td_stripbytecount;
			break;
		case TIFFTAG_EXTRASAMPLES:
			*va_arg(ap, uint16*) = td->td_extrasamples;
			*va_arg(ap, uint16**) = td->td_sampleinfo;
			break;
		case TIFFTAG_SUBIFD:
			*va_arg(ap, uint16*) = td->td_nsubifd;
			*va_arg(ap, uint64**) = td->td_subifd;
			break;

		case TIFFTAG_YCBCRSUBSAMPLING:
			*va_arg(ap, uint16*) = td->td_ycbcrsubsampling[0];
			*va_arg(ap, uint16*) = td->td_ycbcrsubsampling[1];
			break;
		case TIFFTAG_TRANSFERFUNCTION:
			*va_arg(ap, uint16**) = td->td_transferfunction[0];
			if (td->td_samplesperpixel - td->td_extrasamples > 1) {
				*va_arg(ap, uint16**) = td->td_transferfunction[1];
				*va_arg(ap, uint16**) = td->td_transferfunction[2];
			}
			break;
		case TIFFTAG_REFERENCEBLACKWHITE:
			*va_arg(ap, float**) = td->td_refblackwhite;
			break;
		case TIFFTAG_INKNAMES:
			*va_arg(ap, char**) = td->td_inknames;
			break;
		default: {

			if(fip->field_passcount) {
				if (fip->field_readcount == TIFF_VARIABLE ) {
					if(TIFFGetField(tif, aTags[i].m_nTag, &value_count, &raw_data) != 1)
						goto err;
				} else if (fip->field_readcount == TIFF_VARIABLE2 ) {
					uint16 small_value_count;
					if(TIFFGetField(tif, aTags[i].m_nTag, &small_value_count, &raw_data) != 1)
						goto err;
					value_count = small_value_count;
				} else {
					goto err;
				} 
			} else {
				if (fip->field_readcount == TIFF_VARIABLE
					|| fip->field_readcount == TIFF_VARIABLE2)
					value_count = 1;
				else if (fip->field_readcount == TIFF_SPP)
					value_count = td->td_samplesperpixel;
				else
					value_count = fip->field_readcount;
				if (fip->field_tag == TIFFTAG_DOTRANGE
					&& strcmp(fip->field_name,"DotRange") == 0) {
					/* TODO: This is an evil exception and should not have been
					   handled this way ... likely best if we move it into
					   the directory structure with an explicit field in 
					   libtiff 4.1 and assign it a FIELD_ value */
					raw_data = dotrange;
					TIFFGetField(tif, aTags[i].m_nTag, dotrange + 0, dotrange + 1);
				} else if (fip->field_type == TIFF_ASCII
					   || fip->field_readcount == TIFF_VARIABLE
					   || fip->field_readcount == TIFF_VARIABLE2
					   || fip->field_readcount == TIFF_SPP
					   || value_count > 1) {
					if(TIFFGetField(tif, aTags[i].m_nTag, &raw_data) != 1)
						goto err;
				} else {
					raw_data = &data;
					if(TIFFGetField(tif, aTags[i].m_nTag, raw_data) != 1) {
						goto err;
					}
				}
			}
			/*
			aTags[i].m_nValueCount = value_count;
			buff_size = _TIFFDataSize(fip->field_type) * value_count;
			if(buff_size > sizeof(aTags[i].m_pRawData)) {
				aTags[i].m_pData = malloc(buff_size);
				if(!aTags[i].m_pData) {
					goto err;
				}
				aTags[i].m_bAlloc = TRUE;
			} else {
				aTags[i].m_pData = aTags[i].m_pRawData;
				aTags[i].m_bAlloc = FALSE;
			}
			memcpy(aTags[i].m_pData, raw_data, buff_size);
			*/
			TRACE("custom tag %s ,type %d ,value cnt %d , field %d, value %p\n", 
				fip->field_name, fip->field_type, value_count, fip->field_bit, raw_data);

		}
	}
	return TRUE;
err:
	return FALSE;
}