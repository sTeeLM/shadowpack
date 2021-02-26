#include "StdAfx.h"
#include "MediaFactory.h"

#include "BMPImageMedia.h"
#include "PNGImageMedia.h"
#include "TIFFImageMedia.h"

CMediaFactory::ExtTable CMediaFactory::m_ExtTable[] = {
	{&CBMPImageMedia::TestExt, &CBMPImageMedia::GetExtFilter, &CBMPImageMedia::Factory},
	{&CPNGImageMedia::TestExt, &CPNGImageMedia::GetExtFilter, &CPNGImageMedia::Factory},
	{&CTIFFImageMedia::TestExt, &CTIFFImageMedia::GetExtFilter, &CTIFFImageMedia::Factory},
};

CMediaFactory::CMediaFactory(void)
{

}

CMediaFactory::~CMediaFactory(void)
{

}

CMedia * CMediaFactory::CreateMediaFromExt(LPCTSTR szExt)
{
	INT i;
	CMedia * pRet = NULL;

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