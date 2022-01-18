#include "pch.h"
#include "ApeFileMedia.h"
#include "resource.h"

CApeFileMedia::CApeFileMedia() :
	m_nMacFlags(0),
	m_nTotalBlocks(0),
	m_nCompressLevel(0)
{
}

CApeFileMedia::~CApeFileMedia()
{
}
/*
// success
#ifndef ERROR_SUCCESS
#define ERROR_SUCCESS                                   0
#endif

// file and i/o errors (1000's)
#define ERROR_IO_READ                                   1000
#define ERROR_IO_WRITE                                  1001
#define ERROR_INVALID_INPUT_FILE                        1002
#define ERROR_INVALID_OUTPUT_FILE                       1003
#define ERROR_INPUT_FILE_TOO_LARGE                      1004
#define ERROR_INPUT_FILE_UNSUPPORTED_BIT_DEPTH          1005
#define ERROR_INPUT_FILE_UNSUPPORTED_SAMPLE_RATE        1006
#define ERROR_INPUT_FILE_UNSUPPORTED_CHANNEL_COUNT      1007
#define ERROR_INPUT_FILE_TOO_SMALL                      1008
#define ERROR_INVALID_CHECKSUM                          1009
#define ERROR_DECOMPRESSING_FRAME                       1010
#define ERROR_INITIALIZING_UNMAC                        1011
#define ERROR_INVALID_FUNCTION_PARAMETER                1012
#define ERROR_UNSUPPORTED_FILE_TYPE                     1013
#define ERROR_UNSUPPORTED_FILE_VERSION                  1014
#define ERROR_OPENING_FILE_IN_USE                       1015

// memory errors (2000's)
#define ERROR_INSUFFICIENT_MEMORY                       2000

// dll errors (3000's)
#define ERROR_LOADING_APE_DLL                           3000
#define ERROR_LOADING_APE_INFO_DLL                      3001
#define ERROR_LOADING_UNMAC_DLL                         3002

// general and misc errors
#define ERROR_USER_STOPPED_PROCESSING                   4000
#define ERROR_SKIPPED                                   4001

// programmer errors
#define ERROR_BAD_PARAMETER                             5000

// IAPECompress errors
#define ERROR_APE_COMPRESS_TOO_MUCH_DATA                6000

// unknown error
#define ERROR_UNDEFINED                                -1
*/
void CApeFileMedia::LoadErrorString()
{
	m_ErrorString.RemoveAll();
	m_ErrorString.SetAt(ERROR_SUCCESS, CString(_T("ERROR_SUCCESS")));
	m_ErrorString.SetAt(ERROR_IO_READ, CString(_T("ERROR_IO_READ")));
	m_ErrorString.SetAt(ERROR_IO_WRITE, CString(_T("ERROR_IO_WRITE")));
	m_ErrorString.SetAt(ERROR_INVALID_INPUT_FILE, CString(_T("ERROR_INVALID_INPUT_FILE")));
	m_ErrorString.SetAt(ERROR_INVALID_OUTPUT_FILE, CString(_T("ERROR_INVALID_OUTPUT_FILE")));
	m_ErrorString.SetAt(ERROR_INPUT_FILE_TOO_LARGE, CString(_T("ERROR_INPUT_FILE_TOO_LARGE")));
	m_ErrorString.SetAt(ERROR_INPUT_FILE_UNSUPPORTED_BIT_DEPTH, CString(_T("ERROR_INPUT_FILE_UNSUPPORTED_BIT_DEPTH")));
	m_ErrorString.SetAt(ERROR_INPUT_FILE_UNSUPPORTED_SAMPLE_RATE, CString(_T("ERROR_INPUT_FILE_UNSUPPORTED_SAMPLE_RATE")));
	m_ErrorString.SetAt(ERROR_INPUT_FILE_UNSUPPORTED_CHANNEL_COUNT, CString(_T("ERROR_INPUT_FILE_UNSUPPORTED_CHANNEL_COUNT")));
	m_ErrorString.SetAt(ERROR_INPUT_FILE_TOO_SMALL, CString(_T("ERROR_INPUT_FILE_TOO_SMALL")));
	m_ErrorString.SetAt(ERROR_INVALID_CHECKSUM, CString(_T("ERROR_INVALID_CHECKSUM")));
	m_ErrorString.SetAt(ERROR_DECOMPRESSING_FRAME, CString(_T("ERROR_DECOMPRESSING_FRAME")));
	m_ErrorString.SetAt(ERROR_INITIALIZING_UNMAC, CString(_T("ERROR_INITIALIZING_UNMAC")));
	m_ErrorString.SetAt(ERROR_INVALID_FUNCTION_PARAMETER, CString(_T("ERROR_INVALID_FUNCTION_PARAMETER")));
	m_ErrorString.SetAt(ERROR_UNSUPPORTED_FILE_TYPE, CString(_T("ERROR_UNSUPPORTED_FILE_TYPE")));
	m_ErrorString.SetAt(ERROR_UNSUPPORTED_FILE_VERSION, CString(_T("ERROR_UNSUPPORTED_FILE_VERSION")));
	m_ErrorString.SetAt(ERROR_OPENING_FILE_IN_USE, CString(_T("ERROR_OPENING_FILE_IN_USE")));
	m_ErrorString.SetAt(ERROR_INSUFFICIENT_MEMORY, CString(_T("ERROR_INSUFFICIENT_MEMORY")));
	m_ErrorString.SetAt(ERROR_LOADING_APE_DLL, CString(_T("ERROR_LOADING_APE_DLL")));
	m_ErrorString.SetAt(ERROR_LOADING_APE_INFO_DLL, CString(_T("ERROR_LOADING_APE_INFO_DLL")));
	m_ErrorString.SetAt(ERROR_LOADING_UNMAC_DLL, CString(_T("ERROR_LOADING_UNMAC_DLL")));
	m_ErrorString.SetAt(ERROR_USER_STOPPED_PROCESSING, CString(_T("ERROR_USER_STOPPED_PROCESSING")));
	m_ErrorString.SetAt(ERROR_SKIPPED, CString(_T("ERROR_SKIPPED")));
	m_ErrorString.SetAt(ERROR_BAD_PARAMETER, CString(_T("ERROR_BAD_PARAMETER")));
	m_ErrorString.SetAt(ERROR_APE_COMPRESS_TOO_MUCH_DATA, CString(_T("ERROR_APE_COMPRESS_TOO_MUCH_DATA")));
	m_ErrorString.SetAt(ERROR_UNDEFINED, CString(_T("ERROR_UNDEFINED")));
}

const CString& CApeFileMedia::GetErrorString(INT nErrorCode)
{
	CString strRet = _T("UNKNOWN");
	if (m_ErrorString.Lookup(nErrorCode, strRet)) {
		return m_ErrorString[nErrorCode];
	}
	return m_ErrorString[ERROR_UNDEFINED];
}

BOOL CApeFileMedia::LoadMedia(LPCTSTR szFilePath, CPasswordGetterBase& PasswordGetter, CProgressBase& Progress, CPackErrors& Errors)
{
	INT nRetVal;
	INT64 nRet;
	BOOL bRet = FALSE;
	ULONGLONG nBlockLeft;
	APE::IAPEDecompress* pAPEDecompress = NULL;
	LPBYTE pBlockBuffer = NULL;

	if ((pAPEDecompress = CreateIAPEDecompress(szFilePath, &nRetVal, TRUE, TRUE, FALSE)) == NULL) {
		Errors.SetError(CPackErrors::PE_IO, szFilePath, GetErrorString(nRetVal));
		goto err;
	}

	if ((pAPEDecompress->GetInfo(APE::APE_INFO_WAVEFORMATEX, (APE::int64)&m_WaveFormatEx)) < 0) {
		Errors.SetError(CPackErrors::PE_INTERNAL);
		goto err;
	}

	if ((nRet = pAPEDecompress->GetInfo(APE::APE_INFO_FORMAT_FLAGS)) < 0) {
		Errors.SetError(CPackErrors::PE_INTERNAL);
		goto err;
	}
	m_nMacFlags = (UINT)nRet;


	if ((nRet = pAPEDecompress->GetInfo(APE::APE_INFO_TOTAL_BLOCKS)) < 0) {
		Errors.SetError(CPackErrors::PE_INTERNAL);
		goto err;
	}
	m_nTotalBlocks = (ULONGLONG)nRet;
	
	if ((nRet = pAPEDecompress->GetInfo(APE::APE_INFO_COMPRESSION_LEVEL)) < 0) {
		Errors.SetError(CPackErrors::PE_INTERNAL);
		goto err;
	}
	m_nCompressLevel = (UINT)nRet;
	
	// alloc buffers
	if ((pBlockBuffer = (LPBYTE)malloc(m_WaveFormatEx.nBlockAlign * APE_AUDIO_ONE_PASS_FRAMES)) == NULL) {
		Errors.SetError(CPackErrors::PE_NOMEM);
		goto err;
	}

	if (!CPCMAudioMedia::Alloc(m_nTotalBlocks, m_WaveFormatEx.nChannels, m_WaveFormatEx.wBitsPerSample, Errors)) {
		goto err;
	}

	// read data
	nBlockLeft = m_nTotalBlocks;
	Progress.Reset(IDS_READ_FILE);
	Progress.SetFullScale(m_nTotalBlocks);
	while (nBlockLeft) {
		if ((nRetVal = pAPEDecompress->GetData((char*)pBlockBuffer, APE_AUDIO_ONE_PASS_FRAMES, &nRet)) != ERROR_SUCCESS) {
			Errors.SetError(CPackErrors::PE_IO, szFilePath, GetErrorString(nRetVal));
			goto err;
		}
		CPCMAudioMedia::SetPackSample(pBlockBuffer, m_nTotalBlocks - nBlockLeft, (UINT)nRet);
		Progress.Increase(nRet);
		nBlockLeft -= nRet;
	}
	
	// test format
	if (!CBytePerBlockMedia::LoadMeta(PasswordGetter, Errors)) {
		goto err;
	}

	bRet = TRUE;
err:
	if (pBlockBuffer) {
		free(pBlockBuffer);
		pBlockBuffer = NULL;
	}
	if (!bRet)
		CloseMedia();
	SAFE_DELETE(pAPEDecompress);
	return bRet;
}

BOOL CApeFileMedia::SaveMedia(LPCTSTR szFilePath, CProgressBase& Progress, CPackErrors& Errors)
{
	INT nRetVal; 
	BOOL bRet = FALSE;
	ULONGLONG nBlockLeft, nByteWrite;
	LPBYTE pBlockBuffer = NULL;
	APE::IAPECompress* pAPECompress = NULL;

	if (!CBytePerBlockMedia::SaveMeta(Errors)) {
		goto err;
	}

	Progress.Reset(IDS_FILL_EMPTY_SPACE);
	Progress.SetFullScale(GetMediaTotalBytes() - GetMediaUsedBytes());

	if (!CBytePerBlockMedia::FillEmptySpace(Progress, Errors)) {
		goto err;
	}

	if ((pAPECompress = CreateIAPECompress(&nRetVal)) == NULL) {
		Errors.SetError(CPackErrors::PE_IO, szFilePath, GetErrorString(nRetVal));
		goto err;
	}

	if ((nRetVal = pAPECompress->Start(szFilePath, &m_WaveFormatEx, MAX_AUDIO_BYTES_UNKNOWN, m_nCompressLevel,
		NULL, CREATE_WAV_HEADER_ON_DECOMPRESSION, m_nMacFlags)) != 0) {
		Errors.SetError(CPackErrors::PE_IO, szFilePath, GetErrorString(nRetVal));
		goto err;
	}

	// alloc buffer
	if ((pBlockBuffer = (LPBYTE)malloc(m_WaveFormatEx.nBlockAlign * APE_AUDIO_ONE_PASS_FRAMES)) == NULL) {
		Errors.SetError(CPackErrors::PE_NOMEM);
		goto err;
	}

	// write data
	nBlockLeft = m_nTotalBlocks;
	Progress.Reset(IDS_WRITE_FILE);
	Progress.SetFullScale(m_nTotalBlocks);
	while (nBlockLeft) {
		nByteWrite = nBlockLeft > APE_AUDIO_ONE_PASS_FRAMES ?
			APE_AUDIO_ONE_PASS_FRAMES * m_WaveFormatEx.nBlockAlign : nBlockLeft * m_WaveFormatEx.nBlockAlign;
		CPCMAudioMedia::GetPackSample(pBlockBuffer, m_nTotalBlocks - nBlockLeft, (UINT)(nByteWrite / m_WaveFormatEx.nBlockAlign));
		if ((nRetVal = pAPECompress->AddData((unsigned char*)pBlockBuffer, nByteWrite)) != ERROR_SUCCESS) {
			Errors.SetError(CPackErrors::PE_IO, szFilePath, GetErrorString(nRetVal));
			goto err;
		}
		Progress.Increase(nByteWrite / m_WaveFormatEx.nBlockAlign);
		nBlockLeft -= nByteWrite / m_WaveFormatEx.nBlockAlign;
	}

	if ((nRetVal = pAPECompress->Finish(NULL, 0, 0)) != 0) {
		Errors.SetError(CPackErrors::PE_IO, szFilePath, GetErrorString(nRetVal));
		goto err;
	}

	// done!
	ClearMediaDirty();

	bRet = TRUE;
err:
	if (pBlockBuffer) {
		free(pBlockBuffer);
		pBlockBuffer = NULL;
	}
	SAFE_DELETE(pAPECompress);
	return bRet;
}

void CApeFileMedia::CloseMedia()
{
	CPCMAudioMedia::Free();
	::ZeroMemory(&m_WaveFormatEx,sizeof(m_WaveFormatEx));
	m_nMacFlags = 0;
	m_nTotalBlocks = 0;
	m_nCompressLevel = 0;
	m_OptPageApeFileProperty.m_strPCMProperty = _T("");
	m_ErrorString.RemoveAll();
}

CString CApeFileMedia::CompressLevel2String(UINT nCompressLevel)
{
	CString strRet = _T("UNKNOWN");
	switch (nCompressLevel) {
	case MAC_COMPRESSION_LEVEL_FAST:
		strRet = _T("FAST"); break;
	case MAC_COMPRESSION_LEVEL_NORMAL:
		strRet = _T("NORMAL"); break;
	case MAC_COMPRESSION_LEVEL_HIGH:
		strRet = _T("HIGH"); break;
	case MAC_COMPRESSION_LEVEL_EXTRA_HIGH:
		strRet = _T("EXTRA_HIGH"); break;
	case MAC_COMPRESSION_LEVEL_INSANE:
		strRet = _T("INSANE"); break;
	default:
		break;
	}
	return strRet;
}
/*
#define MAC_FORMAT_FLAG_8_BIT                 1    // is 8-bit [OBSOLETE]
#define MAC_FORMAT_FLAG_CRC                   2    // uses the new CRC32 error detection [OBSOLETE]
#define MAC_FORMAT_FLAG_HAS_PEAK_LEVEL        4    // uint32 nPeakLevel after the header [OBSOLETE]
#define MAC_FORMAT_FLAG_24_BIT                8    // is 24-bit [OBSOLETE]
#define MAC_FORMAT_FLAG_HAS_SEEK_ELEMENTS    16    // has the number of seek elements after the peak level
#define MAC_FORMAT_FLAG_CREATE_WAV_HEADER    32    // create the wave header on decompression (not stored)
#define MAC_FORMAT_FLAG_AIFF                 64    // the file is an AIFF that was compressed (instead of WAV)
#define MAC_FORMAT_FLAG_W64                 128    // the file is a W64 (instead of WAV)
#define MAC_FORMAT_FLAG_SND                 256    // the file is a SND (instead of WAV)
#define MAC_FORMAT_FLAG_BIG_ENDIAN          512    // flags that the file uses big endian encoding
*/
CString CApeFileMedia::Flag2String(UINT nFlag)
{
	CString strRet;
	if (nFlag == 0) {
		strRet += _T("    NONE\r\n");
	}
	if (nFlag & MAC_FORMAT_FLAG_8_BIT) {
		strRet += _T("    MAC_FORMAT_FLAG_8_BIT\r\n");
	}
	if (nFlag & MAC_FORMAT_FLAG_CRC) {
		strRet += _T("    MAC_FORMAT_FLAG_CRC\r\n");
	}
	if (nFlag & MAC_FORMAT_FLAG_HAS_PEAK_LEVEL) {
		strRet += _T("    MAC_FORMAT_FLAG_HAS_PEAK_LEVEL\r\n");
	}
	if (nFlag & MAC_FORMAT_FLAG_24_BIT) {
		strRet += _T("    MAC_FORMAT_FLAG_24_BIT\r\n");
	}
	if (nFlag & MAC_FORMAT_FLAG_HAS_SEEK_ELEMENTS) {
		strRet += _T("    MAC_FORMAT_FLAG_HAS_SEEK_ELEMENTS\r\n");
	}
	if (nFlag & MAC_FORMAT_FLAG_CREATE_WAV_HEADER) {
		strRet += _T("    MAC_FORMAT_FLAG_CREATE_WAV_HEADER\r\n");
	}
	if (nFlag & MAC_FORMAT_FLAG_AIFF) {
		strRet += _T("    MAC_FORMAT_FLAG_AIFF\r\n");
	}
	if (nFlag & MAC_FORMAT_FLAG_W64) {
		strRet += _T("    MAC_FORMAT_FLAG_W64\r\n");
	}
	if (nFlag & MAC_FORMAT_FLAG_SND) {
		strRet += _T("    MAC_FORMAT_FLAG_SND\r\n");
	}
	if (nFlag & MAC_FORMAT_FLAG_BIG_ENDIAN) {
		strRet += _T("    MAC_FORMAT_FLAG_BIG_ENDIAN\r\n");
	}
	return strRet;
}
void CApeFileMedia::AddOptPage(CMFCPropertySheet* pPropertySheet)
{
	CString strTemp;
	m_OptPageApeFileProperty.m_strPCMProperty = _T("");
	strTemp.Format(
	_T("MacFlags:\r\n    %sChannels:    %d\r\nBitsPerSample   %d\r\nBlockAlign:    %d\r\nTotalBlocks:    %I64d\r\nCompressLevel:   %s\r\n"),
		(LPCTSTR)Flag2String(m_nMacFlags), m_WaveFormatEx.nChannels, m_WaveFormatEx.wBitsPerSample, m_WaveFormatEx.nBlockAlign, m_nTotalBlocks,
		(LPCTSTR)CompressLevel2String(m_nCompressLevel)
	);
	m_OptPageApeFileProperty.m_strPCMProperty += strTemp;
	strTemp.Format(_T("SamplesPerSec:  %d\r\n"), m_WaveFormatEx.nSamplesPerSec);
	m_OptPageApeFileProperty.m_strPCMProperty += strTemp;
	pPropertySheet->AddPage(&m_OptPageApeFileProperty);
	CPCMAudioMedia::AddOptPage(pPropertySheet);
}

BOOL CApeFileMedia::UpdateOpts(CMFCPropertySheet* pPropertySheet)
{
	return CPCMAudioMedia::UpdateOpts(pPropertySheet);
}

CMediaBase* CApeFileMedia::Factory()
{
	CApeFileMedia* pRet = new(std::nothrow) CApeFileMedia();
	if(pRet)
		pRet->LoadErrorString();
	return pRet;
}

void CApeFileMedia::GetMediaInfo(CArray<CMediaFactory::CMediaInfo>& InfoArray)
{
	CMediaFactory::CMediaInfo Info;
	Info.Exts.Add(_T("ape"));
	Info.fnFactory = Factory;
	Info.nCatagory = IDS_MEDIA_AUDIO_FILE;
	Info.strName = _T("Monkey's Audio File");
	InfoArray.Add(Info);
}
