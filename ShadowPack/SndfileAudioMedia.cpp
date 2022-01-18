#include "pch.h"
#include "SndfileAudioMedia.h"
#include "PackUtils.h"
#include "resource.h"

CSndfileAudioMedia::CSndfileAudioMedia()
{
}

CSndfileAudioMedia::~CSndfileAudioMedia()
{
}


BOOL CSndfileAudioMedia::GetMeta(SNDFILE * infile, CAudioMeta& meta, CPackErrors& Errors)
{

	const char* str;
	int k;

	for (k = SF_STR_FIRST; k <= SF_STR_LAST; k++)
	{
		str = sf_get_string(infile, k);
		if (str != NULL) {
			meta.meta_str.SetAt(k, CStringA(str));
		}
	};

	memset(&meta.inst, 0, sizeof(meta.inst));
	memset(&meta.cues, 0, sizeof(meta.cues));
	memset(&meta.binfo, 0, sizeof(meta.binfo));

	if (meta.snd_info.channels < _countof(meta.chanmap))
	{
		size_t size = meta.snd_info.channels * sizeof(meta.chanmap[0]);
		meta.bChanMapSet = (sf_command(infile, SFC_GET_CHANNEL_MAP_INFO, meta.chanmap, size) == SF_TRUE);
	};
	meta.bCuesSet = (sf_command(infile, SFC_GET_CUE, &meta.cues, sizeof(meta.cues)) == SF_TRUE);
	meta.bInstSet = (sf_command(infile, SFC_GET_INSTRUMENT, &meta.inst, sizeof(meta.inst)) == SF_TRUE);
	meta.bBinfoSet = (sf_command(infile, SFC_GET_BROADCAST_INFO, &meta.binfo, sizeof(meta.binfo)) == SF_TRUE);
	meta.bCartInfoSet = (sf_command(infile, SFC_GET_CART_INFO, &meta.cart_info, sizeof(meta.cart_info)) == SF_TRUE);

	return TRUE;
}

BOOL CSndfileAudioMedia::SetMeta(SNDFILE* outfile, CAudioMeta& meta, CPackErrors& Errors)
{
	int k;
	CStringA str;
	BOOL bRet = FALSE;
	for (k = SF_STR_FIRST; k <= SF_STR_LAST; k++)
	{
		if(meta.meta_str.Lookup(k, str)) {
			sf_set_string(outfile, k, (LPCSTR)str);
		}
	};

	if (meta.snd_info.channels < _countof(meta.chanmap))
	{
		size_t size = meta.snd_info.channels * sizeof(meta.chanmap[0]);
		if (meta.bChanMapSet) {
			if (sf_command(outfile, SFC_SET_CHANNEL_MAP_INFO, meta.chanmap, size) != SF_TRUE) {
				goto err;
			}
		}
	};

	if (meta.bCuesSet && (sf_command(outfile, SFC_SET_CUE, &meta.cues, sizeof(meta.cues)) != SF_TRUE)) {
		goto err;
	}

	if (meta.bInstSet && (sf_command(outfile, SFC_SET_INSTRUMENT, &meta.inst, sizeof(meta.inst)) != SF_TRUE)) {
		goto err;
	}

	if (meta.bCartInfoSet && (sf_command(outfile, SFC_SET_CART_INFO, &meta.cart_info, sizeof(meta.cart_info)) != SF_TRUE)) {
		goto err;
	}

	if (meta.bCartInfoSet && (sf_command(outfile, SFC_SET_CART_INFO, &meta.cart_info, sizeof(meta.cart_info)) != SF_TRUE)) {
		goto err;
	}

	bRet = TRUE;
err:
	if (!bRet) {
		Errors.SetError(CPackErrors::PE_IO, CA2CT(sf_strerror(outfile)), meta.strFilePath);
	}
	return bRet;
}

BOOL CSndfileAudioMedia::IsFloat(INT nFormat)
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


INT CSndfileAudioMedia::GetBitsPerSample(INT nFormat)
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

CString CSndfileAudioMedia::GenerateDurationStr(SF_INFO& snd_info)
{
	double seconds;
	double total_seconds = 0;

	if (snd_info.samplerate < 1)
		return _T("unknown");

	if (snd_info.frames / snd_info.samplerate > 0x7FFFFFFF)
		return _T("unknown");

	seconds = (1.0 * snd_info.frames) / snd_info.samplerate;

	/* Accumulate the total of all known file durations */
	total_seconds += seconds;

	return CPackUtils::FormatSecond(total_seconds);
}
CString CSndfileAudioMedia::ReadLog(SNDFILE* file, SF_INFO &snd_info)
{
	CString strTemp;
	CString strRet;
	LPSTR pLogBuffer = NULL, pTok, pContext = NULL;
	// get log
	if (pLogBuffer = (LPSTR)malloc(PCM_LOG_BUFFER_SIZE)) {

		memset(pLogBuffer, 0, PCM_LOG_BUFFER_SIZE);
		sf_command(file, SFC_GET_LOG_INFO, pLogBuffer, PCM_LOG_BUFFER_SIZE);
		pLogBuffer[PCM_LOG_BUFFER_SIZE - 1] = 0;
		pTok = strtok_s(pLogBuffer, "\n", &pContext);
		while (pTok != NULL) {
			strRet += CA2CT(pTok);
			strRet += _T("\r\n");
			pTok = strtok_s(NULL, "\n", &pContext);
		}
		if (snd_info.frames == SF_COUNT_MAX) {
			strTemp.Format(_T("Frames      : unknown\r\n"));
			strRet += strTemp;
		}
		else {
			strTemp.Format(_T("Frames      : %I64d\r\n"), snd_info.frames);
			strRet += strTemp;

			strTemp.Format(_T("Channels    : %d\r\n"), snd_info.channels);
			strRet += strTemp;

			strTemp.Format(_T("Format      : 0x%08X\r\n"), snd_info.format);
			strRet += strTemp;

			strTemp.Format(_T("Sections    : %d\r\n"), snd_info.sections);
			strRet += strTemp;

			strTemp.Format(_T("Seekable    : %s\r\n"), snd_info.seekable ? _T("TRUE") : _T("FALSE"));
			strRet += strTemp;

			strTemp.Format(_T("Duration    : %s\r\n"), (LPCTSTR)GenerateDurationStr(snd_info));
			strRet += strTemp;
		}
		free(pLogBuffer);
		pLogBuffer = NULL;
	}
	TRACE(_T("%s"), strRet);
	return strRet;
}

BOOL CSndfileAudioMedia::LoadMedia(LPCTSTR szFilePath, CPasswordGetterBase& PasswordGetter, CProgressBase& Progress, CPackErrors& Errors)
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

	m_FileMeta.Free();
	m_FileMeta.strFilePath = szFilePath;

	if ((file = sf_open(filepath, SFM_READ, &m_FileMeta.snd_info)) == NULL) {
		Errors.SetError(CPackErrors::PE_IO, CA2CT(sf_strerror(file)), szFilePath);
		goto err;
	}

	if (m_FileMeta.snd_info.frames == SF_COUNT_MAX) {
		Errors.SetError(CPackErrors::PE_UNSUPPORT_MEDIA, szFilePath);
		goto err;
	}

	// get metas
	if (!GetMeta(file, m_FileMeta, Errors)) {
		goto err;
	}

	m_OptPageSndFileProperty.m_strPCMProperty = ReadLog(file, m_FileMeta.snd_info);

	nContainer = ((m_FileMeta.snd_info.format) & SF_FORMAT_TYPEMASK);
	nCodec     = ((m_FileMeta.snd_info.format) & SF_FORMAT_SUBMASK);
	nEndian    = ((m_FileMeta.snd_info.format) & SF_FORMAT_ENDMASK);

	// check format
	if ((nBitsPerSample = GetBitsPerSample(m_FileMeta.snd_info.format)) == -1) {
		Errors.SetError(CPackErrors::PE_UNSUPPORT_MEDIA, szFilePath);
		goto err;
	}

	TRACE(_T("Audio format %x, nContainer = %x, nCodec = %x, nEndian = %x nBitsPerSample = %d\n"),
		m_FileMeta.snd_info.format, 
		nContainer,
		nCodec,
		nEndian,
		nBitsPerSample);

	// alloc buffer
	if (!CPCMAudioMedia::Alloc(m_FileMeta.snd_info.frames, m_FileMeta.snd_info.channels, nBitsPerSample, Errors)) {
		goto err;
	}

	if (IsFloat(m_FileMeta.snd_info.format)) {
		if ((pBuffer = (LPBYTE)malloc(SND_FILE_ONE_PASS_FRAMES * nBitsPerSample / 8 * m_FileMeta.snd_info.channels)) == NULL) {
			Errors.SetError(CPackErrors::PE_NOMEM);
			goto err;
		}
	}
	else {
		if ((pBuffer = (LPBYTE)malloc(SND_FILE_ONE_PASS_FRAMES * sizeof(INT) * m_FileMeta.snd_info.channels)) == NULL) {
			Errors.SetError(CPackErrors::PE_NOMEM);
			goto err;
		}
	}

	nTotalFrames = m_FileMeta.snd_info.frames;
	nFrameOffset = 0;
	Progress.Reset(IDS_READ_FILE);
	Progress.SetFullScale(nTotalFrames);
	while (nTotalFrames > 0) {
		nRead = nTotalFrames > SND_FILE_ONE_PASS_FRAMES ? SND_FILE_ONE_PASS_FRAMES : nTotalFrames;
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
			Errors.SetError(CPackErrors::PE_IO, CA2CT(sf_strerror(file)), szFilePath);
			goto err;
		}
		CPCMAudioMedia::SetFrame(pBuffer, nFrameOffset, (UINT)nRead);
		nTotalFrames -= nRead;
		nFrameOffset += nRead;
		Progress.Increase(nRead);
		if (Progress.IsCanceled(Errors)) {
			goto err;
		}
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

BOOL CSndfileAudioMedia::SaveMedia(LPCTSTR szFilePath, CProgressBase& Progress, CPackErrors& Errors)
{
	CT2CA filepath(szFilePath);
	BOOL bRet = FALSE;
	SNDFILE* file = NULL;
	LPBYTE pBuffer = NULL;
	INT nBitsPerSample;
	sf_count_t nTotalFrames, nRet, nWrite, nFrameOffset;


	nTotalFrames = m_FileMeta.snd_info.frames;

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
		Errors.SetError(CPackErrors::PE_UNSUPPORT_MEDIA, szFilePath);
		goto err;
	}

	if ((file = sf_open(filepath, SFM_WRITE, &m_FileMeta.snd_info)) == NULL) {
		Errors.SetError(CPackErrors::PE_IO, CA2CT(sf_strerror(file)), szFilePath);
		goto err;
	}

	// set metas
	if (!SetMeta(file, m_FileMeta, Errors)) {
		goto err;
	}

	// alloc buffer
	if (IsFloat(m_FileMeta.snd_info.format)) {
		if ((pBuffer = (LPBYTE)malloc(SND_FILE_ONE_PASS_FRAMES * nBitsPerSample / 8 * m_FileMeta.snd_info.channels)) == NULL) {
			Errors.SetError(CPackErrors::PE_NOMEM);
			goto err;
		}
	}
	else {
		if ((pBuffer = (LPBYTE)malloc(SND_FILE_ONE_PASS_FRAMES * sizeof(INT) * m_FileMeta.snd_info.channels)) == NULL) {
			Errors.SetError(CPackErrors::PE_NOMEM);
			goto err;
		}
	}

//	if (sf_seek(file, 0, SEEK_SET) < 0) {
//		Errors.SetError(CPackErrors::PE_IO, CA2CT(sf_strerror(file)), szFilePath);
//		goto err;
//	}

	
	nFrameOffset = 0;
	Progress.Reset(IDS_WRITE_FILE);
	Progress.SetFullScale(nTotalFrames);
	while (nTotalFrames > 0) {
		nWrite = nTotalFrames > SND_FILE_ONE_PASS_FRAMES ? SND_FILE_ONE_PASS_FRAMES : nTotalFrames;
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
			Errors.SetError(CPackErrors::PE_IO, CA2CT(sf_strerror(file)), szFilePath);
			goto err;
		}
		nTotalFrames -= nWrite;
		nFrameOffset += nWrite;
		Progress.Increase(nWrite);
		if (Progress.IsCanceled(Errors)) {
			goto err;
		}
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

void CSndfileAudioMedia::CloseMedia()
{
	m_FileMeta.Free();
	m_OptPageSndFileProperty.m_strPCMProperty = _T("");
	CPCMAudioMedia::Free();
}

void CSndfileAudioMedia::AddOptPage(CMFCPropertySheet* pPropertySheet)
{

	pPropertySheet->AddPage(&m_OptPageSndFileProperty);
	CPCMAudioMedia::AddOptPage(pPropertySheet);
}

BOOL CSndfileAudioMedia::UpdateOpts(CMFCPropertySheet* pPropertySheet)
{
	return CPCMAudioMedia::UpdateOpts(pPropertySheet);;
}

CMediaFactory::MEDIA_EXT_TABLE_T CSndfileAudioMedia::m_szExtTable[] = {
	{_T("paf,fap"),_T("Ensoniq PARIS") },
	{_T("nist,sph"),_T("NIST Sphere") },
	{_T("voc"),_T("Creative Labs") },
	{_T("sf,ircam"),_T("Berkeley/IRCAM/CARL") },
	{_T("mat,mat4,mat5"),_T("GNU Octave 2.0/2.1 ,Matlab 4.2/5.0") },
	{_T("pvf"),_T("Portable Voice Format") },
	{_T("avr"),_T("Audio Visual Research") },
	{_T("wavex"),_T("Microsoft WAVEX") },
	{_T("sd2"),_T("Sound Designer II") },
	{_T("mpc"),_T("Akai MPC 2k") },
	{_T("rf64,bwf"),_T("RIFF 64") },
};

CMediaBase* CSndfileAudioMedia::Factory()
{
	return new(std::nothrow) CSndfileAudioMedia();
}

void CSndfileAudioMedia::GetMediaInfo(CArray<CMediaFactory::CMediaInfo>& InfoArray)
{
	CMediaFactory::LoadMediaExt(m_szExtTable, _countof(m_szExtTable), Factory, IDS_MEDIA_AUDIO_FILE, InfoArray);
}