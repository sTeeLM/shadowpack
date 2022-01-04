#include "pch.h"
#include "MiscAudioMedia.h"

CMiscAudioMedia::CMiscAudioMedia()
{
}

CMiscAudioMedia::~CMiscAudioMedia()
{
}

BOOL CMiscAudioMedia::LoadMedia(LPCTSTR szFilePath, CPasswordGetterBase& PasswordGetter, CProgressBase& Progress, CPackErrors& Errors)
{
	CT2CA filepath(szFilePath);
	SNDFILE * file = NULL;
	UINT nContainer;
	UINT nCodec;
	UINT nEndian;
	BOOL bRet = FALSE;
	BOOL bCodecOK = FALSE;

	::ZeroMemory(&m_Info, sizeof(m_Info));

	if ((file = sf_open(filepath, SFM_READ, &m_Info.snd_info)) == NULL) {
		goto err;
	}

	// get cart info
	m_Info.bCartInfoSet = (sf_command(file, SFC_GET_CART_INFO, &m_Info.cart_info, sizeof(m_Info.cart_info)) != SF_FALSE);

	nContainer = ((m_Info.snd_info.format) & SF_FORMAT_TYPEMASK);
	nCodec     = ((m_Info.snd_info.format) & SF_FORMAT_SUBMASK);
	nEndian    = ((m_Info.snd_info.format) & SF_FORMAT_ENDMASK);

	// check nCodec
	switch (nCodec) {
	case SF_FORMAT_PCM_S8:
	case SF_FORMAT_PCM_16:
	case SF_FORMAT_PCM_24:
	case SF_FORMAT_PCM_32:
	case SF_FORMAT_PCM_U8:
	case SF_FORMAT_FLOAT:
	case SF_FORMAT_DOUBLE:
	case SF_FORMAT_ALAC_16:
	case SF_FORMAT_ALAC_20:
	case SF_FORMAT_ALAC_24:
	case SF_FORMAT_ALAC_32:
		bCodecOK = TRUE;
		break;
	default:
		break;
	}
	sf_readf_short(file, NULL, 1);
	sf_close(file);
err:
	return FALSE;
}

BOOL CMiscAudioMedia::SaveMedia(LPCTSTR szFilePath, CProgressBase& Progress, CPackErrors& Errors)
{
	return 0;
}

void CMiscAudioMedia::CloseMedia()
{
}

void CMiscAudioMedia::AddOptPage(CMFCPropertySheet* pPropertySheet)
{
}

BOOL CMiscAudioMedia::UpdateOpts(CMFCPropertySheet* pPropertySheet)
{
	return 0;
}

LPCTSTR CMiscAudioMedia::m_szName = _T("wav audio file");
LPCTSTR CMiscAudioMedia::m_szExtTable[] = {
	_T("wav"),
	_T("flac"),
	_T("m4a"),
	NULL
};

CMediaBase* CMiscAudioMedia::Factory()
{
	return new(std::nothrow) CMiscAudioMedia();
}

LPCTSTR CMiscAudioMedia::GetName()
{
	return m_szName;
}

LPCTSTR* CMiscAudioMedia::GetExtTable()
{
	return m_szExtTable;
}
