#pragma once
#include "BrushEntity.h"
// *************************************************
// CLightmapBrush
// *************************************************

class CLightmapBrush : public CBrushEntity
{
public:
	CLightmapBrush(void);
	virtual ~CLightmapBrush(void);

public:

	void UpdateDrawing(const Ogre::Vector3& rBrushPos);	

public:
	
	int							m_InnerRad;

	int							m_OutterRad;
	
	float						m_Intensity;

	// ��ɫ
	
	Ogre::ColourValue			m_Colour;

protected:

	CLineObj*				m_pInnerLineObj;

	CLineObj*				m_pOutterLineObj;
	
	unsigned char*				m_pRawData[3];

protected:

	// �õ�Ӱ��rect
	
	OTE::CRectA<float>			GetBrushInfluRect(float rad);

	void Init();

public:	

	void						DeleteBrushEntity();

	unsigned char*				GetBrushRawData();

};
