#pragma once
#include "OTEStaticInclude.h"
#include "OTETilepage.h"

namespace OTE
{
// *****************************************
// COTETerrainObjectListMgr
// 邦定到山体上的物体
// 静态函数集合
// *****************************************

class COTETerrainObjectRender
{
public:
	// 邦定到tile上
	static void AttachToTile(const Ogre::AxisAlignedBox& aabb, Ogre::MovableObject* mo, COTETilePage* page);
	
	// 加入渲染队列
	static void AddToRenderQueue(COTETile* tile, Ogre::RenderQueue* queue);

};

}
