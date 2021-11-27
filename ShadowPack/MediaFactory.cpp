
#include "MediaFactory.h"
#include "BMPFileMedia.h"

CMediaFactory::ExtTable CMediaFactory::m_ExtTable[] = {
	{&CBMPFileMedia::TestExt, &CBMPFileMedia::GetExtFilter, &CBMPFileMedia::Factory},

};

CMediaFactory::CMediaFactory(void)
{

}

CMediaFactory::~CMediaFactory(void)
{

}

CMediaBase* CMediaFactory::CreateMediaFromExt(LPCTSTR szExt)
{
	INT i;
	CMediaBase* pRet = NULL;

	for(i = 0 ; i < sizeof(m_ExtTable) / sizeof(ExtTable) ; i++) {
		if(m_ExtTable[i].fnTestExt(szExt)) {
			pRet = m_ExtTable[i].fnFactory();
			break;
		}
	}
	
	return pRet;

}

CString CMediaFactory::CreateExtTable()
{
	INT i;
	CString strRet;
	for(i = 0 ; i < sizeof(m_ExtTable) / sizeof(ExtTable) ; i++) {
		strRet += m_ExtTable[i].fnGetFilter();
	}
	strRet += _T("All Files (*.*)|*.*||");
	return strRet;
}