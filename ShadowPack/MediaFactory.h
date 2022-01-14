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
public:
	static CMediaBase* CreateMediaFromExt(LPCTSTR szExt);
	static CString CreateExtTable();
	static void LoadMediaInfo();
	static void DestoryMediaInfo();
private:
	static FN_MEDIA_GET_MEDIA_INFO m_InfoTable[];
	static CArray<CMediaInfo> m_AllInfo;

	// 扩展名对CMediaInfo，CMediaInfo可能有多个， value是CObArray，array item是CMediaInfo
	static CMapStringToOb m_ExtFactoryMap;

	// nCatagory对扩展名列表，扩展名有多个， value是CStringArray, array item是Ext
	static CMapWordToOb  m_CatagorExtMap;

	// dlgfile的filter
	static CString m_strFilter;
};
