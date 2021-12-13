#pragma once
#include "BytePerBlockMedia.h"
#include "OptPageCorImageMedia.h"

class CCorImageMedia :
    public CBytePerBlockMedia
{
public:
    CCorImageMedia();
    virtual ~CCorImageMedia();

public:
	BOOL Alloc(UINT nWidth, UINT nHeight, UINT nComponents, CPackErrors& Error);
	void Free();
	UINT GetTotalCoeffs();
	UINT GetHeightInBlocks(UINT nComponents);
	UINT GetWidthInBlocks(UINT nComponents);
	UINT GetCoeffPerBlock();
	void AddCoeff(UINT nComponents, UINT nX, UINT nY, UINT nCoeff, SHORT nData);
	SHORT GetCoeff(UINT nComponents, UINT nX, UINT nY, UINT nCoeff);

public:
	// 子类实现接口，我要调用
	virtual UINT GetVSampleFactor(UINT nComponents) = 0;
	virtual UINT GetHSampleFactor(UINT nComponents) = 0;

public:
	// 实现父类接口，实现这几个接口让父类调用
	BYTE GetByteFromBlocks(UINT nOffset, UINT nBlockPerByte);
	void SetByteToBlocks(BYTE nData, UINT nOffset, UINT nBlockPerByte);
	UINT GetTotalBlocks();

	// 实现CMediaBase接口
	// 添加opt page
	virtual void AddOptPage(CMFCPropertySheet* pPropertySheet);
	virtual BOOL UpdateOpts(CMFCPropertySheet* pPropertySheet);

private:
	UINT DivRoundup(UINT a, UINT b);
	SHORT& GetCorFromIndex(UINT nIndex);
private:
	UINT m_nComponents;
	UINT m_nTotalCoeffs;
	UINT* m_pHeightInBlocks;
	UINT* m_pWidthInBlocks;
	SHORT ** m_pCoeffBuffer;
	SHORT m_nDummy;
	class CCoeffIndex {
	public:
		CCoeffIndex(UINT nComponents = 0, UINT nX = 0, UINT nY = 0, UINT nCoeff = 0) :
			m_nComponents(nComponents),
			m_nX(nX),
			m_nY(nY),
			m_nCoeff(nCoeff) {
			TRACE(_T("CCoeffIndex %d %d %d %d Alloced!\n"), m_nComponents, m_nX, m_nY, m_nCoeff);
		};
		virtual ~CCoeffIndex() {
			TRACE(_T("CCoeffIndex %d %d %d %d Freed!\n"), m_nComponents, m_nX, m_nY, m_nCoeff);
		}
		CCoeffIndex& operator=(const CCoeffIndex& cls) {
			TRACE(_T("CCoeffIndex operator= called\n"));
			m_nComponents = cls.m_nComponents;
			m_nX = cls.m_nX;
			m_nY = cls.m_nY;
			m_nCoeff = cls.m_nCoeff;
			return *this;
		}
	public:
		UINT m_nComponents;
		UINT m_nX;
		UINT m_nY;
		UINT m_nCoeff;
	};

protected:
	CArray<CCoeffIndex, CCoeffIndex> m_CoeffIndex;
	COptPageCorImageMedia m_OptPageCorImageMedia;
};

