#pragma once

#include "Media.h"

typedef CMedia * (*FN_MEDIA_FACTORY)();
typedef BOOL (*FN_TEST_EXT)(LPCTSTR szExt);
typedef LPCTSTR (*FN_GET_FILTER)();
class CMediaFactory
{
public:
	CMediaFactory(void);
	virtual ~CMediaFactory(void);

public:
	static CMedia * CreateMediaFromExt(LPCTSTR szExt);
	static CString CreateExtTable();
private:
	typedef struct {
		FN_TEST_EXT fnTestExt;
		FN_GET_FILTER fnGetFilter;
		FN_MEDIA_FACTORY fnFactory;
		
	}ExtTable;

	static ExtTable m_ExtTable[];
};
