#include "pch.h"
#include "MiscAudioMedia.h"
#include "resource.h"

CMiscAudioMedia::CMiscAudioMedia()
{
}

CMiscAudioMedia::~CMiscAudioMedia()
{
}

BOOL CMiscAudioMedia::GetMeta(SNDFILE * file, MISC_AUDIO_META_T& meta, CPackErrors& Errors)
{
	return TRUE;
}

BOOL CMiscAudioMedia::SetMeta(SNDFILE* file, MISC_AUDIO_META_T& meta, CPackErrors& Errors)
{
	return TRUE;
}

BOOL CMiscAudioMedia::IsFloat(INT nFormat)
{
	BOOL bRet = FALSE;
	switch ((nFormat)&SF_FORMAT_SUBMASK) {
	case SF_FORMAT_DOUBLE:
	case SF_FORMAT_FLOAT:
		bRet = TRUE;
		break;
	default:
		break;
	}
	return bRet;
}

INT CMiscAudioMedia::GetBitsPerSample(INT nFormat)
{
	INT nRet = -1;
	switch ((nFormat) & SF_FORMAT_SUBMASK) {
	case SF_FORMAT_PCM_S8:
	case SF_FORMAT_PCM_U8:
		nRet = 8; break;
	case SF_FORMAT_ALAC_16:
	case SF_FORMAT_PCM_16:
		nRet = 16; break;
	case SF_FORMAT_ALAC_24:
	case SF_FORMAT_PCM_24:
		nRet = 24; break;
	case SF_FORMAT_FLOAT:
	case SF_FORMAT_PCM_32:
	case SF_FORMAT_ALAC_32:
		nRet = 32; break;
	case SF_FORMAT_DOUBLE:
		nRet = 64; break;
	case SF_FORMAT_ALAC_20:
		nRet = 20; break;
	default:
		break;
	}
	return nRet;
}

BOOL CMiscAudioMedia::LoadMedia(LPCTSTR szFilePath, CPasswordGetterBase& PasswordGetter, CProgressBase& Progress, CPackErrors& Errors)
{
	CT2CA filepath(szFilePath);
	SNDFILE * file = NULL;
	INT nContainer;
	INT nCodec;
	INT nEndian;
	INT nBitsPerSample;
	BOOL bRet = FALSE;
	BOOL bCodecOK = FALSE;
	LPBYTE pBuffer = NULL;
	sf_count_t nTotalFrames, nRet, nRead, nFrameOffset;


	::ZeroMemory(&m_FileMeta, sizeof(m_FileMeta));

	if ((file = sf_open(filepath, SFM_READ, &m_FileMeta.snd_info)) == NULL) {
		goto err;
	}

	// get metas
	if (!GetMeta(file, m_FileMeta, Errors)) {
		goto err;
	}

	nContainer = ((m_FileMeta.snd_info.format) & SF_FORMAT_TYPEMASK);
	nCodec     = ((m_FileMeta.snd_info.format) & SF_FORMAT_SUBMASK);
	nEndian    = ((m_FileMeta.snd_info.format) & SF_FORMAT_ENDMASK);

	// check format
	if ((nBitsPerSample = GetBitsPerSample(m_FileMeta.snd_info.format)) == -1) {
		goto err;
	}

	// alloc buffer
	if (!CPCMAudioMedia::Alloc(m_FileMeta.snd_info.frames, m_FileMeta.snd_info.channels, nBitsPerSample, Errors)) {
		goto err;
	}

	if (IsFloat(m_FileMeta.snd_info.format)) {
		if ((pBuffer = (LPBYTE)malloc(ONE_PASS_FRAMES * nBitsPerSample * m_FileMeta.snd_info.channels)) == NULL) {
			goto err;
		}
	}
	else {
		if ((pBuffer = (LPBYTE)malloc(ONE_PASS_FRAMES * 32 * m_FileMeta.snd_info.channels)) == NULL) {
			goto err;
		}
	}

	nTotalFrames = m_FileMeta.snd_info.frames;
	nFrameOffset = 0;
	while (nTotalFrames > 0) {
		nRead = nTotalFrames > ONE_PASS_FRAMES ? ONE_PASS_FRAMES : nTotalFrames;
		if (IsFloat(m_FileMeta.snd_info.format) && nBitsPerSample == 32) {
			nRet = sf_readf_float(file, (float *)pBuffer, nRead);
		}
		else if (IsFloat(m_FileMeta.snd_info.format) && nBitsPerSample == 64) {
			nRet = sf_readf_double(file, (double*)pBuffer, nRead);
		}
		else {
			nRet = sf_readf_int(file, (int*)pBuffer, nRead);
		}
		if (nRet != nRead) {
			goto err;
		}
		CPCMAudioMedia::SetFrame(pBuffer, nFrameOffset, (UINT)nRead);
		nTotalFrames -= nRead;
		nFrameOffset += nRead;
	}
	
	// test format
	if (!CBytePerBlockMedia::LoadMeta(PasswordGetter, Errors)) {
		goto err;
	}
	
	bRet = TRUE;
err:
	if (file) {
		sf_close(file);
		file = NULL;
	}
	if (pBuffer) {
		free(pBuffer);
		pBuffer = NULL;
	}
	if (!bRet) {
		CloseMedia();
	}
	return bRet;
}

BOOL CMiscAudioMedia::SaveMedia(LPCTSTR szFilePath, CProgressBase& Progress, CPackErrors& Errors)
{
	CT2CA filepath(szFilePath);
	BOOL bRet = FALSE;
	SNDFILE* file = NULL;
	SF_INFO snd_info;
	LPBYTE pBuffer = NULL;
	INT nBitsPerSample;
	sf_count_t nTotalFrames, nRet, nWrite, nFrameOffset;

	if (!CBytePerBlockMedia::SaveMeta(Errors)) {
		goto err;
	}
	Progress.Reset(IDS_FILL_EMPTY_SPACE);
	Progress.SetFullScale(GetMediaTotalBytes() - GetMediaUsedBytes());

	if (!CBytePerBlockMedia::FillEmptySpace(Progress, Errors)) {
		goto err;
	}

	// check format
	if ((nBitsPerSample = GetBitsPerSample(m_FileMeta.snd_info.format)) == -1) {
		goto err;
	}

	if ((file = sf_open(filepath, SFM_WRITE, &snd_info)) == NULL) {
		goto err;
	}

	// set metas
	if (!SetMeta(file, m_FileMeta, Errors)) {
		goto err;
	}

	// alloc buffer
	if (IsFloat(m_FileMeta.snd_info.format)) {
		if ((pBuffer = (LPBYTE)malloc(ONE_PASS_FRAMES * nBitsPerSample * m_FileMeta.snd_info.channels)) == NULL) {
			goto err;
		}
	}
	else {
		if ((pBuffer = (LPBYTE)malloc(ONE_PASS_FRAMES * 32 * m_FileMeta.snd_info.channels)) == NULL) {
			goto err;
		}
	}

	if (sf_seek(file, 0, SEEK_SET) < 0) {
		goto err;
	}

	nTotalFrames = m_FileMeta.snd_info.frames;
	nFrameOffset = 0;
	while (nTotalFrames > 0) {
		nWrite = nTotalFrames > ONE_PASS_FRAMES ? ONE_PASS_FRAMES : nTotalFrames;
		CPCMAudioMedia::GetFrame(pBuffer, nFrameOffset, (UINT)nWrite);
		if (IsFloat(m_FileMeta.snd_info.format) && nBitsPerSample == 32) {
			nRet = sf_writef_float(file, (float*)pBuffer, nWrite);
		}
		else if (IsFloat(m_FileMeta.snd_info.format) && nBitsPerSample == 64) {
			nRet = sf_writef_double(file, (double*)pBuffer, nWrite);
		}
		else {
			nRet = sf_writef_int(file, (int*)pBuffer, nWrite);
		}
		if (nRet != nWrite) {
			goto err;
		}
		nTotalFrames -= nWrite;
		nFrameOffset += nWrite;
	}
	// done!
	ClearMediaDirty();

	bRet = TRUE;
err:
	if (file) {
		sf_close(file);
		file = NULL;
	}
	if (pBuffer) {
		free(pBuffer);
		pBuffer = NULL;
	}
	return bRet;
}

void CMiscAudioMedia::CloseMedia()
{
	CPCMAudioMedia::Free();
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
	_T("aif"),
	_T("au"),
	_T("snd"),
	_T("raw"),
	_T("gsm"),
	_T("vox"),
	_T("paf"),
	_T("fap"),
	_T("svx"),
	_T("nist"),
	_T("sph"),
	_T("voc"),
	_T("ircam"),
	_T("sf"),
	_T("w64"),
	_T("mat"),
	_T("mat4"),
	_T("mat5"),
	_T("pvf"),
	_T("xi"),
	_T("htk"),
	_T("sds"),
	_T("avr"),
	_T("wavex"),
	_T("sd2"),
	_T("flac"),
	_T("caf"),
	_T("wve"),
	_T("prc"),
	_T("ogg"),
	_T("oga"),
	_T("mpc"),
	_T("rf64"),
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
