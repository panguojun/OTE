#pragma once
#include "OTELineObj.h"
#include "OTETile.h"

// **************************************************
// CBrushEntity
// **************************************************
class CBrushEntity
{
public:
	CBrushEntity(void);
	virtual ~CBrushEntity(void);

public:

	// ¸üÐÂ»æÖÆ
	
	virtual void UpdateDrawing(const Ogre::Vector3& rBrushPos) = 0;	

public:

	std::string				m_strName;

	bool					m_bVisible;	

	Ogre::Vector3			m_Pos;
	
};
