#include "pch.h"
#include "CorImageMedia.h"

CCorImageMedia::CCorImageMedia()
{
}

CCorImageMedia::~CCorImageMedia()
{
}

BOOL CCorImageMedia::Alloc(UINT nWidth, UINT nHeight, CPackErrors& Error)
{
    return 0;
}

void CCorImageMedia::Free()
{
}

BYTE CCorImageMedia::GetByteFromBlocks(UINT nOffset, UINT nBlockPerByte)
{
    return 0;
}

void CCorImageMedia::SetByteToBlocks(BYTE nData, UINT nOffset, UINT nBlockPerByte)
{
}

UINT CCorImageMedia::GetTotalBlocks()
{
    return 0;
}

void CCorImageMedia::AddOptPage(CMFCPropertySheet* pPropertySheet)
{
}

BOOL CCorImageMedia::UpdateOpts(CMFCPropertySheet* pPropertySheet)
{
    return 0;
}
