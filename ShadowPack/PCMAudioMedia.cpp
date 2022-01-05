#include "pch.h"
#include "PCMAudioMedia.h"

CPCMAudioMedia::CPCMAudioMedia() :
	m_bUseFileCache(FALSE)
{
}

CPCMAudioMedia::~CPCMAudioMedia()
{
}

BYTE CPCMAudioMedia::GetByteFromBlocks(ULONGLONG nOffset, UINT nBlockPerByte)
{
	return 0;
}

void CPCMAudioMedia::SetByteToBlocks(BYTE nData, ULONGLONG nOffset, UINT nBlockPerByte)
{
}

ULONGLONG CPCMAudioMedia::GetTotalBlocks()
{
	return ULONGLONG();
}

void CPCMAudioMedia::AddOptPage(CMFCPropertySheet* pPropertySheet)
{
}

BOOL CPCMAudioMedia::UpdateOpts(CMFCPropertySheet* pPropertySheet)
{
	return 0;
}

BOOL CPCMAudioMedia::Alloc(ULONGLONG nFrames, UINT nChannels, UINT nBitsPerSample, CPackErrors& Error)
{
	return TRUE;
}

void CPCMAudioMedia::Free()
{

}

void CPCMAudioMedia::SetFrame(LPVOID pBuffer, ULONGLONG nFrameOffset, UINT nFrameCnt)
{
}

void CPCMAudioMedia::GetFrame(LPVOID pBuffer, ULONGLONG nFrameOffset, UINT nFrameCnt)
{
}
