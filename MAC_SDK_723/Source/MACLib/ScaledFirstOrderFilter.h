#pragma once

namespace APE
{

template <class INTTYPE, int MULTIPLY, int SHIFT> class CScaledFirstOrderFilter
{
public:
    
    __forceinline void Flush()
    {
        m_nLastValue = 0;
    }

    __forceinline INTTYPE Compress(const int32 nInput)
    {
        INTTYPE nResult = INTTYPE(nInput) - ((INTTYPE(m_nLastValue) * MULTIPLY) >> SHIFT);
        m_nLastValue = nInput;
        return nResult;
    }

    __forceinline int32 Decompress(const INTTYPE nInput)
    {
        m_nLastValue = int32(nInput + ((INTTYPE(m_nLastValue) * MULTIPLY) >> SHIFT));
        return m_nLastValue;
    }

protected:
    int32 m_nLastValue;
};

}