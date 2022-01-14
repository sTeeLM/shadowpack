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

CString  CMiscAudioMedia::FillInfoStr()
{
	CString strRet,strTmp;
	AVFormatContext* ic = m_FileMeta.m_pFormatCtx;

	strRet += _T("Format:    ");
	strTmp.Format(_T("%s\r\n"), (LPCTSTR)CA2CT(m_FileMeta.m_pFormatCtx->iformat->long_name));
	strRet += strTmp;

	strRet += _T("Duration:    ");
	if (ic->duration != AV_NOPTS_VALUE) {
		int64_t hours, mins, secs, us;
		int64_t duration = ic->duration + (ic->duration <= INT64_MAX - 5000 ? 5000 : 0);
		secs = duration / AV_TIME_BASE;
		us = duration % AV_TIME_BASE;
		mins = secs / 60;
		secs %= 60;
		hours = mins / 60;
		mins %= 60;
		strTmp.Format(_T("%02I64d:%02I64d:%02I64d.%02I64d\r\n"), hours, mins, secs,
			(100 * us) / AV_TIME_BASE);
		strRet += strTmp;
	} else {
		strRet += _T("N/A\r\n");
	}

	strRet += _T("Start:       ");
	if (ic->start_time != AV_NOPTS_VALUE) {
		int secs, us;
		
		secs = llabs(ic->start_time / AV_TIME_BASE);
		us = llabs(ic->start_time % AV_TIME_BASE);
		strTmp.Format(_T("%s%d.%06d\r\n"),
			ic->start_time >= 0 ? _T("") : _T("-"),
			secs,
			(int)av_rescale(us, 1000000, AV_TIME_BASE));
		strRet += strTmp;
	} else {
		strRet += _T("N/A\r\n");
	}

	strRet += _T("Bitrate:       ");
	if (ic->bit_rate)
		strTmp.Format(_T("%I64d kb/s\r\n"), ic->bit_rate / 1000);
	else
		strTmp = _T("N/A\r\n");
	strRet += strTmp;

	strRet += _T("SampleRate:    ");
	strTmp.Format(_T("%d\r\n"), m_FileMeta.m_pCodecCtx->sample_rate);
	strRet += strTmp;

	strRet += _T("Frames:       ");
	strTmp.Format(_T("%I64d\r\n"), m_FileMeta.m_TotalFrames);
	strRet += strTmp;

	strRet += _T("Codec:       ");
	strTmp.Format(_T("%s\r\n"), (LPCTSTR)CA2CT(m_FileMeta.m_pCodec->long_name));
	strRet += strTmp;

	strRet += _T("Meta:       \r\n");
	if (m_FileMeta.m_pFormatCtx->metadata) {
		const AVDictionaryEntry* tag = NULL;
		while ((tag = av_dict_get(m_FileMeta.m_pFormatCtx->metadata, "", tag, AV_DICT_IGNORE_SUFFIX))) {
			strTmp.Format(_T("  [%s] : [%s]\r\n"), (LPCTSTR)CA2CT(tag->key), (LPCTSTR)CA2CT(tag->value));
		}
	}
	strRet += strTmp;

	return strRet;
}


/* only lossless codec used */
BOOL CMiscAudioMedia::CheckCodecID(INT nCodecID)
{
	BOOL bRet = FALSE;
	switch (nCodecID) {
	case AV_CODEC_ID_DTS:
	case AV_CODEC_ID_WAVPACK:
	case AV_CODEC_ID_TTA:
	case AV_CODEC_ID_TRUEHD:
	case AV_CODEC_ID_S302M:
	case AV_CODEC_ID_MLP:
	case AV_CODEC_ID_PCM_DVD:
	case AV_CODEC_ID_PCM_S16LE:
	case AV_CODEC_ID_PCM_S16BE:
	case AV_CODEC_ID_PCM_U16LE:
	case AV_CODEC_ID_PCM_U16BE:
	case AV_CODEC_ID_PCM_S8:
	case AV_CODEC_ID_PCM_U8:
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
	AVCodecContext* pCodecCtx,
	LPCTSTR szFilePath,
	ULONGLONG &nTotalFrames, 
	CPackErrors& Errors)
{
	AVPacket* input_packet = NULL;
	AVFrame* input_frame = NULL;
	nTotalFrames = 0;
	INT nRet = 0;
	BOOL bRet = FALSE;
	if ((input_packet = av_packet_alloc())) {
		if ((input_frame = av_frame_alloc())) {

			while ((nRet = av_read_frame(pFormatCtx, input_packet)) == 0) {
				if ((nRet = avcodec_send_packet(pCodecCtx, input_packet)) != 0) {
					break;
				}

				if ((nRet = avcodec_receive_frame(pCodecCtx, input_frame)) != 0) {
					break;
				}
				nTotalFrames += input_frame->nb_samples;
			}

			if (nRet != AVERROR_EOF) {
				Errors.SetError(CPackErrors::PE_IO, szFilePath, GetErrorString(nRet));
			}
			else {
				if (av_seek_frame(pFormatCtx, 0, 0, AVSEEK_FLAG_BACKWARD) >= 0) {
					bRet = TRUE;
				}
			}
			av_frame_free(&input_frame);
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
	ULONGLONG nTotalFramesSave;
//	AVDictionary * pOptions = NULL;
//	AVCodecParserContext* pParserCtx = NULL;
	AVPacket* input_packet = NULL;
	AVFrame* input_frame = NULL;

	if ((nRet = avformat_open_input(&m_FileMeta.m_pFormatCtx, filepath, NULL, NULL)) < 0) {
		Errors.SetError(CPackErrors::PE_IO, szFilePath, GetErrorString(nRet));
		goto err;
	}

	if ((nRet = avformat_find_stream_info(m_FileMeta.m_pFormatCtx, NULL)) < 0) {
		Errors.SetError(CPackErrors::PE_IO, szFilePath, GetErrorString(nRet));
		goto err;
	}

	//GetMeta();

	av_log_set_callback(CBLogger);
	av_dump_format(m_FileMeta.m_pFormatCtx, 0, filepath, 0);
	av_log_set_callback(av_log_default_callback);

	if (m_FileMeta.m_pFormatCtx->nb_streams != 1
		|| m_FileMeta.m_pFormatCtx->streams[0]->codecpar->codec_type != AVMEDIA_TYPE_AUDIO) {
		Errors.SetError(CPackErrors::PE_UNSUPPORT_MEDIA, szFilePath);
		goto err;
	}
	
	if ((m_FileMeta.m_pCodec = avcodec_find_decoder(m_FileMeta.m_pFormatCtx->streams[0]->codecpar->codec_id)) == NULL) {
		Errors.SetError(CPackErrors::PE_IO, szFilePath, GetErrorString(nRet));
		goto err;
	}

	// check codec
	if (!CheckCodecID(m_FileMeta.m_pCodec->id)) {
		Errors.SetError(CPackErrors::PE_UNSUPPORT_MEDIA, szFilePath);
		goto err;
	}



//	pParserCtx = av_parser_init(pCodec->id);

	if ((m_FileMeta.m_pCodecCtx = avcodec_alloc_context3(m_FileMeta.m_pCodec)) == NULL) {
		Errors.SetError(CPackErrors::PE_UNSUPPORT_MEDIA, szFilePath);
		goto err;
	}

	if ((nRet = avcodec_parameters_to_context(m_FileMeta.m_pCodecCtx, m_FileMeta.m_pFormatCtx->streams[0]->codecpar)) < 0) {
		Errors.SetError(CPackErrors::PE_UNSUPPORT_MEDIA, szFilePath);
		goto err;
	}

	if ((nRet = avcodec_open2(m_FileMeta.m_pCodecCtx, m_FileMeta.m_pCodec,NULL)) < 0) {
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

	// nb_frames has bug..
	if (!ProbeTotalFrames(m_FileMeta.m_pFormatCtx, m_FileMeta.m_pCodecCtx, szFilePath, m_FileMeta.m_TotalFrames, Errors)) {
		goto err;
	}

/*
	m_FileMeta.m_TotalFrames = m_FileMeta.m_pFormatCtx->streams[0]->nb_frames;
	if (nTotalFrames == 0) {
		if (!ProbeTotalFrames(pFormatCtx, pCodecCtx, szFilePath, nTotalFrames, Errors)) {
			goto err;
		}
	}
*/
	if (m_FileMeta.m_TotalFrames == 0) {
		Errors.SetError(CPackErrors::PE_UNSUPPORT_MEDIA, szFilePath);
		goto err;
	}

	nBitsPerSample = av_get_bits_per_sample(m_FileMeta.m_pCodec->id);
	if (nBitsPerSample == 0 && m_FileMeta.m_pCodecCtx->bits_per_raw_sample != 0) {
		nBitsPerSample = m_FileMeta.m_pCodecCtx->bits_per_raw_sample;
	} else if(nBitsPerSample == 0){
		Errors.SetError(CPackErrors::PE_UNSUPPORT_MEDIA, szFilePath);
		goto err;
	}

	// if codec is alac, and bit depth is 20 , DO NOT SUPPORT!
	if (m_FileMeta.m_pCodec->id == AV_CODEC_ID_ALAC && nBitsPerSample == 20) {
		Errors.SetError(CPackErrors::PE_UNSUPPORT_MEDIA, szFilePath);
		goto err;
	}

	// alloc buffer
	if (!CPCMAudioMedia::Alloc(m_FileMeta.m_TotalFrames,
		m_FileMeta.m_pCodecCtx->channels, nBitsPerSample, Errors)) {
		goto err;
	}

	nTotalFramesSave = m_FileMeta.m_TotalFrames;
	Progress.Reset(IDS_READ_FILE);
	Progress.SetFullScale(nTotalFramesSave);
	while (nTotalFramesSave) {
		if ((nRet = av_read_frame(m_FileMeta.m_pFormatCtx, input_packet)) == 0) {
			if ((nRet = avcodec_send_packet(m_FileMeta.m_pCodecCtx, input_packet)) == 0 ) {
				if ((nRet = avcodec_receive_frame(m_FileMeta.m_pCodecCtx, input_frame)) == 0) {
					CPCMAudioMedia::SetSample(
						input_frame->extended_data,
						m_FileMeta.m_TotalFrames - nTotalFramesSave,
						input_frame->linesize[0],
						(CPCMAudioMedia::SAMPLE_FORMAT_T)input_frame->format,
						input_frame->nb_samples,
						input_frame->channels);
					nTotalFramesSave -= (nTotalFramesSave >= input_frame -> nb_samples) ?
						input_frame->nb_samples : nTotalFramesSave;
					Progress.Increase((nTotalFramesSave >= input_frame->nb_samples) ?
						input_frame->nb_samples : nTotalFramesSave);
					if (Progress.IsCanceled(Errors)) {
						goto err;
					}
					TRACE(_T("%I64d:%I64d\n"), m_FileMeta.m_TotalFrames, nTotalFramesSave);
				}else {
					Errors.SetError(CPackErrors::PE_IO, szFilePath, GetErrorString(nRet));
					goto err;
				}
			}
			else {
				Errors.SetError(CPackErrors::PE_IO, szFilePath, GetErrorString(nRet));
				goto err;
			}
			if (nRet == AVERROR_EOF)
				break;
		} else {
			Errors.SetError(CPackErrors::PE_IO, szFilePath, GetErrorString(nRet));
			goto err;
		}
	}

	// test format
	if (!CBytePerBlockMedia::LoadMeta(PasswordGetter, Errors)) {
		goto err;
	}
	
	bRet = TRUE;
err:

	if (input_packet) {
		av_packet_free(&input_packet);
	}
	if (input_frame) {
		av_frame_free(&input_frame);
	}

	if (!bRet) {
		CloseMedia();
	}
	return bRet;
}

INT CMiscAudioMedia::EncodeFrame(AVFormatContext* pEncodeFormatCtx, 
	AVCodecContext* pEncodeCodecCtx, AVCodecContext* pDecodeCodecCtx, 
	AVFrame* pFrame, AVPacket* pPacket)
{
	INT nRet = 0;
	nRet = avcodec_send_frame(pEncodeCodecCtx, pFrame);
	while (nRet >= 0) {
		nRet = avcodec_receive_packet(pEncodeCodecCtx, pPacket);
		if (nRet == AVERROR(EAGAIN) || nRet == AVERROR_EOF)
			return 0;
		else if (nRet < 0) {
			fprintf(stderr, "Error encoding audio frame\n");
			return nRet;
		}

		pPacket->stream_index = 0;
		av_packet_rescale_ts(pPacket,
			pEncodeCodecCtx->time_base,
			pEncodeFormatCtx->streams[0]->time_base);
		nRet = av_interleaved_write_frame(pEncodeFormatCtx, pPacket);
		av_packet_unref(pPacket);
	}
	return nRet;
}

BOOL CMiscAudioMedia::SaveMedia(LPCTSTR szFilePath, CProgressBase& Progress, CPackErrors& Errors)
{
	CT2CA filepath(szFilePath);
	BOOL bRet = FALSE;
	ULONGLONG nTotalFramesSave;
	INT nRet;
	AVCodec* pCodec = NULL;
	AVFormatContext* pFormatCtx = NULL;
	AVStream* pOutStream = NULL;
	AVCodecContext* pCodecCtx = NULL;
	AVPacket* output_packet = NULL;
	AVFrame* output_frame = NULL;
	const AVDictionaryEntry* tag = NULL;

	if (!CBytePerBlockMedia::SaveMeta(Errors)) {
		goto err;
	}

	Progress.Reset(IDS_FILL_EMPTY_SPACE);
	Progress.SetFullScale(GetMediaTotalBytes() - GetMediaUsedBytes());

	if (!CBytePerBlockMedia::FillEmptySpace(Progress, Errors)) {
		goto err;
	}

	if ((nRet = avformat_alloc_output_context2(&pFormatCtx, NULL, NULL, filepath)) < 0) {
		Errors.SetError(CPackErrors::PE_NOMEM);
		goto err;
	}


	if ((pCodec = avcodec_find_encoder(m_FileMeta.m_pCodec->id)) == NULL) {
		Errors.SetError(CPackErrors::PE_INTERNAL);
		goto err;
	}

	if ((pOutStream = avformat_new_stream(pFormatCtx, pCodec)) == NULL) {
		Errors.SetError(CPackErrors::PE_NOMEM);
		goto err;
	}

	if ((pCodecCtx = avcodec_alloc_context3(pCodec)) == NULL) {
		Errors.SetError(CPackErrors::PE_NOMEM);
		goto err;
	}


	pCodecCtx->sample_rate = m_FileMeta.m_pCodecCtx->sample_rate;
	pCodecCtx->channel_layout = m_FileMeta.m_pCodecCtx->channel_layout;
	pCodecCtx->channels = m_FileMeta.m_pCodecCtx->channels;
	pCodecCtx->sample_fmt = m_FileMeta.m_pCodecCtx->sample_fmt;
	pCodecCtx->time_base = m_FileMeta.m_pCodecCtx->time_base;
	pCodecCtx->bits_per_raw_sample = m_FileMeta.m_pCodecCtx->bits_per_raw_sample;

	if (pFormatCtx->oformat->flags & AVFMT_GLOBALHEADER) {
		pCodecCtx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
	}

	
	if ((nRet = avcodec_open2(pCodecCtx, pCodec, NULL)) < 0) {
		CString str = GetErrorString(nRet);
		Errors.SetError(CPackErrors::PE_NOMEM);
		goto err;
	}

	if ((nRet = avcodec_parameters_from_context(pOutStream->codecpar, pCodecCtx)) < 0) {
		Errors.SetError(CPackErrors::PE_NOMEM);
		goto err;
	}

	pOutStream->time_base = pCodecCtx->time_base;

	if (m_FileMeta.m_pFormatCtx->metadata) {
		av_dict_copy(&pFormatCtx->metadata, m_FileMeta.m_pFormatCtx->metadata, AV_DICT_IGNORE_SUFFIX);
	}

	av_log_set_callback(CBLogger);
	av_dump_format(pFormatCtx, 0, filepath, 1);

	if (!(pFormatCtx->oformat->flags & AVFMT_NOFILE)) {
		nRet = avio_open(&pFormatCtx->pb, filepath, AVIO_FLAG_WRITE);
		if (nRet < 0) {
			Errors.SetError(CPackErrors::PE_IO, szFilePath, GetErrorString(nRet));
			goto err;
		}
	}

	nRet = avformat_write_header(pFormatCtx, NULL);
	if (nRet < 0) {
		Errors.SetError(CPackErrors::PE_IO, szFilePath, GetErrorString(nRet));
		goto err;
	}

	if ((output_packet = av_packet_alloc()) == NULL) {
		Errors.SetError(CPackErrors::PE_NOMEM);
		goto err;
	}
	if ((output_frame = av_frame_alloc()) == NULL) {
		Errors.SetError(CPackErrors::PE_NOMEM);
		goto err;
	}

	output_frame->nb_samples = pCodecCtx->frame_size;
	if (output_frame->nb_samples == 0) {
		output_frame->nb_samples = ONE_PASS_FRAMES;
	}
	output_frame->format = pCodecCtx->sample_fmt;
	output_frame->channels = pCodecCtx->channels;
	output_frame->channel_layout = pCodecCtx->channel_layout;
	output_frame->sample_rate = pCodecCtx->sample_rate;

	if ((nRet = av_frame_get_buffer(output_frame, 0)) < 0) {
		Errors.SetError(CPackErrors::PE_NOMEM);
		goto err;
	}

	nTotalFramesSave = m_FileMeta.m_TotalFrames;
	Progress.Reset(IDS_WRITE_FILE);
	Progress.SetFullScale(nTotalFramesSave);
	// very IMPORTANT!!!
	output_frame->pts = 0;

	while (nTotalFramesSave) {
		if ((nRet = av_frame_make_writable(output_frame)) < 0) {
			Errors.SetError(CPackErrors::PE_NOMEM);
			goto err;
		}
		CPCMAudioMedia::GetSample(
			output_frame->data,
			m_FileMeta.m_TotalFrames - nTotalFramesSave,
			output_frame->linesize[0],
			(CPCMAudioMedia::SAMPLE_FORMAT_T)output_frame->format,
			output_frame->nb_samples,
			output_frame->channels);

		if ((nRet = EncodeFrame(pFormatCtx, pCodecCtx, m_FileMeta.m_pCodecCtx, output_frame, output_packet)) < 0) {
			Errors.SetError(CPackErrors::PE_IO, szFilePath, GetErrorString(nRet));
			goto err;
		}

		Progress.Increase((nTotalFramesSave >= output_frame->nb_samples) ?
			output_frame->nb_samples : nTotalFramesSave);

		if (Progress.IsCanceled(Errors)) {
			goto err;
		}

		// very IMPORTANT!!!
		output_frame->pts += output_frame->nb_samples;

		nTotalFramesSave -= (nTotalFramesSave >= output_frame->nb_samples) ?
			output_frame->nb_samples : nTotalFramesSave;
	}

	/* flush codec */
	nRet = EncodeFrame(pFormatCtx, pCodecCtx, m_FileMeta.m_pCodecCtx, NULL, output_packet);
	if (nRet < 0) {
		Errors.SetError(CPackErrors::PE_IO, szFilePath, GetErrorString(nRet));
		goto err;
	}

	nRet = av_write_trailer(pFormatCtx);
	if (nRet < 0) {
		Errors.SetError(CPackErrors::PE_IO, szFilePath, GetErrorString(nRet));
		goto err;
	}

	// done!
	ClearMediaDirty();

	bRet = TRUE;
err:
	if (pFormatCtx && !(pFormatCtx->oformat->flags & AVFMT_NOFILE)) {
		avio_closep(&pFormatCtx->pb);
	}
	if (pFormatCtx && pFormatCtx->metadata) {
		av_dict_free(&pFormatCtx->metadata);
	}
	if (pFormatCtx) {
		avformat_free_context(pFormatCtx);
	}
	if (pCodecCtx) {
		avcodec_free_context(&pCodecCtx);
	}
	if (output_packet) {
		av_packet_free(&output_packet);
	}
	if (output_frame) {
		av_frame_free(&output_frame);
	}
	pOutStream = NULL;
	pCodec = NULL;
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
	m_OptPagePCMFileProperty.m_strPCMProperty = FillInfoStr();
	pPropertySheet->AddPage(&m_OptPagePCMFileProperty);
	CPCMAudioMedia::AddOptPage(pPropertySheet);
}

BOOL CMiscAudioMedia::UpdateOpts(CMFCPropertySheet* pPropertySheet)
{
	return CPCMAudioMedia::UpdateOpts(pPropertySheet);;
}

CMediaBase* CMiscAudioMedia::Factory()
{
	return new(std::nothrow) CMiscAudioMedia();
}

CMiscAudioMedia::MISC_AUDIO_EXT CMiscAudioMedia::m_szExtTable[] = {
	{_T("wav"),_T("Waveform Audio File Format") },
	{_T("aif,aiff,aifc"),_T("Audio Interchange File Format")},
	{_T("au,snd"),_T("NeXT/Sun")},
	{_T("m4a,m4r"),_T("Apple MPEG - 4 Audio")},
	{_T("caf"),_T("Core Audio Format")},
	{_T("flac"),_T("Free Lossless Audio Codec") },
	{_T("dts"), _T("Digital Theater Systems") },
	{_T("mlp"), _T("Meridian Lossless Packing") },
	{_T("ogg,oga"), _T("OGG Container Format") },
	{_T("wv"), _T("Wave Pack") },
	{_T("w64"), _T("Sony Wave64") },
	{_T("tta"), _T("True Audio") },
	{_T("oma,aa3"), _T("Sony OpenMG Audio") },
	{_T("thd"), _T("Muxer truehd") },
};

void CMiscAudioMedia::GetMediaInfo(CArray<CMediaFactory::CMediaInfo>& InfoArray)
{
	CMediaFactory::CMediaInfo Info;
	TCHAR szBuffer[1024];
	TCHAR* context = NULL;
	TCHAR* token;
	for (INT i = 0; i < _countof(m_szExtTable); i++) {
		context = NULL;
		Info.Exts.RemoveAll();
		_tcsncpy_s(szBuffer, m_szExtTable[i].szExts, lstrlen(m_szExtTable[i].szExts));
		szBuffer[_countof(szBuffer) - 1] = 0;
		token = _tcstok_s(szBuffer, _T(","), &context);
		while (token) {
			Info.Exts.Add(token);
			token = _tcstok_s(NULL, _T(","), &context);
		}
		Info.fnFactory = Factory;
		Info.nCatagory = IDS_MEDIA_AUDIO_FILE;
		Info.strName = m_szExtTable[i].szName;
		InfoArray.Add(Info);
	}
}





