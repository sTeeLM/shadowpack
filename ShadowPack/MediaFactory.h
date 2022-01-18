#pragma once

#include "framework.h"
#include "MediaBase.h"

class CMediaFactory
{
public:
	CMediaFactory(void);
	virtual ~CMediaFactory(void);

public:
	typedef CMediaBase* (*FN_MEDIA_FACTORY)();
	class CMediaInfo : public CObject
	{
	public:
		UINT nCatagory;
		CString strName;
		CStringArray Exts;
		FN_MEDIA_FACTORY fnFactory;
		const CMediaInfo& operator=(const CMediaInfo& src) {
			nCatagory = src.nCatagory;
			strName = src.strName;
			fnFactory = src.fnFactory;
			Exts.Copy(src.Exts);
			return *this;
		}
	};
	
	typedef void (*FN_MEDIA_GET_MEDIA_INFO)(CArray<CMediaInfo>& InfoArray);
	typedef struct _MEDIA_EXT_TABLE_T {
		LPCTSTR szExts;
		LPCTSTR szName;
	} MEDIA_EXT_TABLE_T;
public:
	static void LoadMediaExt(MEDIA_EXT_TABLE_T * pExtTable, UINT nCnt, FN_MEDIA_FACTORY fnFactory, 
		UINT nCatagory, CArray<CMediaInfo>& InfoArray);
	static CMediaBase* CreateMediaFromExt(LPCTSTR szExt);
	static CString CreateExtTable();
	static void LoadMediaInfo();
	static void DestoryMediaInfo();
private:
	static FN_MEDIA_GET_MEDIA_INFO m_InfoTable[];
	static CArray<CMediaInfo> m_AllInfo;

	// ��չ����CMediaInfo��CMediaInfo�����ж���� value��CObArray��array item��CMediaInfo
	static CMapStringToOb m_ExtFactoryMap;

	// nCatagory����չ���б���չ���ж���� value��CStringArray, array item��Ext
	static CMapWordToOb  m_CatagorExtMap;

	// dlgfile��filter
	static CString m_strFilter;
};
