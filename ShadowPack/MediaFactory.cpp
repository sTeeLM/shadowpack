#include "pch.h"
#include "resource.h"
#include "MediaFactory.h"
#include "BMPFileMedia.h"
#include "JPEGFileMedia.h"
#include "PNGFileMedia.h"
#include "TIFFileMedia.h"
#include "PPMFileMedia.h"


CMediaFactory::MEDIA_INFO_T CMediaFactory::m_MediaInfoImageFile[] =
{
	{CBMPFileMedia::GetName, CBMPFileMedia::GetExtTable, CBMPFileMedia::Factory},
	{CJPEGFileMedia::GetName, CJPEGFileMedia::GetExtTable, CJPEGFileMedia::Factory},
	{CPNGFileMedia::GetName, CPNGFileMedia::GetExtTable, CPNGFileMedia::Factory},
	{CTIFFileMedia::GetName, CTIFFileMedia::GetExtTable, CTIFFileMedia::Factory},
	{CPPMFileMedia::GetName, CPPMFileMedia::GetExtTable, CPPMFileMedia::Factory},
	{NULL, NULL, NULL}
};

CMediaFactory::MEDIA_DESC_T CMediaFactory::m_MediaDescTable[] =
{
	{IDS_MEDIA_IMAGE_FILE, m_MediaInfoImageFile},
};

CMediaFactory::CMediaFactory(void)
{

}

CMediaFactory::~CMediaFactory(void)
{

}

CMediaBase* CMediaFactory::CreateMediaFromExt(LPCTSTR szExt)
{
	MEDIA_INFO_T* p = NULL;
	LPCTSTR* pExt = NULL;
	for(INT i = 0 ; i < _countof(m_MediaDescTable) ; i++) {
		p = m_MediaDescTable[i].Infos;
		while (p && p->fnGetExtTable && p->fnFatory) {
			pExt = (*(p->fnGetExtTable))();
			while (pExt && *pExt) {
				if (lstrcmpi(*pExt, szExt) == 0) {
					return p->fnFatory();
				}
				pExt++;
			}
			p++;
		}
	}
	
	return NULL;

}

CString CMediaFactory::CreateExtTable()
{
	CString strRet;
	CString strTemp = _T("");
	CString strTemp1 = _T("");
	CString strTemp2 = _T("");
	MEDIA_INFO_T* p = NULL;
	LPCTSTR * pExt = NULL;
	for(INT i = 0 ; i < _countof(m_MediaDescTable); i++) {
		p = m_MediaDescTable[i].Infos;
		strTemp = _T("");
		while (p && p->fnGetExtTable) {
			pExt = (*(p->fnGetExtTable))();
			while (pExt && *pExt) {
				strTemp += _T("*.");
				strTemp += *pExt;
				strTemp += _T(";");
				pExt++;
			}
			p++;
		}
		if (!strTemp.IsEmpty()) {
			strTemp1.LoadString(m_MediaDescTable[i].Catagory);
			strTemp2.Format(_T("%s (%s)|%s|"), (LPCTSTR)strTemp1, (LPCTSTR)strTemp, (LPCTSTR)strTemp);
			strRet += strTemp2;
		}
	}

	strRet += _T("All Files (*.*)|*.*||");
	return strRet;
}