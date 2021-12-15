#pragma once
class CProgressBase
{
public:
	CProgressBase() {}
	virtual ~CProgressBase(){}
public:
	virtual void Reset(UINT nIDS = 0) = 0;
	virtual void SetFullScale(ULONGLONG nFull) = 0;
	virtual void Increase(ULONGLONG nVal) = 0;
	virtual void SetScale(ULONGLONG nVal) = 0;
	virtual void Cancel() = 0;
	virtual BOOL IsCanceled() = 0;
	virtual void Show(BOOL bShow = TRUE) = 0;
};