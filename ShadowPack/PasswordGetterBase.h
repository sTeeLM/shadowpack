#pragma once

#include "framework.h"

class CPasswordGetterBase
{
public:
	CPasswordGetterBase() {};
	virtual ~CPasswordGetterBase() {};
public:
	virtual CString GetPassword() = 0;
};