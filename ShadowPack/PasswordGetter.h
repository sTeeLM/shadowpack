#pragma once

class CPasswordGetter
{
public:
	CPasswordGetter() {}
	virtual ~CPasswordGetter() {}
public:
	virtual CString GetPassword() = 0;

};