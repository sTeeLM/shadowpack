#include "All.h"
#include "WAVInputSource.h"
#include "IO.h"
#include "MACLib.h"
#include "GlobalFunctions.h"

namespace APE
{

struct RIFF_HEADER 
{
    char cRIFF[4];          // the characters 'RIFF' indicating that it's a RIFF file
    uint32 nBytes;          // the number of bytes following this header
};

struct DATA_TYPE_ID_HEADER 
{
    char cDataTypeID[4];      // should equal 'WAVE' for a WAV file
};

struct WAV_FORMAT_HEADER
{
    uint16 nFormatTag;            // the format of the WAV...should equal 1 for a PCM file
    uint16 nChannels;             // the number of channels
    uint32 nSamplesPerSecond;     // the number of samples per second
    uint32 nBytesPerSecond;       // the bytes per second
    uint16 nBlockAlign;           // block alignment
    uint16 nBitsPerSample;        // the number of bits per sample
};

struct RIFF_CHUNK_HEADER
{
    char cChunkLabel[4];      // should equal "data" indicating the data chunk
    uint32 nChunkBytes;       // the bytes of the chunk  
};


CInputSource * CreateInputSource(const wchar_t * pSourceName, WAVEFORMATEX * pwfeSource, int64 * pTotalBlocks, int64 * pHeaderBytes, int64 * pTerminatingBytes, int32 * pFlags, int * pErrorCode)
{ 
    // error check the parameters
    if ((pSourceName == NULL) || (wcslen(pSourceName) == 0))
    {
        if (pErrorCode) *pErrorCode = ERROR_BAD_PARAMETER;
        return NULL;
    }

    // get the extension
    const wchar_t * pExtension = &pSourceName[wcslen(pSourceName)];
    while ((pExtension > pSourceName) && (*pExtension != '.'))
        pExtension--;

    // create the proper input source
    if (StringIsEqual(pExtension, L".wav", false))
    {
        if (pErrorCode) *pErrorCode = ERROR_SUCCESS;
        return new CWAVInputSource(pSourceName, pwfeSource, pTotalBlocks, pHeaderBytes, pTerminatingBytes, pErrorCode);
    }
    else if (StringIsEqual(pExtension, L".aiff", false) || StringIsEqual(pExtension, L".aif", false))
    {
        if (pErrorCode) *pErrorCode = ERROR_SUCCESS;
        *pFlags |= MAC_FORMAT_FLAG_AIFF | MAC_FORMAT_FLAG_BIG_ENDIAN;
        return new CAIFFInputSource(pSourceName, pwfeSource, pTotalBlocks, pHeaderBytes, pTerminatingBytes, pErrorCode);
    }
    else if (StringIsEqual(pExtension, L".w64", false))
    {
        if (pErrorCode) *pErrorCode = ERROR_SUCCESS;
        *pFlags |= MAC_FORMAT_FLAG_W64;
        return new CW64InputSource(pSourceName, pwfeSource, pTotalBlocks, pHeaderBytes, pTerminatingBytes, pErrorCode);
    }
    else if (StringIsEqual(pExtension, L".snd", false) || StringIsEqual(pExtension, L".au", false))
    {
        if (pErrorCode) *pErrorCode = ERROR_SUCCESS;
        return new CSNDInputSource(pSourceName, pwfeSource, pTotalBlocks, pHeaderBytes, pTerminatingBytes, pErrorCode, pFlags);
    }
    else
    {
        if (pErrorCode) *pErrorCode = ERROR_INVALID_INPUT_FILE;
        return NULL;
    }
}

/*************************************************************************************
CWAVInputSource - wraps working with WAV files
*************************************************************************************/
CWAVInputSource::CWAVInputSource(const wchar_t * pSourceName, WAVEFORMATEX * pwfeSource, int64 * pTotalBlocks, int64 * pHeaderBytes, int64 * pTerminatingBytes, int * pErrorCode)
    : CInputSource(pSourceName, pwfeSource, pTotalBlocks, pHeaderBytes, pTerminatingBytes, pErrorCode)
{
    m_bIsValid = false;

    if (pSourceName == NULL || pwfeSource == NULL)
    {
        if (pErrorCode) *pErrorCode = ERROR_BAD_PARAMETER;
        return;
    }
    
    m_spIO.Assign(CreateCIO());
    if (m_spIO->Open(pSourceName, true) != ERROR_SUCCESS)
    {
        m_spIO.Delete();
        if (pErrorCode) *pErrorCode = ERROR_INVALID_INPUT_FILE;
        return;
    }

    int nResult = AnalyzeSource();
    if (nResult == ERROR_SUCCESS)
    {
        // fill in the parameters
        if (pwfeSource) memcpy(pwfeSource, &m_wfeSource, sizeof(WAVEFORMATEX));
        if (pTotalBlocks) *pTotalBlocks = m_nDataBytes / int64(m_wfeSource.nBlockAlign);
        if (pHeaderBytes) *pHeaderBytes = m_nHeaderBytes;
        if (pTerminatingBytes) *pTerminatingBytes = m_nTerminatingBytes;

        m_bIsValid = true;
    }
    
    if (pErrorCode) *pErrorCode = nResult;
}

CWAVInputSource::~CWAVInputSource()
{
}

int CWAVInputSource::AnalyzeSource()
{
    // seek to the beginning (just in case)
    m_spIO->SetSeekMethod(APE_FILE_BEGIN);
    m_spIO->SetSeekPosition(0);
    m_spIO->PerformSeek();
    
    // get the file size
    m_nFileBytes = m_spIO->GetSize();

    // get the RIFF header
    RIFF_HEADER RIFFHeader;
    RETURN_ON_ERROR(ReadSafe(m_spIO, &RIFFHeader, sizeof(RIFFHeader))) 

    // make sure the RIFF header is valid
    if (!(RIFFHeader.cRIFF[0] == 'R' && RIFFHeader.cRIFF[1] == 'I' && RIFFHeader.cRIFF[2] == 'F' && RIFFHeader.cRIFF[3] == 'F')) 
        return ERROR_INVALID_INPUT_FILE;

    // read the data type header
    DATA_TYPE_ID_HEADER DataTypeIDHeader;
    RETURN_ON_ERROR(ReadSafe(m_spIO, &DataTypeIDHeader, sizeof(DataTypeIDHeader))) 
    
    // make sure it's the right data type
    if (!(DataTypeIDHeader.cDataTypeID[0] == 'W' && DataTypeIDHeader.cDataTypeID[1] == 'A' && DataTypeIDHeader.cDataTypeID[2] == 'V' && DataTypeIDHeader.cDataTypeID[3] == 'E')) 
        return ERROR_INVALID_INPUT_FILE;

    // find the 'fmt ' chunk
    RIFF_CHUNK_HEADER RIFFChunkHeader;
    RETURN_ON_ERROR(ReadSafe(m_spIO, &RIFFChunkHeader, sizeof(RIFFChunkHeader))) 
    
    while (!(RIFFChunkHeader.cChunkLabel[0] == 'f' && RIFFChunkHeader.cChunkLabel[1] == 'm' && RIFFChunkHeader.cChunkLabel[2] == 't' && RIFFChunkHeader.cChunkLabel[3] == ' ')) 
    {
        // move the file pointer to the end of this chunk
        m_spIO->SetSeekMethod(APE_FILE_CURRENT);
        m_spIO->SetSeekPosition(RIFFChunkHeader.nChunkBytes);
        m_spIO->PerformSeek();

        // check again for the data chunk
        RETURN_ON_ERROR(ReadSafe(m_spIO, &RIFFChunkHeader, sizeof(RIFFChunkHeader))) 
    }
    
    // read the format info
    WAV_FORMAT_HEADER WAVFormatHeader;
    RETURN_ON_ERROR(ReadSafe(m_spIO, &WAVFormatHeader, sizeof(WAVFormatHeader))) 

    // error check the header to see if we support it
    if ((WAVFormatHeader.nFormatTag != WAVE_FORMAT_PCM) && (WAVFormatHeader.nFormatTag != WAVE_FORMAT_EXTENSIBLE))
        return ERROR_INVALID_INPUT_FILE;

    // copy the format information to the WAVEFORMATEX passed in
    FillWaveFormatEx(&m_wfeSource, WAVFormatHeader.nFormatTag, WAVFormatHeader.nSamplesPerSecond, WAVFormatHeader.nBitsPerSample, WAVFormatHeader.nChannels);

    // skip over any extra data in the header
    int nWAVFormatHeaderExtra = RIFFChunkHeader.nChunkBytes - sizeof(WAVFormatHeader);
    if (nWAVFormatHeaderExtra < 0)
        return ERROR_INVALID_INPUT_FILE;
    else
    {
        m_spIO->SetSeekMethod(APE_FILE_CURRENT);
        m_spIO->SetSeekPosition(nWAVFormatHeaderExtra);
        m_spIO->PerformSeek();
    }
    
    // find the data chunk
    RETURN_ON_ERROR(ReadSafe(m_spIO, &RIFFChunkHeader, sizeof(RIFFChunkHeader))) 

    while (!(RIFFChunkHeader.cChunkLabel[0] == 'd' && RIFFChunkHeader.cChunkLabel[1] == 'a' && RIFFChunkHeader.cChunkLabel[2] == 't' && RIFFChunkHeader.cChunkLabel[3] == 'a')) 
    {
        // move the file pointer to the end of this chunk
        m_spIO->SetSeekMethod(APE_FILE_CURRENT);
        m_spIO->SetSeekPosition(RIFFChunkHeader.nChunkBytes);
        m_spIO->PerformSeek();

        // check again for the data chunk
        RETURN_ON_ERROR(ReadSafe(m_spIO, &RIFFChunkHeader, sizeof(RIFFChunkHeader))) 
    }

    // we're at the data block
    m_nHeaderBytes = m_spIO->GetPosition();
    m_nDataBytes = RIFFChunkHeader.nChunkBytes;
    if (m_nDataBytes > (m_nFileBytes - m_nHeaderBytes))
        m_nDataBytes = m_nFileBytes - m_nHeaderBytes;

    // cap at 4 GB (since file offsets are stored with 32-bit numbers)
    if (m_nDataBytes > (4 * BYTES_IN_GIGABYTE))
        return ERROR_INPUT_FILE_TOO_LARGE;

    // make sure the data bytes is a whole number of blocks
    if ((m_nDataBytes % m_wfeSource.nBlockAlign) != 0)
        return ERROR_INVALID_INPUT_FILE;

    // calculate the terminating byts
    m_nTerminatingBytes = m_nFileBytes - m_nDataBytes - m_nHeaderBytes;
    
    // we made it this far, everything must be cool
    return ERROR_SUCCESS;
}

int CWAVInputSource::GetData(unsigned char * pBuffer, int nBlocks, int * pBlocksRetrieved)
{
    if (!m_bIsValid) return ERROR_UNDEFINED;

    int nBytes = (m_wfeSource.nBlockAlign * nBlocks);
    unsigned int nBytesRead = 0;

    if (m_spIO->Read(pBuffer, nBytes, &nBytesRead) != ERROR_SUCCESS)
        return ERROR_IO_READ;

    if (pBlocksRetrieved) *pBlocksRetrieved = (nBytesRead / m_wfeSource.nBlockAlign);

    return ERROR_SUCCESS;
}

int CWAVInputSource::GetHeaderData(unsigned char * pBuffer)
{
    if (!m_bIsValid) return ERROR_UNDEFINED;

    int nResult = ERROR_SUCCESS;

    if (m_nHeaderBytes > 0)
    {
        int64 nOriginalFileLocation = m_spIO->GetPosition();

        m_spIO->SetSeekMethod(APE_FILE_BEGIN);
        m_spIO->SetSeekPosition(0);
        m_spIO->PerformSeek();
        
        unsigned int nBytesRead = 0;
        int nReadRetVal = m_spIO->Read(pBuffer, uint32(m_nHeaderBytes), &nBytesRead);

        if ((nReadRetVal != ERROR_SUCCESS) || (m_nHeaderBytes != int(nBytesRead)))
        {
            nResult = ERROR_UNDEFINED;
        }

        m_spIO->SetSeekMethod(APE_FILE_BEGIN);
        m_spIO->SetSeekPosition(nOriginalFileLocation);
        m_spIO->PerformSeek();
    }

    return nResult;
}

int CWAVInputSource::GetTerminatingData(unsigned char * pBuffer)
{
    if (!m_bIsValid) return ERROR_UNDEFINED;

    int nResult = ERROR_SUCCESS;

    if (m_nTerminatingBytes > 0)
    {
        int64 nOriginalFileLocation = m_spIO->GetPosition();

        m_spIO->SetSeekMethod(APE_FILE_END);
        m_spIO->SetSeekPosition(-m_nTerminatingBytes);
        m_spIO->PerformSeek();
        
        unsigned int nBytesRead = 0;
        int nReadRetVal = m_spIO->Read(pBuffer, uint32(m_nTerminatingBytes), &nBytesRead);

        if ((nReadRetVal != ERROR_SUCCESS) || (m_nTerminatingBytes != int(nBytesRead)))
        {
            nResult = ERROR_UNDEFINED;
        }

        m_spIO->SetSeekMethod(APE_FILE_BEGIN);
        m_spIO->SetSeekPosition(nOriginalFileLocation);
        m_spIO->PerformSeek();
    }

    return nResult;
}

/*************************************************************************************
CAIFFInputSource - wraps working with AIFF files
*************************************************************************************/
CAIFFInputSource::CAIFFInputSource(const wchar_t * pSourceName, WAVEFORMATEX * pwfeSource, int64 * pTotalBlocks, int64 * pHeaderBytes, int64 * pTerminatingBytes, int * pErrorCode)
    : CInputSource(pSourceName, pwfeSource, pTotalBlocks, pHeaderBytes, pTerminatingBytes, pErrorCode)
{
    m_bIsValid = false;

    if (pSourceName == NULL || pwfeSource == NULL)
    {
        if (pErrorCode) *pErrorCode = ERROR_BAD_PARAMETER;
        return;
    }
    
    m_spIO.Assign(CreateCIO());
    if (m_spIO->Open(pSourceName, true) != ERROR_SUCCESS)
    {
        m_spIO.Delete();
        if (pErrorCode) *pErrorCode = ERROR_INVALID_INPUT_FILE;
        return;
    }

    int nResult = AnalyzeSource();
    if (nResult == ERROR_SUCCESS)
    {
        // fill in the parameters
        if (pwfeSource) memcpy(pwfeSource, &m_wfeSource, sizeof(WAVEFORMATEX));
        if (pTotalBlocks) *pTotalBlocks = m_nDataBytes / int64(m_wfeSource.nBlockAlign);
        if (pHeaderBytes) *pHeaderBytes = m_nHeaderBytes;
        if (pTerminatingBytes) *pTerminatingBytes = m_nTerminatingBytes;

        m_bIsValid = true;
    }
    
    if (pErrorCode) *pErrorCode = nResult;
}

CAIFFInputSource::~CAIFFInputSource()
{
}

int CAIFFInputSource::AnalyzeSource()
{
    // analyze AIFF header
    //
    // header has 54 bytes
    //    FORM                        - 4 bytes        "FORM"
    //      Size                        - 4                size of all data, excluding the top 8 bytes
    //      AIFF                        - 4                "AIFF"
    //        COMM                    - 4                "COMM"
    //          size                    - 4                size of COMM chunk excluding the 8 bytes for "COMM" and size, should be 18
    //            Channels            - 2                number of channels
    //            sampleFrames        - 4                number of frames
    //            sampleSize            - 2                size of each sample
    //            sampleRate            - 10            samples per second
    //        SSND                    - 4                "SSND"
    //          size                    - 4                size of all data in the chunk, excluding "SSND" and size field
    //            BlockAlign            - 4                normally set to 0
    //            Offset                - 4                normally set to 0
    //            Audio data follows

    // seek to the beginning (just in case)
    m_spIO->SetSeekMethod(APE_FILE_BEGIN);
    m_spIO->SetSeekPosition(0);
    m_spIO->PerformSeek();

    // get the file size
    m_nFileBytes = m_spIO->GetSize();

    // cap at 4 GB (since file offsets are stored with 32-bit numbers)
    if (m_nFileBytes > (4 * BYTES_IN_GIGABYTE))
        return ERROR_INPUT_FILE_TOO_LARGE;

    // get the RIFF header
    RIFF_HEADER RIFFHeader;
    RETURN_ON_ERROR(ReadSafe(m_spIO, &RIFFHeader, sizeof(RIFFHeader)))
    FlipLong(&RIFFHeader.nBytes);

    // make sure the RIFF header is valid
    if (!(RIFFHeader.cRIFF[0] == 'F' && RIFFHeader.cRIFF[1] == 'O' && RIFFHeader.cRIFF[2] == 'R' && RIFFHeader.cRIFF[3] == 'M'))
        return ERROR_INVALID_INPUT_FILE;
    if (RIFFHeader.nBytes != (m_nFileBytes - sizeof(RIFF_HEADER)))
        return ERROR_INVALID_INPUT_FILE;

    // read the AIFF header
    #pragma pack(push)
    #pragma pack(1)
    struct AIFF_HEADER
    {
        char cAIFF[4];
        char cCOMM[4];
        int32 nSize;
        int16 nChannels;
        uint32 nFrames;
        int16 nSampleSize;
        char cSampleRate[6];
        char cExtra[4];
    };
    #pragma pack(pop)

    AIFF_HEADER AIFFHeader;
    RETURN_ON_ERROR(ReadSafe(m_spIO, &AIFFHeader, sizeof(AIFFHeader)))
    FlipLong(&AIFFHeader.nSize);
    FlipShort(&AIFFHeader.nChannels);
    FlipLong(&AIFFHeader.nFrames);
    FlipShort(&AIFFHeader.nSampleSize);
    uint32 nSampleRate = IEEE754ExtendedFloatToUINT32((unsigned char *) &AIFFHeader.cSampleRate[0]);

    // only support AIFF
    if ((AIFFHeader.cAIFF[0] != 'A') ||
        (AIFFHeader.cAIFF[1] != 'I') ||
        (AIFFHeader.cAIFF[2] != 'F') ||
        (AIFFHeader.cAIFF[3] != 'F'))
    {
        return ERROR_INVALID_INPUT_FILE;
    }

    // only support 16-bit and 24-bit
    if ((AIFFHeader.nSampleSize != 16) && (AIFFHeader.nSampleSize != 24))
        return ERROR_INVALID_INPUT_FILE;

    m_nDataBytes = -1;
    while (true)
    {
        struct GenericRIFFChunkHeaderStruct
        {
            char            cChunkLabel[4];            // the label of the chunk (PCM data = 'data')
            uint32            nChunkLength;            // the length of the chunk
        };
        GenericRIFFChunkHeaderStruct Generic;
        RETURN_ON_ERROR(ReadSafe(m_spIO, &Generic, sizeof(Generic)))
        FlipLong(&Generic.nChunkLength);

        if ((Generic.cChunkLabel[0] == 'S') && (Generic.cChunkLabel[1] == 'S') && (Generic.cChunkLabel[2] == 'N') && (Generic.cChunkLabel[3] == 'D'))
        {
            // read the SSND header
            struct SSNDHeader
            {
                uint32 offset;
                uint32 blocksize;
            };
            SSNDHeader Header;
            RETURN_ON_ERROR(ReadSafe(m_spIO, &Header, sizeof(Header)))
            m_nDataBytes = Generic.nChunkLength - 8;

            // check the size
            if ((m_nDataBytes / AIFFHeader.nFrames) != (AIFFHeader.nSampleSize * AIFFHeader.nChannels / 8))
                return ERROR_INVALID_INPUT_FILE;
            break;
        }
        m_spIO->SetSeekMethod(APE_FILE_CURRENT);
        m_spIO->SetSeekPosition(Generic.nChunkLength);
        m_spIO->PerformSeek();
    }

    // make sure we found the SSND header
    if (m_nDataBytes < 0)
        return ERROR_INVALID_INPUT_FILE;
    
    // copy the format information to the WAVEFORMATEX passed in
    FillWaveFormatEx(&m_wfeSource, WAVE_FORMAT_PCM, nSampleRate, AIFFHeader.nSampleSize, AIFFHeader.nChannels);

    // calculate the header and terminating data
    m_nHeaderBytes = m_spIO->GetPosition();
    m_nTerminatingBytes = m_nFileBytes - (m_nHeaderBytes + m_nDataBytes);

    // we made it this far, everything must be cool
    return ERROR_SUCCESS;
}

int CAIFFInputSource::GetData(unsigned char * pBuffer, int nBlocks, int * pBlocksRetrieved)
{
    if (!m_bIsValid) return ERROR_UNDEFINED;

    int nBytes = (m_wfeSource.nBlockAlign * nBlocks);
    unsigned int nBytesRead = 0;

    if (m_spIO->Read(pBuffer, nBytes, &nBytesRead) != ERROR_SUCCESS)
        return ERROR_IO_READ;

    if (m_wfeSource.wBitsPerSample == 16)
    {
        for (int nSample = 0; nSample < nBlocks * m_wfeSource.nChannels; nSample++)
        {
            unsigned char cTemp = pBuffer[(nSample * 2) + 0];
            pBuffer[(nSample * 2) + 0] = pBuffer[(nSample * 2) + 1];
            pBuffer[(nSample * 2) + 1] = cTemp;
        }
    }
    else if (m_wfeSource.wBitsPerSample == 24)
    {
        for (int nSample = 0; nSample < nBlocks * m_wfeSource.nChannels; nSample++)
        {
            unsigned char cTemp = pBuffer[(nSample * 3) + 0];
            pBuffer[(nSample * 3) + 0] = pBuffer[(nSample * 3) + 2];
            pBuffer[(nSample * 3) + 2] = cTemp;
        }
    }

    if (pBlocksRetrieved) *pBlocksRetrieved = (nBytesRead / m_wfeSource.nBlockAlign);

    return ERROR_SUCCESS;
}

int CAIFFInputSource::GetHeaderData(unsigned char * pBuffer)
{
    if (!m_bIsValid) return ERROR_UNDEFINED;

    int nResult = ERROR_SUCCESS;

    if (m_nHeaderBytes > 0)
    {
        int64 nOriginalFileLocation = m_spIO->GetPosition();

        m_spIO->SetSeekMethod(APE_FILE_BEGIN);
        m_spIO->SetSeekPosition(0);
        m_spIO->PerformSeek();
        
        unsigned int nBytesRead = 0;
        int nReadRetVal = m_spIO->Read(pBuffer, uint32(m_nHeaderBytes), &nBytesRead);

        if ((nReadRetVal != ERROR_SUCCESS) || (m_nHeaderBytes != int(nBytesRead)))
        {
            nResult = ERROR_UNDEFINED;
        }

        m_spIO->SetSeekMethod(APE_FILE_BEGIN);
        m_spIO->SetSeekPosition(nOriginalFileLocation);
        m_spIO->PerformSeek();
    }

    return nResult;
}

int CAIFFInputSource::GetTerminatingData(unsigned char * pBuffer)
{
    if (!m_bIsValid) return ERROR_UNDEFINED;

    int nResult = ERROR_SUCCESS;

    if (m_nTerminatingBytes > 0)
    {
        int64 nOriginalFileLocation = m_spIO->GetPosition();

        m_spIO->SetSeekMethod(APE_FILE_END);
        m_spIO->SetSeekPosition(-m_nTerminatingBytes);
        m_spIO->PerformSeek();
        
        unsigned int nBytesRead = 0;
        int nReadRetVal = m_spIO->Read(pBuffer, uint32(m_nTerminatingBytes), &nBytesRead);

        if ((nReadRetVal != ERROR_SUCCESS) || (m_nTerminatingBytes != int(nBytesRead)))
        {
            nResult = ERROR_UNDEFINED;
        }

        m_spIO->SetSeekMethod(APE_FILE_BEGIN);
        m_spIO->SetSeekPosition(nOriginalFileLocation);
        m_spIO->PerformSeek();
    }

    return nResult;
}

void CAIFFInputSource::FlipShort(void * pv)
{
    unsigned char * ptr = (unsigned char *) pv;
    unsigned char val;

    /* Swap 1st and 2nd bytes */
    val = *(ptr);
    *(ptr) = *(ptr + 1);
    *(ptr + 1) = val;
}

void CAIFFInputSource::FlipLong(void * pv)
{
    unsigned char * ptr = (unsigned char *) pv;
    unsigned char val;

    /* Swap 1st and 4th bytes */
    val = *(ptr);
    *(ptr) = *(ptr + 3);
    *(ptr + 3) = val;

    /* Swap 2nd and 3rd bytes */
    ptr += 1;
    val = *(ptr);
    *(ptr) = *(ptr + 1);
    *(ptr + 1) = val;
}

uint32 CAIFFInputSource::IEEE754ExtendedFloatToUINT32(unsigned char * buffer)
{
    unsigned long mantissa;
    unsigned long last = 0;
    unsigned char exp;

    FlipLong((buffer + 2));

    mantissa = *((unsigned long*)(buffer + 2));
    exp = 30 - *(buffer + 1);
    while (exp--)
    {
        last = mantissa;
        mantissa >>= 1;
    }
    if (last & 0x00000001)
        mantissa++;

    return (mantissa);
}

/*************************************************************************************
CW64InputSource - wraps working with W64 files
*************************************************************************************/
CW64InputSource::CW64InputSource(const wchar_t * pSourceName, WAVEFORMATEX * pwfeSource, int64 * pTotalBlocks, int64 * pHeaderBytes, int64 * pTerminatingBytes, int * pErrorCode)
    : CInputSource(pSourceName, pwfeSource, pTotalBlocks, pHeaderBytes, pTerminatingBytes, pErrorCode)
{
    m_bIsValid = false;

    if (pSourceName == NULL || pwfeSource == NULL)
    {
        if (pErrorCode) *pErrorCode = ERROR_BAD_PARAMETER;
        return;
    }

    m_spIO.Assign(CreateCIO());
    if (m_spIO->Open(pSourceName, true) != ERROR_SUCCESS)
    {
        m_spIO.Delete();
        if (pErrorCode) *pErrorCode = ERROR_INVALID_INPUT_FILE;
        return;
    }

    int nResult = AnalyzeSource();
    if (nResult == ERROR_SUCCESS)
    {
        // fill in the parameters
        if (pwfeSource) memcpy(pwfeSource, &m_wfeSource, sizeof(WAVEFORMATEX));
        if (pTotalBlocks) *pTotalBlocks = m_nDataBytes / int64(m_wfeSource.nBlockAlign);
        if (pHeaderBytes) *pHeaderBytes = m_nHeaderBytes;
        if (pTerminatingBytes) *pTerminatingBytes = m_nTerminatingBytes;

        m_bIsValid = true;
    }

    if (pErrorCode) *pErrorCode = nResult;
}

CW64InputSource::~CW64InputSource()
{
}

int CW64InputSource::AnalyzeSource()
{
    // chunk identifiers
    static const GUID guidRIFF = { 0x66666972, 0x912E, 0x11CF, 0xA5, 0xD6, 0x28, 0xDB, 0x04, 0xC1, 0x00, 0x00 };
    static const GUID guidWAVE = { 0x65766177, 0xACF3, 0x11D3, 0x8C, 0xD1, 0x00, 0xC0, 0x4F, 0x8E, 0xDB, 0x8A };
    static const GUID guidDATA = { 0x61746164, 0xACF3, 0x11D3, 0x8C, 0xD1, 0x00, 0xC0, 0x4F, 0x8E, 0xDB, 0x8A };
    static const GUID guidFMT = { 0x20746D66, 0xACF3, 0x11D3, 0x8C, 0xD1, 0x00, 0xC0, 0x4F, 0x8E, 0xDB, 0x8A };
    bool bReadMetadataChunks = false;

    // read the riff header
    bool bDataChunkRead = false;
    bool bFormatChunkRead = false;
    W64ChunkHeader RIFFHeader;
    unsigned int nBytesRead = 0;
    m_nFileBytes = m_spIO->GetSize();

    // cap at 4 GB (since file offsets are stored with 32-bit numbers)
    if (m_nFileBytes > (4 * BYTES_IN_GIGABYTE))
        return ERROR_INPUT_FILE_TOO_LARGE;

    m_spIO->Read(&RIFFHeader, sizeof(RIFFHeader), &nBytesRead);
    if ((memcmp(&RIFFHeader.guidIdentifier, &guidRIFF, sizeof(GUID)) == 0) && (RIFFHeader.nBytes == uint64(m_nFileBytes)))
    {
        // read and verify the wave data type header
        GUID DataHeader;
        nBytesRead = m_spIO->Read(&DataHeader, sizeof(DataHeader), &nBytesRead);
        if (memcmp(&DataHeader, &guidWAVE, sizeof(GUID)) == 0)
        {
            // for now, we only need to process these two chunks besides 'fmt ' chunk above -
            // "data", and "id3 "/"tag "
            while (1)
            {
                // read chunks one by one
                W64ChunkHeader Header;
                m_spIO->Read(&Header, sizeof(Header), &nBytesRead);

                // perhaps we have reached EOF
                if (nBytesRead < sizeof(Header))
                    break;

                // get / check chunk size
                int64 nChunkRemainingBytes = Header.nBytes - sizeof(Header);
                if ((m_spIO->GetPosition() + nChunkRemainingBytes) > m_nFileBytes)
                    break;

                // switched based on the chunk type
                if ((memcmp(&Header.guidIdentifier, &guidFMT, sizeof(GUID)) == 0) &&
                    (nChunkRemainingBytes >= sizeof(WAVFormatChunkData)))
                {
                    // read data
                    WAVFormatChunkData Data;
                    m_spIO->Read(&Data, sizeof(Data), &nBytesRead);
                    if (nBytesRead != sizeof(Data))
                        break;

                    // skip the rest
                    m_spIO->SetSeekMethod(APE_FILE_CURRENT);
                    m_spIO->SetSeekPosition(Align(nChunkRemainingBytes, 8) - sizeof(Data));
                    m_spIO->PerformSeek();

                    // verify the format (must be WAVE_FORMAT_PCM)
                    if (Data.nFormatTag != WAVE_FORMAT_PCM)
                    {
                        break;
                    }

                    // copy information over for internal storage
                    // may want to error check this header (bad avg bytes per sec, bad format, bad block align, etc...)
                    FillWaveFormatEx(&m_wfeSource, WAVE_FORMAT_PCM, Data.nSamplesPerSecond, Data.nBitsPerSample, Data.nChannels);
                    
                    m_wfeSource.nAvgBytesPerSec = Data.nAverageBytesPerSecond;
                    m_wfeSource.nBlockAlign = Data.nBlockAlign;

                    bFormatChunkRead = true;

                    // short circuit if we don't need metadata
                    if (!bReadMetadataChunks && (bFormatChunkRead && bDataChunkRead))
                        break;
                }
                else if (memcmp(&Header.guidIdentifier, &guidDATA, sizeof(GUID)) == 0)
                {
                    // 'data' chunk

                    // fill in the data bytes (the length of the 'data' chunk)
                    m_nDataBytes = nChunkRemainingBytes;
                    m_nHeaderBytes = m_spIO->GetPosition();

                    bDataChunkRead = true;

                    // short circuit if we don't need metadata
                    if (!bReadMetadataChunks && (bFormatChunkRead && bDataChunkRead))
                        break;

                    // move to the end of WAVEFORM data, so we can read other chunks behind it (if necessary)
                    m_spIO->SetSeekMethod(APE_FILE_CURRENT);
                    m_spIO->SetSeekPosition(Align(nChunkRemainingBytes, 8));
                    m_spIO->PerformSeek();
                }
                else
                {
                    m_spIO->SetSeekMethod(APE_FILE_CURRENT);
                    m_spIO->SetSeekPosition(Align(nChunkRemainingBytes, 8));
                    m_spIO->PerformSeek();
                }
            }
        }
    }

    // we must read both the data and format chunks
    if (bDataChunkRead && bFormatChunkRead)
    {
        // should error check this maybe
        m_nDataBytes = ape_min(m_nDataBytes, m_nFileBytes - m_nHeaderBytes);

        // get terminating bytes
        m_nTerminatingBytes = m_nFileBytes - m_nDataBytes - m_nHeaderBytes;

        // we're valid if we make it this far
        m_bIsValid = true;
    }

    // we made it this far, everything must be cool
    return ERROR_SUCCESS;
}

int CW64InputSource::GetData(unsigned char * pBuffer, int nBlocks, int * pBlocksRetrieved)
{
    if (!m_bIsValid) return ERROR_UNDEFINED;

    int nBytes = (m_wfeSource.nBlockAlign * nBlocks);
    unsigned int nBytesRead = 0;

    if (m_spIO->Read(pBuffer, nBytes, &nBytesRead) != ERROR_SUCCESS)
        return ERROR_IO_READ;

    if (pBlocksRetrieved) *pBlocksRetrieved = (nBytesRead / m_wfeSource.nBlockAlign);

    return ERROR_SUCCESS;
}

int CW64InputSource::GetHeaderData(unsigned char* pBuffer)
{
    if (!m_bIsValid) return ERROR_UNDEFINED;

    int nResult = ERROR_SUCCESS;

    if (m_nHeaderBytes > 0)
    {
        int64 nOriginalFileLocation = m_spIO->GetPosition();

        m_spIO->SetSeekMethod(APE_FILE_BEGIN);
        m_spIO->SetSeekPosition(0);
        m_spIO->PerformSeek();

        unsigned int nBytesRead = 0;
        int nReadRetVal = m_spIO->Read(pBuffer, uint32(m_nHeaderBytes), &nBytesRead);

        if ((nReadRetVal != ERROR_SUCCESS) || (m_nHeaderBytes != int(nBytesRead)))
        {
            nResult = ERROR_UNDEFINED;
        }

        m_spIO->SetSeekMethod(APE_FILE_BEGIN);
        m_spIO->SetSeekPosition(nOriginalFileLocation);
        m_spIO->PerformSeek();
    }

    return nResult;
}

int CW64InputSource::GetTerminatingData(unsigned char* pBuffer)
{
    if (!m_bIsValid) return ERROR_UNDEFINED;

    int nResult = ERROR_SUCCESS;

    if (m_nTerminatingBytes > 0)
    {
        int64 nOriginalFileLocation = m_spIO->GetPosition();

        m_spIO->SetSeekMethod(APE_FILE_END);
        m_spIO->SetSeekPosition(-m_nTerminatingBytes);
        m_spIO->PerformSeek();

        unsigned int nBytesRead = 0;
        int nReadRetVal = m_spIO->Read(pBuffer, uint32(m_nTerminatingBytes), &nBytesRead);

        if ((nReadRetVal != ERROR_SUCCESS) || (m_nTerminatingBytes != int(nBytesRead)))
        {
            nResult = ERROR_UNDEFINED;
        }

        m_spIO->SetSeekMethod(APE_FILE_BEGIN);
        m_spIO->SetSeekPosition(nOriginalFileLocation);
        m_spIO->PerformSeek();
    }

    return nResult;
}

/*************************************************************************************
CSNDInputSource - wraps working with SND files
*************************************************************************************/
CSNDInputSource::CSNDInputSource(const wchar_t * pSourceName, WAVEFORMATEX * pwfeSource, int64 * pTotalBlocks, int64 * pHeaderBytes, int64 * pTerminatingBytes, int * pErrorCode, int32 * pFlags)
    : CInputSource(pSourceName, pwfeSource, pTotalBlocks, pHeaderBytes, pTerminatingBytes, pErrorCode)
{
    m_bIsValid = false;
    m_bBigEndian = false;

    if (pSourceName == NULL || pwfeSource == NULL)
    {
        if (pErrorCode) *pErrorCode = ERROR_BAD_PARAMETER;
        return;
    }

    m_spIO.Assign(CreateCIO());
    if (m_spIO->Open(pSourceName, true) != ERROR_SUCCESS)
    {
        m_spIO.Delete();
        if (pErrorCode) *pErrorCode = ERROR_INVALID_INPUT_FILE;
        return;
    }

    int nResult = AnalyzeSource(pFlags);
    if (nResult == ERROR_SUCCESS)
    {
        // fill in the parameters
        if (pwfeSource) memcpy(pwfeSource, &m_wfeSource, sizeof(WAVEFORMATEX));
        if (pTotalBlocks) *pTotalBlocks = m_nDataBytes / int64(m_wfeSource.nBlockAlign);
        if (pHeaderBytes) *pHeaderBytes = m_nHeaderBytes;
        if (pTerminatingBytes) *pTerminatingBytes = m_nTerminatingBytes;

        m_bIsValid = true;
    }

    if (pErrorCode) *pErrorCode = nResult;
}

CSNDInputSource::~CSNDInputSource()
{
}

int CSNDInputSource::AnalyzeSource(int32 * pFlags)
{
    bool bIsValid = false;
    bool bSupportedFormat = false;

    // store the original pointer and move to the beginning of the file
    m_spIO->SetSeekMethod(APE_FILE_BEGIN);
    m_spIO->SetSeekPosition(0);
    m_spIO->PerformSeek();

    // get the file size (may want to error check this for files over 2 GB)
    m_nFileBytes = m_spIO->GetSize();

    // read the AU header
    class CAUHeader
    {
    public:
        uint32 m_nMagicNumber;
        uint32 m_nDataOffset;
        uint32 m_nDataSize;
        uint32 m_nEncoding;
        uint32 m_nSampleRate;
        uint32 m_nChannels;
    };
    CAUHeader Header = { 0 };
    unsigned int nBytesRead = 0;
    if ((m_spIO->Read(&Header, sizeof(Header), &nBytesRead) == ERROR_SUCCESS) &&
        (nBytesRead == sizeof(Header)))
    {
        bool bMagicNumberValid = false;
        if (memcmp(&Header.m_nMagicNumber, "dns.", 4) == 0)
        {
            // already little-endian
            bMagicNumberValid = true;
        }
        else if (memcmp(&Header.m_nMagicNumber, ".snd", 4) == 0)
        {
            // big-endian (so reverse)
            bMagicNumberValid = true;
            m_bBigEndian = true;
            Header.m_nDataOffset = FlipByteOrder32(Header.m_nDataOffset);
            Header.m_nDataSize = FlipByteOrder32(Header.m_nDataSize);
            Header.m_nEncoding = FlipByteOrder32(Header.m_nEncoding);
            Header.m_nSampleRate = FlipByteOrder32(Header.m_nSampleRate);
            Header.m_nChannels = FlipByteOrder32(Header.m_nChannels);
        }

        if (bMagicNumberValid &&
            (Header.m_nDataOffset >= sizeof(Header)) &&
            (Header.m_nDataOffset < m_nFileBytes))
        {
            // get sizes
            m_nHeaderBytes = Header.m_nDataOffset;
            m_nDataBytes = m_nFileBytes - m_nHeaderBytes;
            if (Header.m_nDataSize > 0)
                m_nDataBytes = ape_min((int64) Header.m_nDataSize, m_nDataBytes);
            m_nTerminatingBytes = m_nFileBytes - m_nHeaderBytes - m_nDataBytes;

            // set format
            if (Header.m_nEncoding == 1)
            {
                // 8-bit mulaw
                // not supported
            }
            else if (Header.m_nEncoding == 2)
            {
                // 8-bit PCM
                FillWaveFormatEx(&m_wfeSource, WAVE_FORMAT_PCM, Header.m_nSampleRate, 8, Header.m_nChannels);
                bSupportedFormat = true;
            }
            else if (Header.m_nEncoding == 3)
            {
                // 16-bit PCM
                FillWaveFormatEx(&m_wfeSource, WAVE_FORMAT_PCM, Header.m_nSampleRate, 16, Header.m_nChannels);
                bSupportedFormat = true;
            }
            else if (Header.m_nEncoding == 4)
            {
                // 24-bit PCM
                FillWaveFormatEx(&m_wfeSource, WAVE_FORMAT_PCM, Header.m_nSampleRate, 24, Header.m_nChannels);
                bSupportedFormat = true;
            }
            else if (Header.m_nEncoding == 5)
            {
                // 32-bit PCM
                FillWaveFormatEx(&m_wfeSource, WAVE_FORMAT_PCM, Header.m_nSampleRate, 32, Header.m_nChannels);
                bSupportedFormat = true;
            }
            else if (Header.m_nEncoding == 6)
            {
                // 32-bit float
                // not supported
            }
            else if (Header.m_nEncoding == 7)
            {
                // 64-bit float
                // not supported
            }
            else
            {
                // unsupported format
                ASSERT(false);
            }
        }
        else
        {
            // invalid header
            ASSERT(false);
        }

        // update return value
        if (bSupportedFormat)
            bIsValid = true;
    }

    // seek to the end of the header
    m_spIO->SetSeekMethod(APE_FILE_BEGIN);
    m_spIO->SetSeekPosition(m_nHeaderBytes);
    m_spIO->PerformSeek();

    // update flags
    *pFlags |= MAC_FORMAT_FLAG_SND;
    if (m_bBigEndian)
        *pFlags |= MAC_FORMAT_FLAG_BIG_ENDIAN;

    // we made it this far, everything must be cool
    return bIsValid ? ERROR_SUCCESS : ERROR_UNDEFINED;
}

int CSNDInputSource::GetData(unsigned char * pBuffer, int nBlocks, int * pBlocksRetrieved)
{
    if (!m_bIsValid) return ERROR_UNDEFINED;

    int nBytes = (m_wfeSource.nBlockAlign * nBlocks);
    unsigned int nBytesRead = 0;

    if (m_spIO->Read(pBuffer, nBytes, &nBytesRead) != ERROR_SUCCESS)
        return ERROR_IO_READ;

    if (m_bBigEndian)
    {
        if (m_wfeSource.wBitsPerSample == 16)
        {
            for (int nSample = 0; nSample < nBlocks * m_wfeSource.nChannels; nSample++)
            {
                unsigned char cTemp = pBuffer[(nSample * 2) + 0];
                pBuffer[(nSample * 2) + 0] = pBuffer[(nSample * 2) + 1];
                pBuffer[(nSample * 2) + 1] = cTemp;
            }
        }
        else if (m_wfeSource.wBitsPerSample == 24)
        {
            for (int nSample = 0; nSample < nBlocks * m_wfeSource.nChannels; nSample++)
            {
                unsigned char cTemp = pBuffer[(nSample * 3) + 0];
                pBuffer[(nSample * 3) + 0] = pBuffer[(nSample * 3) + 2];
                pBuffer[(nSample * 3) + 2] = cTemp;
            }
        }
        else if (m_wfeSource.wBitsPerSample == 32)
        {
            for (int nSample = 0; nSample < nBlocks * m_wfeSource.nChannels; nSample++)
            {
                uint32 nValue = *((uint32 *) &pBuffer[(nSample * 4) + 0]);
                uint32 nFlippedValue = (((nValue >> 0) & 0xFF) << 24) | (((nValue >> 8) & 0xFF) << 16) | (((nValue >> 16) & 0xFF) << 8) | (((nValue >> 24) & 0xFF) << 0);
                *((uint32 *) &pBuffer[(nSample * 4) + 0]) = nFlippedValue;
            }
        }
    }

    if (pBlocksRetrieved) *pBlocksRetrieved = (nBytesRead / m_wfeSource.nBlockAlign);

    return ERROR_SUCCESS;
}

int CSNDInputSource::GetHeaderData(unsigned char * pBuffer)
{
    if (!m_bIsValid) return ERROR_UNDEFINED;

    int nResult = ERROR_SUCCESS;

    if (m_nHeaderBytes > 0)
    {
        int64 nOriginalFileLocation = m_spIO->GetPosition();

        m_spIO->SetSeekMethod(APE_FILE_BEGIN);
        m_spIO->SetSeekPosition(0);
        m_spIO->PerformSeek();

        unsigned int nBytesRead = 0;
        int nReadRetVal = m_spIO->Read(pBuffer, uint32(m_nHeaderBytes), &nBytesRead);

        if ((nReadRetVal != ERROR_SUCCESS) || (m_nHeaderBytes != int(nBytesRead)))
        {
            nResult = ERROR_UNDEFINED;
        }

        m_spIO->SetSeekMethod(APE_FILE_BEGIN);
        m_spIO->SetSeekPosition(nOriginalFileLocation);
        m_spIO->PerformSeek();
    }

    return nResult;
}

int CSNDInputSource::GetTerminatingData(unsigned char * pBuffer)
{
    if (!m_bIsValid) return ERROR_UNDEFINED;

    int nResult = ERROR_SUCCESS;

    if (m_nTerminatingBytes > 0)
    {
        int64 nOriginalFileLocation = m_spIO->GetPosition();

        m_spIO->SetSeekMethod(APE_FILE_END);
        m_spIO->SetSeekPosition(-m_nTerminatingBytes);
        m_spIO->PerformSeek();

        unsigned int nBytesRead = 0;
        int nReadRetVal = m_spIO->Read(pBuffer, uint32(m_nTerminatingBytes), &nBytesRead);

        if ((nReadRetVal != ERROR_SUCCESS) || (m_nTerminatingBytes != int(nBytesRead)))
        {
            nResult = ERROR_UNDEFINED;
        }

        m_spIO->SetSeekMethod(APE_FILE_BEGIN);
        m_spIO->SetSeekPosition(nOriginalFileLocation);
        m_spIO->PerformSeek();
    }

    return nResult;
}

}