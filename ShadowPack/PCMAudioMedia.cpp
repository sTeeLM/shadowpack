#include "pch.h"
#include "PCMAudioMedia.h"

CPCMAudioMedia::CPCMAudioMedia() :
	m_bUseFileCache(FALSE),
	m_pSampleBuffer(NULL),
	m_nBitsPerSample(0),
	m_nSampleCnt(0),
	m_nChannels(0)
{
}

CPCMAudioMedia::~CPCMAudioMedia()
{

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
	ULONGLONG nSize = nBitsPerSample;
	BOOL bRet;

	if (nSize < 32)
		nSize = 4;
	else
		nSize = nBitsPerSample / 8;

	nSize *= (nFrames * nChannels);

	if (nSize >= 4294967296L) {
		Error.SetError(CPackErrors::PE_NOMEM);
		goto err;
	}

	if ((m_pSampleBuffer = (LPBYTE)malloc(nSize)) == NULL) {
		Error.SetError(CPackErrors::PE_NOMEM);
		goto err;
	}
	m_nSampleCnt = nFrames * nChannels;
	m_nBitsPerSample = nBitsPerSample;
	m_nChannels = nChannels;
	bRet = TRUE;
err:
	return bRet;
}

void CPCMAudioMedia::Free()
{
	if (m_pSampleBuffer) {
		free(m_pSampleBuffer);
		m_pSampleBuffer = NULL;
	}
	m_nBitsPerSample = 0;
	m_nSampleCnt = 0;
	m_nChannels = 0;
}

void CPCMAudioMedia::SetFrame(LPVOID pBuffer, ULONGLONG nFrameOffset, UINT nFrameCnt)
{
	ULONGLONG nOffset;
	UINT nSize;

	if (m_nBitsPerSample <= 32) {
		nOffset = nFrameOffset * m_nChannels * 4;
		nSize = nFrameCnt * m_nChannels * 4;
	}
	else {
		nOffset = nFrameOffset * m_nChannels * (m_nBitsPerSample / 8);
		nSize = nFrameCnt * m_nChannels * (m_nBitsPerSample / 8);
	}

	memcpy(m_pSampleBuffer + nFrameOffset, pBuffer, nSize);
}

void CPCMAudioMedia::GetFrame(LPVOID pBuffer, ULONGLONG nFrameOffset, UINT nFrameCnt)
{
	ULONGLONG nOffset;
	UINT nSize;

	if (m_nBitsPerSample <= 32) {
		nOffset = nFrameOffset * m_nChannels * 4;
		nSize = nFrameCnt * m_nChannels * 4;
	}
	else {
		nOffset = nFrameOffset * m_nChannels * (m_nBitsPerSample / 8);
		nSize = nFrameCnt * m_nChannels * (m_nBitsPerSample / 8);
	}

	memcpy(pBuffer, m_pSampleBuffer + nFrameOffset, nSize);
}

/*
m_nBitsPerSample:
8  : m_nSampleCnt / 3;
16 : m_nSampleCnt / 2;
20 : m_nSampleCnt / 2;
24 : m_nSampleCnt
32 : m_nSampleCnt
64 : m_nSampleCnt
*/

ULONGLONG CPCMAudioMedia::GetTotalBlocks()
{
	if (m_nBitsPerSample == 8) {
		return m_nSampleCnt / 3;
	}
	else if (m_nBitsPerSample == 16 || m_nBitsPerSample == 20) {
		return m_nSampleCnt / 2;
	}
	else {
		return m_nSampleCnt;
	}
}

BYTE CPCMAudioMedia::GetByteFromBlocks(ULONGLONG nBlockOffset, UINT nBlockPerByte)
{
	return 0;
}

void CPCMAudioMedia::SetByteToBlocks(BYTE nData, ULONGLONG nBlockOffset, UINT nBlockPerByte)
{

}
