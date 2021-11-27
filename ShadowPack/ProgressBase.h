#pragma once
class CProgressBase
{
public:
	CProgressBase() {}
	virtual ~CProgressBase(){}
public:
	virtual void Reset() = 0;
	virtual void Cancel() = 0;
};