#pragma once
#include "OTEStaticInclude.h"
#include "OTETilepage.h"

namespace OTE
{
// *****************************************
// COTETerrainObjectListMgr
// ���ɽ���ϵ�����
// ��̬��������
// *****************************************

class COTETerrainObjectRender
{
public:
	// ���tile��
	static void AttachToTile(const Ogre::AxisAlignedBox& aabb, Ogre::MovableObject* mo, COTETilePage* page);
	
	// ������Ⱦ����
	static void AddToRenderQueue(COTETile* tile, Ogre::RenderQueue* queue);

};

}
