#include "OTETerrainObjectRender.h"

using namespace Ogre;
using namespace OTE;

// ----------------------------------------------
void COTETerrainObjectRender::AttachToTile(const AxisAlignedBox& aabb, Ogre::MovableObject* mo, COTETilePage* page)
{	
	//Vector3 min = aabb.getMinimum();
	//Vector3 max = aabb.getMaximum();
	//
	//// ��TILEΪ��λ ����ռ�õĿռ�
	//float factor = 1.0f / ((page->m_TileSize - 1) * page->m_Scale.x); // m_Scale.x == m_Scale.z

	//int tileMinPosX = (int)min.x * factor;
	//int tileMaxPosX = (int)max.x * factor;
	//int tileMinPosZ = (int)min.z * factor;
	//int tileMaxPosZ = (int)max.z * factor;

	//// �߽�

	//tileMinPosX < 0 ? tileMinPosX = 0 : NULL;
	//tileMinPosZ < 0 ? tileMinPosZ = 0 : NULL;
	//tileMaxPosX < 0 ? tileMaxPosX = 0 : NULL;
	//tileMaxPosZ < 0 ? tileMaxPosZ = 0 : NULL;

	//int maxX = (int)(page->m_PageSize - 1) * factor;
	//int maxZ = maxX;

	//tileMinPosX > maxX ? tileMinPosX = maxX : NULL;
	//tileMinPosZ > maxZ ? tileMinPosZ = maxZ : NULL;
	//tileMaxPosX > maxX ? tileMaxPosX = maxX : NULL;
	//tileMaxPosZ > maxZ ? tileMaxPosZ = maxZ : NULL;

	//for(int x = tileMinPosX; x <= tileMaxPosX; ++ x)
	//for(int z = tileMinPosZ; z <= tileMaxPosZ; ++ z)
	//{
	//	COTETile* tile = page->GetTileAt(x, z);
	//	assert(tile);	

	//	tile->m_ObjList.push_back(mo);
	//}
}

// ----------------------------------------------
void COTETerrainObjectRender::AddToRenderQueue(COTETile* tile, Ogre::RenderQueue* queue)
{
	//std::vector<Ogre::MovableObject*>::iterator it = tile->m_ObjList.begin();
	//while(it != tile->m_ObjList.end())
	//{	
	//	Ogre::MovableObject* mo = *it;
	//	if(mo->getUserObject() == 0)
	//	{
	//		mo->_updateRenderQueue(queue);
	//		mo->setUserObject((UserDefinedObject*)0xFFFFFFFF); // #BUG# ����������addtorenderqueue֮�� һЩ�����Ⱦʧ�ܵ�ԭ�� ��һЩ����ῴ����
	//	}
	//					
	//	++ it;
	//};

}