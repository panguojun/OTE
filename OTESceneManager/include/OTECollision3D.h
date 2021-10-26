#pragma once
#include "OTEStaticInclude.h"

namespace OTE
{
// *****************************************
// COTECollision3D
// 3DÅö×²½Ó¿Ú
// *****************************************
class _OTEExport COTECollision3D
{
public:
	
	static float GetHeightAt(float x, float y, float lastHeight);

	static float GetHeightAt(float x, float y);

	static bool HitTest(float x, float z, float tx, float tz);

	static bool HitTest(const Ogre::Vector3& startPos, const Ogre::Vector3& endPos, 
						Ogre::Vector3& hitPos);
	
	static bool HitTest(const Ogre::Vector3& startPos,  const Ogre::Vector3& endPos);

	static bool PickFloor(const Ogre::Ray& rRay, float hgt, Ogre::Vector3& rHitPos);

};

}

