#include "pch.h"
#include "FrameAudioMedia.h"

CFrameAudioMedia::CFrameAudioMedia() :
	m_bUseFileCache(FALSE)
{
}

CFrameAudioMedia::~CFrameAudioMedia()
{
}

BYTE CFrameAudioMedia::GetByteFromBlocks(ULONGLONG nOffset, UINT nBlockPerByte)
{
	return 0;
}

void CFrameAudioMedia::SetByteToBlocks(BYTE nData, ULONGLONG nOffset, UINT nBlockPerByte)
{
}

ULONGLONG CFrameAudioMedia::GetTotalBlocks()
{
	return ULONGLONG();
}

void CFrameAudioMedia::AddOptPage(CMFCPropertySheet* pPropertySheet)
{
}

BOOL CFrameAudioMedia::UpdateOpts(CMFCPropertySheet* pPropertySheet)
{
	return 0;
}

BOOL CFrameAudioMedia::Alloc(ULONGLONG nFrames, UINT nChannels, FRAME_FORMAT_T Fmt, CPackErrors& Error)
{
	return 0;
}

void CFrameAudioMedia::Free()
{

}

void CFrameAudioMedia::SetFrame(LPVOID pBuffer, ULONGLONG nFrameOffset, UINT nFrameCnt, UINT nChannel, FRAME_FORMAT_T Fmt)
{
}

void CFrameAudioMedia::GetFrame(LPVOID pBuffer, ULONGLONG nFrameOffset, UINT nFrameCnt, UINT nChannel, FRAME_FORMAT_T Fmt)
{
}
