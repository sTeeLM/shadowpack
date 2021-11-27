#pragma once
class CProgressBase
{
public:
	CProgressBase() {}
	virtual ~CProgressBase(){}
public:
	virtual void Reset() = 0;
	virtual void SetFullScale(UINT nFull) = 0;
	virtual void Increase(UINT val) = 0;
	virtual void Cancel() = 0;
	virtual BOOL IsCanceled() = 0;
};