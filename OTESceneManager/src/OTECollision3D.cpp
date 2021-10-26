#include "OTEStaticInclude.h"
#include "OTECollision3D.h"
#include "OTECollisionManager.h"
#include "OTETilePage.h"

// ---------------------------------------------
using namespace Ogre;
using namespace OTE;

// ---------------------------------------------
float COTECollision3D::GetHeightAt(float x, float y, float lastHeight)
{
	static float old_ret = -1.0f;
	static float old_x = -1.0f, old_y = -1.0f, old_lastHeight = -1.0f;
	if( (old_x == x) && (old_y == y) && (old_lastHeight == lastHeight) )
		return old_ret;

	old_x = x; old_y = y; old_lastHeight = lastHeight;

	const float c_TestHgt = 2.5f;

	float terrainHgt = 0.0f;
	if(COTETilePage::GetCurrentPage())
		terrainHgt = COTETilePage::GetCurrentPage()->GetHeightAt(x, y);

	Ogre::Vector3 hitPos;
	
	if(COTECollisionManager::TriangleIntersection(
		Ogre::Ray(Ogre::Vector3(x, lastHeight + c_TestHgt, y), Ogre::Vector3(0, -1.0f, 0)), 
		hitPos))
	{
		if(terrainHgt < hitPos.y)
		{
			old_ret = hitPos.y;			
		}
		else
		{
			old_ret = terrainHgt;
		}
	}
	else
	{
		old_ret = terrainHgt;		
	}

	return old_ret;	
}

// ---------------------------------------------
float COTECollision3D::GetHeightAt(float x, float y)
{	
	if(!COTETilePage::GetCurrentPage())
		return 0.0f;

	static float old_ret = -1;
	static float old_x = -1.0f, old_y = -1.0f;
	if( (old_x == x) && (old_y == y) )
		return old_ret;

	old_x = x; old_y = y;

	Ogre::Vector3 hitPos;

	const float c_TestHgt = 2.0f;

	float hgt = COTETilePage::GetCurrentPage()->GetHeightAt(x, y);	

	if(COTECollisionManager::TriangleIntersection(
		Ogre::Ray(Ogre::Vector3(x, hgt + c_TestHgt , y), Ogre::Vector3(0, -1.0f, 0)), 
		hitPos))
	{
		if(hgt < hitPos.y)
			old_ret = hitPos.y;
		else
			old_ret = hgt;
	}
	else
	{
		old_ret = hgt;		
	}	

	return old_ret;
}

// ---------------------------------------------
bool COTECollision3D::HitTest(float x, float z, float tx, float tz)
{
	if(!COTETilePage::GetCurrentPage())
		return false;

	Ogre::Vector3 hitPos;

	float h1 = COTETilePage::GetCurrentPage()->GetHeightAt(x, z);
	float h2 = COTETilePage::GetCurrentPage()->GetHeightAt(tx, tz);

	Ogre::Vector3 dir(tx - x, h2 - h1, tz - z);
	float dis = dir.length();
	dir.normalise();
	 
	if(COTECollisionManager::TriangleIntersection(
		Ogre::Ray(Ogre::Vector3(x, h1, z), dir), hitPos, dis)
		)
	{
		return true;
	}
	else
		return false;

}

// ---------------------------------------------
inline bool COTECollision3D::HitTest(const Ogre::Vector3& startPos,
						  const Ogre::Vector3& endPos, Ogre::Vector3& hitPos)
{
	Ogre::Vector3 dir = endPos - startPos;
	float dis = dir.length();
	dir.normalise();
	 
	if(COTECollisionManager::TriangleIntersection(
		Ogre::Ray(startPos, dir), hitPos, dis)
		)
	{
		return true;
	}

	return false;	
}

// ---------------------------------------------
bool COTECollision3D::HitTest(const Ogre::Vector3& startPos,
						  const Ogre::Vector3& endPos)
{
	float min_h = 1.2f;	// 最低
	float ply_h = 1.5f; // 玩家高度
	float l = 1.0f;		// 测试距离

	Ogre::Vector3 spPos1 = startPos; spPos1.y += min_h;
	Ogre::Vector3 headPos1 = spPos1; headPos1.y += ply_h; 	
	Ogre::Vector3 dir = Ogre::Vector3(endPos.x, spPos1.y, endPos.z) - spPos1;
	Ogre::Vector3 spPos2 = spPos1 + dir * l;
	Ogre::Vector3 headPos2 = headPos1 + dir * l;

	Ogre::Vector3 hitPos;

	if(   HitTest(spPos1, spPos2, hitPos)	  ||
		  HitTest(headPos1, headPos2, hitPos) ||
		  HitTest(spPos2, headPos2, hitPos)
		)
	{
		return true; // 不可达		
	}
	else
	{	
		return false; // 可达
	}
}

// ---------------------------------------------
// 地平线碰撞

bool COTECollision3D::PickFloor(const Ogre::Ray& rRay, float hgt, Ogre::Vector3& rHitPos)
{
	return COTECollisionManager::PickFloor(rRay, hgt, rHitPos);	
}

