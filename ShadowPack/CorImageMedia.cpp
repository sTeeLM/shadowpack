#include "pch.h"
#include "CorImageMedia.h"

CCorImageMedia::CCorImageMedia() :
    m_pHeightInBlocks(NULL),
    m_pWidthInBlocks(NULL),
    m_pCoeffBuffer(NULL),
    m_nComponents(0),
    m_nTotalCoeffs(0)
{
}

CCorImageMedia::~CCorImageMedia()
{
}

UINT CCorImageMedia::DivRoundup(UINT a, UINT b)
{
    UINT c = b - 1;
    return (a + c) / b;
}

BOOL CCorImageMedia::Alloc(UINT nWidth, UINT nHeight, UINT nComponents, CPackErrors& Error)
{
    USHORT max_v_samp_factor = 0;
    USHORT max_h_samp_factor = 0;

    m_nComponents = nComponents;

    m_pHeightInBlocks = new(std::nothrow) INT[nComponents];
    if (!m_pHeightInBlocks) {
        Error.SetError(CPackErrors::PE_NOMEM);
        goto err;
    }

    m_pWidthInBlocks = new(std::nothrow) INT[nComponents];
    if (!m_pWidthInBlocks) {
        Error.SetError(CPackErrors::PE_NOMEM);
        goto err;
    }

    m_pCoeffBuffer = new(std::nothrow) SHORT* [nComponents];
    if (!m_pCoeffBuffer) {
        Error.SetError(CPackErrors::PE_NOMEM);
        goto err;
    }
    ZeroMemory(m_pCoeffBuffer, sizeof(SHORT*) * nComponents);

    for (USHORT icomp = 0; icomp < nComponents; icomp++) {
        max_v_samp_factor = max_v_samp_factor > GetVSampleFactor(icomp) ?
            max_v_samp_factor : GetVSampleFactor(icomp);
        max_h_samp_factor = max_h_samp_factor > GetHSampleFactor(icomp) ?
            max_h_samp_factor : GetHSampleFactor(icomp);
    }

    for (USHORT icomp = 0; icomp < nComponents; icomp++) {
        m_pHeightInBlocks[icomp] = DivRoundup(nHeight * GetVSampleFactor(icomp),
            8 * max_v_samp_factor);
        m_pWidthInBlocks[icomp] = DivRoundup(nWidth * GetHSampleFactor(icomp),
            8 * max_h_samp_factor);
        TRACE(_T("m_pHeightInBlocks[%d] = %d, m_pWidthInBlocks[%d] = %d\n"),
            icomp, m_pHeightInBlocks[icomp], icomp, m_pWidthInBlocks[icomp]);
    }

    for (USHORT icomp = 0; icomp < nComponents; icomp++) {
        m_pCoeffBuffer[icomp] = new(std::nothrow) SHORT[m_pHeightInBlocks[icomp] * m_pWidthInBlocks[icomp] * GetCoeffPerBlock()];
        if (!m_pCoeffBuffer[icomp]) {
            Error.SetError(CPackErrors::PE_NOMEM);
            goto err;
        }
    }
    m_nTotalCoeffs = 0;
    for (USHORT icomp = 0; icomp < nComponents; icomp++) {
        m_nTotalCoeffs += GetCoeffPerBlock() * (m_pHeightInBlocks[icomp] * m_pWidthInBlocks[icomp]);
    }
    return TRUE;
err:
    Free();
    return FALSE;
}

void CCorImageMedia::Free()
{

    m_CoeffIndex.RemoveAll();

    if (m_pHeightInBlocks) {
        delete[] m_pHeightInBlocks;
        m_pHeightInBlocks = NULL;
    }
    if (m_pWidthInBlocks) {
        delete[] m_pWidthInBlocks;
        m_pWidthInBlocks = NULL;
    }
    if (m_pCoeffBuffer) {
        for (USHORT icomp = 0; icomp < m_nComponents; icomp++) {
            if (m_pCoeffBuffer[icomp]) {
                delete[] m_pCoeffBuffer[icomp];
                m_pCoeffBuffer[icomp] = NULL;
            }
        }
        delete[] m_pCoeffBuffer;
        m_pCoeffBuffer = NULL;
    }
    m_nComponents = 0;
    m_nTotalCoeffs = 0;
}

UINT CCorImageMedia::GetTotalCoeffs()
{
    return m_nTotalCoeffs;
}

UINT CCorImageMedia::GetHeightInBlocks(UINT nComponents)
{
    ASSERT(m_pHeightInBlocks);
    return m_pHeightInBlocks ? m_pHeightInBlocks[nComponents] : 0;
}

UINT CCorImageMedia::GetWidthInBlocks(UINT nComponents)
{
    ASSERT(m_pWidthInBlocks);
    return m_pWidthInBlocks ? m_pWidthInBlocks[nComponents] : 0;
}

UINT CCorImageMedia::GetCoeffPerBlock()
{
    return 64;
}

void CCorImageMedia::AddCoeff(UINT nComponents, UINT nX, UINT nY, UINT nCoeff, SHORT nData)
{
    ASSERT(m_pCoeffBuffer);
    ASSERT(nX < GetWidthInBlocks(nComponents));
    ASSERT(nY < GetHeightInBlocks(nComponents));
    ASSERT(nCoeff < GetCoeffPerBlock());
    if (m_pCoeffBuffer) {
        if (nX < GetWidthInBlocks(nComponents) && nY < GetHeightInBlocks(nComponents)
            && nCoeff < GetCoeffPerBlock()) {
            m_pCoeffBuffer[nComponents][
                (nY * GetWidthInBlocks(nComponents) + nX) * GetCoeffPerBlock() + nCoeff
            ] = nData;
            if (nData) {
                m_CoeffIndex.Add(CCoeffIndex(nComponents, nX, nY, nCoeff));
            }
        }
    }
}

SHORT CCorImageMedia::GetCoeff(UINT nComponents, UINT nX, UINT nY, UINT nCoeff)
{
    ASSERT(m_pCoeffBuffer);
    ASSERT(nX < GetWidthInBlocks(nComponents));
    ASSERT(nY < GetHeightInBlocks(nComponents));
    ASSERT(nCoeff < GetCoeffPerBlock());
    if (m_pCoeffBuffer) {
        if (nX < GetWidthInBlocks(nComponents) && nY < GetHeightInBlocks(nComponents)
            && nCoeff < GetCoeffPerBlock()) {
            return m_pCoeffBuffer[nComponents][
                (nY * GetWidthInBlocks(nComponents) + nX) * GetCoeffPerBlock() + nCoeff
            ];
        }
    }
    return 0;
}

SHORT& CCorImageMedia::GetCorFromIndex(UINT nIndex)
{

}

BYTE CCorImageMedia::GetByteFromBlocks(UINT nOffset, UINT nBlockPerByte)
{
    BYTE nRet = 0;
    ASSERT(nBlockPerByte <= MAX_BPB_MEDIA_BPB_SIZE && nBlockPerByte >= MIN_BPB_MEDIA_BPB_SIZE);
    if (nBlockPerByte == 1) { /* 1 byte in 2 cor, 4 | 4 */
        for (INT i = 0; i < 2; i++) {
            nRet <<= 4;
            nRet |= (GetCorFromIndex(nOffset * 4) + i) & 0xF;
        }
    } else if (nBlockPerByte == 2) {  /* 1 byte in 4 cor, 2 | 2 | 2 | 2 */
        for (INT i = 0; i < 4; i++) {
            nRet <<= 2;
            nRet |= (GetCorFromIndex(nOffset * 4) + i) & 0x3;
        }
    } else if (nBlockPerByte == 3) {  /* 1 byte in 6 cor, 2 | 1 | 2 | 1 | 1 | 1 */
        nRet = (GetCorFromIndex(nOffset * 6)) & 0x3;
        nRet <<= 2;

        nRet |= (GetCorFromIndex(nOffset * 6) + 1) & 0x1;
        nRet <<= 1;

        nRet |= (GetCorFromIndex(nOffset * 6) + 2) & 0x3;
        nRet <<= 2;

        nRet |= (GetCorFromIndex(nOffset * 6) + 3) & 0x1;
        nRet <<= 1;

        nRet |= (GetCorFromIndex(nOffset * 6) + 4) & 0x1;
        nRet <<= 1;

        nRet |= (GetCorFromIndex(nOffset * 6) + 5) & 0x1;

    } else if (nBlockPerByte == 4) {  /* 1 byte in 8 cor, 1 | 1 | 1 | 1 | 1 | 1 | 1 | 1 */
        for (INT i = 0; i < 8; i++) {
            nRet <<= 1;
            nRet |= (GetCorFromIndex(nOffset * 8) + i) & 0x1;
        }
        
    }
}

void CCorImageMedia::SetByteToBlocks(BYTE nData, UINT nOffset, UINT nBlockPerByte)
{
    BYTE nRet = 0;
    ASSERT(nBlockPerByte <= MAX_BPB_MEDIA_BPB_SIZE && nBlockPerByte >= MIN_BPB_MEDIA_BPB_SIZE);
    if (nBlockPerByte == 1) { /* 1 byte in 2 cor, 4 | 4 */
        for (INT i = 0; i < 2; i++) {
            nRet <<= 4;
            nRet |= (GetCorFromIndex(nOffset * 4) + i) & 0xF;
        }
    }
    else if (nBlockPerByte == 2) {  /* 1 byte in 4 cor, 2 | 2 | 2 | 2 */
        for (INT i = 0; i < 4; i++) {
            nRet <<= 2;
            nRet |= (GetCorFromIndex(nOffset * 4) + i) & 0x3;
        }
    }
    else if (nBlockPerByte == 3) {  /* 1 byte in 6 cor, 2 | 1 | 2 | 1 | 1 | 1 */
        nRet = (GetCorFromIndex(nOffset * 6)) & 0x3;
        nRet <<= 2;

        nRet |= (GetCorFromIndex(nOffset * 6) + 1) & 0x1;
        nRet <<= 1;

        nRet |= (GetCorFromIndex(nOffset * 6) + 2) & 0x3;
        nRet <<= 2;

        nRet |= (GetCorFromIndex(nOffset * 6) + 3) & 0x1;
        nRet <<= 1;

        nRet |= (GetCorFromIndex(nOffset * 6) + 4) & 0x1;
        nRet <<= 1;

        nRet |= (GetCorFromIndex(nOffset * 6) + 5) & 0x1;

    }
    else if (nBlockPerByte == 4) {  /* 1 byte in 8 cor, 1 | 1 | 1 | 1 | 1 | 1 | 1 | 1 */
        for (INT i = 0; i < 8; i++) {
            nRet <<= 1;
            nRet |= (GetCorFromIndex(nOffset * 8) + i) & 0x1;
        }

    }
}

UINT CCorImageMedia::GetTotalBlocks()
{
    return m_CoeffIndex.GetCount() / (m_Header.dwBPBBlockPerByte * 2);
}

void CCorImageMedia::AddOptPage(CMFCPropertySheet* pPropertySheet)
{
}

BOOL CCorImageMedia::UpdateOpts(CMFCPropertySheet* pPropertySheet)
{
    return 0;
}

