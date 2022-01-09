#include "pch.h"
#include "MiscAudioMedia.h"
#include "PackUtils.h"
#include "resource.h"

CMiscAudioMedia::CMiscAudioMedia()
{
}

CMiscAudioMedia::~CMiscAudioMedia()
{
}



BOOL CMiscAudioMedia::IsFloat(INT nFormat)
{
	return FALSE;
}


BOOL CMiscAudioMedia::CheckCodecID(INT nCodecID)
{
	BOOL bRet = FALSE;
	switch (nCodecID) {
	case AV_CODEC_ID_PCM_S16LE:
	case AV_CODEC_ID_PCM_S16BE:
	case AV_CODEC_ID_PCM_U16LE:
	case AV_CODEC_ID_PCM_U16BE:
	case AV_CODEC_ID_PCM_S8:
	case AV_CODEC_ID_PCM_U8:
	case AV_CODEC_ID_PCM_MULAW:
	case AV_CODEC_ID_PCM_ALAW:
	case AV_CODEC_ID_PCM_S32LE:
	case AV_CODEC_ID_PCM_S32BE:
	case AV_CODEC_ID_PCM_U32LE:
	case AV_CODEC_ID_PCM_U32BE:
	case AV_CODEC_ID_PCM_S24LE:
	case AV_CODEC_ID_PCM_S24BE:
	case AV_CODEC_ID_PCM_U24LE:
	case AV_CODEC_ID_PCM_U24BE:
	case AV_CODEC_ID_PCM_F32BE:
	case AV_CODEC_ID_PCM_F32LE:
	case AV_CODEC_ID_PCM_F64BE:
	case AV_CODEC_ID_PCM_F64LE:
	case AV_CODEC_ID_PCM_S8_PLANAR:
	case AV_CODEC_ID_PCM_S16LE_PLANAR:
	case AV_CODEC_ID_PCM_S24LE_PLANAR:
	case AV_CODEC_ID_PCM_S32LE_PLANAR:
	case AV_CODEC_ID_PCM_S16BE_PLANAR:
	case AV_CODEC_ID_PCM_S64LE:
	case AV_CODEC_ID_PCM_S64BE:
	case AV_CODEC_ID_PCM_F16LE:
	case AV_CODEC_ID_PCM_F24LE:
	case AV_CODEC_ID_PCM_VIDC:
	case AV_CODEC_ID_FLAC:
	case AV_CODEC_ID_ALAC:
	case AV_CODEC_ID_APE:
		bRet = TRUE;
		break;
	default:
		break;
	}
	return bRet;
}

CString CMiscAudioMedia::m_strLastLog;
void CMiscAudioMedia::CBLogger(void* avcl, int level, const char* fmt, va_list vl)
{
	CHAR Buffer[4096];
	::vsnprintf(Buffer, sizeof(Buffer) - 1, fmt, vl);
	Buffer[sizeof(Buffer) - 1] = 0;
	m_strLastLog += (LPCTSTR)CA2CT(Buffer);
	TRACE(_T("%s\n"), (LPCTSTR)CA2CT(Buffer));
}

BOOL  CMiscAudioMedia::ProbeTotalFrames(
	AVFormatContext* pFormatCtx,
	LPCTSTR szFilePath,
	ULONGLONG &nTotalFrames, 
	CPackErrors& Errors)
{
	AVPacket* input_packet = NULL;
	nTotalFrames = 0;
	INT nRet = 0;
	BOOL bRet = FALSE;
	if ((input_packet = av_packet_alloc())) {
		while ((nRet = av_read_frame(pFormatCtx, input_packet)) == 0) {
			nTotalFrames++;
		}

		if (nRet != AVERROR_EOF) {
			Errors.SetError(CPackErrors::PE_IO, szFilePath, GetErrorString(nRet));
		} else {
			if (av_seek_frame(pFormatCtx, 0, 0, AVSEEK_FLAG_BACKWARD) >= 0) {
				bRet = TRUE;
			}
		}
		av_packet_free(&input_packet);
		input_packet = NULL;
	}
	return bRet;
}

CString CMiscAudioMedia::GetErrorString(INT nErr)
{
	CHAR Buffer[4096];
	const char* str = av_make_error_string(Buffer, sizeof(Buffer) - 1, nErr);
	CString strRet;
	if (str) {
		CString strRet = (LPCTSTR)CA2CT(str);
	}
	return strRet;
}

BOOL CMiscAudioMedia::LoadMedia(LPCTSTR szFilePath, CPasswordGetterBase& PasswordGetter, CProgressBase& Progress, CPackErrors& Errors)
{
	CT2CA filepath(szFilePath);
	BOOL bRet = FALSE;
	INT nRet;
	INT nBitsPerSample;
	ULONGLONG nTotalSamples;
	AVFormatContext* pFormatCtx = NULL;
	AVDictionary * pOptions = NULL;
	AVCodecContext* pCodecCtx = NULL;
	AVCodec* pCodec = NULL;
//	AVCodecParserContext* pParserCtx = NULL;
	AVPacket* input_packet = NULL;
	AVFrame* input_frame = NULL;

	if ((nRet = avformat_open_input(&pFormatCtx, filepath, NULL, NULL)) < 0) {
		Errors.SetError(CPackErrors::PE_IO, szFilePath, GetErrorString(nRet));
		goto err;
	}

	if ((nRet = avformat_find_stream_info(pFormatCtx, &pOptions)) < 0) {
		Errors.SetError(CPackErrors::PE_IO, szFilePath, GetErrorString(nRet));
		goto err;
	}

	if (pFormatCtx->nb_streams != 1 
		|| pFormatCtx->streams[0]->codecpar->codec_type != AVMEDIA_TYPE_AUDIO) {
		Errors.SetError(CPackErrors::PE_UNSUPPORT_MEDIA, szFilePath);
		goto err;
	}

	// check format

	av_log_set_callback(CBLogger);
	av_dump_format(pFormatCtx, 0, filepath, 0);
	av_log_set_callback(av_log_default_callback);
	
	if ((pCodec = avcodec_find_decoder(pFormatCtx->streams[0]->codecpar->codec_id)) == NULL) {
		Errors.SetError(CPackErrors::PE_IO, szFilePath, GetErrorString(nRet));
		goto err;
	}

	// check codec
	if (!CheckCodecID(pCodec->id)) {
		Errors.SetError(CPackErrors::PE_UNSUPPORT_MEDIA, szFilePath);
		goto err;
	}

//	pParserCtx = av_parser_init(pCodec->id);

	if ((pCodecCtx = avcodec_alloc_context3(pCodec)) == NULL) {
		Errors.SetError(CPackErrors::PE_UNSUPPORT_MEDIA, szFilePath);
		goto err;
	}

	if ((nRet = avcodec_parameters_to_context(pCodecCtx, pFormatCtx->streams[0]->codecpar)) < 0) {
		Errors.SetError(CPackErrors::PE_UNSUPPORT_MEDIA, szFilePath);
		goto err;
	}

	if ((nRet = avcodec_open2(pCodecCtx, pCodec, &pOptions)) < 0) {
		Errors.SetError(CPackErrors::PE_UNSUPPORT_MEDIA, szFilePath);
		goto err;
	}

	if ((input_frame = av_frame_alloc()) == NULL) {
		Errors.SetError(CPackErrors::PE_NOMEM);
		goto err;
	}

	if ((input_packet = av_packet_alloc()) == NULL) {
		Errors.SetError(CPackErrors::PE_NOMEM);
		goto err;
	}

	nTotalSamples= pFormatCtx->streams[0]->nb_frames;
	if (nTotalSamples == 0) {
		if (!ProbeTotalFrames(pFormatCtx, szFilePath, nTotalSamples, Errors)) {
			goto err;
		}
	}

	if (nTotalSamples == 0) {
		Errors.SetError(CPackErrors::PE_UNSUPPORT_MEDIA, szFilePath);
		goto err;
	}

	nBitsPerSample = av_get_bits_per_sample(pCodec->id);
	if (nBitsPerSample == 0) {
		nBitsPerSample = pCodecCtx->bits_per_raw_sample;
	}
	// alloc buffer
	if (!CPCMAudioMedia::Alloc(nTotalSamples,
		pCodecCtx->channels, nBitsPerSample, Errors)) {
		goto err;
	}

	//avformat_seek_file(pFormatCtx, 0, );
	av_read_frame(pFormatCtx, input_packet);

	avcodec_send_packet(pCodecCtx, input_packet);

	avcodec_receive_frame(pCodecCtx, input_frame);
	{
	int ret = 	av_samples_get_buffer_size(&input_frame->linesize[0], input_frame->channels,
			input_frame->nb_samples, (AVSampleFormat)input_frame->format, 0);
	ret = 0;
	AVSampleFormat;
	}

	av_get_bytes_per_sample((AVSampleFormat)(input_frame->format));

	//AVSampleFormat;
	//av_frame_get_buffer
	
	// test format
	if (!CBytePerBlockMedia::LoadMeta(PasswordGetter, Errors)) {
		goto err;
	}
	
	bRet = TRUE;
err:

	if (!bRet) {
		CloseMedia();
	}
	return bRet;
}

BOOL CMiscAudioMedia::SaveMedia(LPCTSTR szFilePath, CProgressBase& Progress, CPackErrors& Errors)
{
	CT2CA filepath(szFilePath);
	BOOL bRet = FALSE;


	if (!CBytePerBlockMedia::SaveMeta(Errors)) {
		goto err;
	}

	Progress.Reset(IDS_FILL_EMPTY_SPACE);
	Progress.SetFullScale(GetMediaTotalBytes() - GetMediaUsedBytes());

	if (!CBytePerBlockMedia::FillEmptySpace(Progress, Errors)) {
		goto err;
	}


	// done!
	ClearMediaDirty();

	bRet = TRUE;
err:

	return bRet;
}

void CMiscAudioMedia::CloseMedia()
{
	m_FileMeta.Free();
	m_OptPagePCMFileProperty.m_strPCMProperty = _T("");
	CPCMAudioMedia::Free();
}

void CMiscAudioMedia::AddOptPage(CMFCPropertySheet* pPropertySheet)
{

	pPropertySheet->AddPage(&m_OptPagePCMFileProperty);
	CPCMAudioMedia::AddOptPage(pPropertySheet);
}

BOOL CMiscAudioMedia::UpdateOpts(CMFCPropertySheet* pPropertySheet)
{
	return CPCMAudioMedia::UpdateOpts(pPropertySheet);;
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
