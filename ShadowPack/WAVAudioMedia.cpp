#include "pch.h"
#include "WAVAudioMedia.h"

CWAVAudioMedia::CWAVAudioMedia() :
	m_FileSetup(NULL)
{
}

CWAVAudioMedia::~CWAVAudioMedia()
{
}

CString CWAVAudioMedia::m_strLastError = _T("");

void CWAVAudioMedia::WAVError(long nCode, const char* strMessage)
{
	CA2CT strMsg(strMessage);
	m_strLastError = strMessage;
}

BOOL CWAVAudioMedia::LoadMedia(LPCTSTR szFilePath, CPasswordGetterBase& PasswordGetter, CProgressBase& Progress, CPackErrors& Errors)
{
	CT2CA filepath(szFilePath);
	AFfilehandle file = NULL;
	INT filefmt;
	const char* formatstring = NULL;
	const char* labelstring = NULL;
	INT sampleFormat, sampleWidth;
	INT byteOrder;
	INT compressionType;
	FLOAT frameSize;

	afSetErrorHandler(WAVError);

	if ((m_FileSetup = afNewFileSetup()) == AF_NULL_FILESETUP) {
		Errors.SetError(CPackErrors::PE_NOMEM);
		goto err;
	}

	if ((file = afOpenFile(filepath, "rw", m_FileSetup)) == AF_NULL_FILEHANDLE) {
		Errors.SetError(CPackErrors::PE_IO, szFilePath, m_strLastError);
		goto err;
	}

	filefmt = afGetFileFormat(file, NULL);

	// Microsoft RIFF WAVE Format
	formatstring =
		(const char*)afQueryPointer(AF_QUERYTYPE_FILEFMT, AF_QUERY_DESC,
			filefmt, 0, 0);
	// wave
	labelstring =
		(const char*)afQueryPointer(AF_QUERYTYPE_FILEFMT, AF_QUERY_LABEL,
			filefmt, 0, 0);

	afGetSampleFormat(file, AF_DEFAULT_TRACK, &sampleFormat, &sampleWidth);

	byteOrder = afGetByteOrder(file, AF_DEFAULT_TRACK);

	compressionType = afGetCompression(file, AF_DEFAULT_TRACK);
	
	// afGetChannels();
	frameSize = afGetFrameSize(file, AF_DEFAULT_TRACK, 0);
	afCloseFile(file);
	afFreeFileSetup(m_FileSetup);
err:
	return FALSE;
}

BOOL CWAVAudioMedia::SaveMedia(LPCTSTR szFilePath, CProgressBase& Progress, CPackErrors& Errors)
{
	return 0;
}

void CWAVAudioMedia::CloseMedia()
{
}

void CWAVAudioMedia::AddOptPage(CMFCPropertySheet* pPropertySheet)
{
}

BOOL CWAVAudioMedia::UpdateOpts(CMFCPropertySheet* pPropertySheet)
{
	return 0;
}

LPCTSTR CWAVAudioMedia::m_szName = _T("wav audio file");
LPCTSTR CWAVAudioMedia::m_szExtTable[] = {
	_T("wav"),
	_T("flac"),
	_T("m4a"),
	NULL
};

CMediaBase* CWAVAudioMedia::Factory()
{
	return new(std::nothrow) CWAVAudioMedia();
}

LPCTSTR CWAVAudioMedia::GetName()
{
	return m_szName;
}

LPCTSTR* CWAVAudioMedia::GetExtTable()
{
	return m_szExtTable;
}
