#include "pch.h"
#include "resource.h"
#include "MediaFactory.h"
#include "BMPFileMedia.h"
#include "JPEGFileMedia.h"
#include "PNGFileMedia.h"
#include "TIFFileMedia.h"
#include "PPMFileMedia.h"

#include "MiscAudioMedia.h"
#include "SndfileAudioMedia.h"


CMediaFactory::FN_MEDIA_GET_MEDIA_INFO CMediaFactory::m_InfoTable[] =
{
	CBMPFileMedia::GetMediaInfo,
	CJPEGFileMedia::GetMediaInfo,
	CPNGFileMedia::GetMediaInfo,
	CTIFFileMedia::GetMediaInfo,
	CPPMFileMedia::GetMediaInfo,
	CMiscAudioMedia::GetMediaInfo,
	CSndfileAudioMedia::GetMediaInfo
};

// 所有CMediaInfo的列表
CArray<CMediaFactory::CMediaInfo> CMediaFactory::m_AllInfo;

// 扩展名对CMediaInfo，CMediaInfo可能有多个, value是CObArray,array item是CMediaInfo
CMapStringToOb  CMediaFactory::m_ExtFactoryMap;

// nCatagory对扩展名列表，扩展名有多个， value是CStringArray,array item是Ext
CMapWordToOb   CMediaFactory::m_CatagorExtMap;

CString CMediaFactory::m_strFilter =_T("");

CMediaFactory::CMediaFactory(void)
{

}

CMediaFactory::~CMediaFactory(void)
{

}

void CMediaFactory::DestoryMediaInfo()
{
	WORD nKey;
	CString strKey;
	CObject* pValue = NULL;
	for (POSITION pos = m_CatagorExtMap.GetStartPosition(); pos != NULL;) {
		m_CatagorExtMap.GetNextAssoc(pos, nKey, pValue);
		delete dynamic_cast<CStringArray*>(pValue);
	}
	
	for (POSITION pos = m_ExtFactoryMap.GetStartPosition(); pos != NULL;) {
		m_ExtFactoryMap.GetNextAssoc(pos, strKey, pValue);
		delete dynamic_cast<CObArray*>(pValue);
	}
	m_CatagorExtMap.RemoveAll();
	m_ExtFactoryMap.RemoveAll();
	m_AllInfo.RemoveAll();
}

void CMediaFactory::LoadMediaInfo()
{
	CObject* p = NULL;
	WORD nKey;
	CString strKey;
	CObArray* pArray = NULL;
	CStringArray* pStringArray = NULL;
	CMediaInfo* pMediaInfo = NULL;
	CString strTemp;
	for (INT i = 0; i < _countof(m_InfoTable); i++) {
		m_InfoTable[i](m_AllInfo);
	}

	// fill m_ExtFactoryMap,m_CatagorExtMap
	for (INT i = 0; i < m_AllInfo.GetCount(); i++) {
		if (m_CatagorExtMap.Lookup(m_AllInfo[i].nCatagory, p)) {
			(dynamic_cast<CStringArray*>(p))->Append(m_AllInfo[i].Exts);
		}
		else {
			p = new CStringArray();
			(dynamic_cast<CStringArray*>(p))->Append(m_AllInfo[i].Exts);
			m_CatagorExtMap.SetAt(m_AllInfo[i].nCatagory,p);
		}

		for (INT j = 0; j < m_AllInfo[i].Exts.GetCount(); j++) {
			if (m_ExtFactoryMap.Lookup(m_AllInfo[i].Exts[j], p)) {
				(dynamic_cast<CObArray *>(p))->Add(&m_AllInfo[i]);
			}
			else {
				p = new CObArray();
				(dynamic_cast<CObArray*>(p))->Add(&m_AllInfo[i]);
				m_ExtFactoryMap.SetAt(m_AllInfo[i].Exts[j], p);
			}
		}
	}

	/* creat ext string from m_ExtFactoryMap, m_CatagorExtMap */
	/* BMP Files (*.bmp)|*.bmp| */
	for (INT i = 0; i <  m_AllInfo.GetCount();i ++) {
		for (INT j = 0; j < m_AllInfo[i].Exts.GetCount(); j++) {
			strTemp += _T("*.") + m_AllInfo[i].Exts[j] + _T(";");
		}
		if (strTemp.Right(1) == _T(";")) {
			strTemp = strTemp.Left(strTemp.GetLength() - 1);
		}

		m_strFilter += m_AllInfo[i].strName + _T("(");
		m_strFilter += strTemp + _T(")|") + strTemp + _T("|");
		strTemp = _T("");
	}

	for (POSITION pos = m_CatagorExtMap.GetStartPosition(); pos != NULL;) {
		m_CatagorExtMap.GetNextAssoc(pos, nKey, p);
		pStringArray = dynamic_cast<CStringArray*>(p);
		strTemp.LoadString(nKey);
		m_strFilter += strTemp + _T("(");
		strTemp = _T("");
		for (INT i = 0; i < pStringArray->GetCount(); i++) {
			strTemp += _T("*.") + pStringArray->GetAt(i) + _T(";");
		}
		if (strTemp.Right(1) == _T(";")) {
			strTemp = strTemp.Left(strTemp.GetLength() - 1);
		}
		m_strFilter += strTemp + _T(")|") + strTemp + _T("|");
	}
	m_strFilter += _T("|");
}

CMediaBase* CMediaFactory::CreateMediaFromExt(LPCTSTR szExt)
{
	CString strKey;
	CObject* pValue = NULL;
	CObArray* pArray = NULL;
	CMediaInfo* pMediaInfo = NULL;

	strKey = szExt;
	m_ExtFactoryMap.Lookup(strKey, pValue);
	if (pValue) {
		pArray = dynamic_cast<CObArray*>(pValue);
		ASSERT(pArray->GetCount() == 1);
		pMediaInfo = dynamic_cast<CMediaInfo*>(pArray->GetAt(0)); /* 目前只取第一个 */
		return pMediaInfo->fnFactory();
	}
	return NULL;

}

CString CMediaFactory::CreateExtTable()
{
	return m_strFilter;
}