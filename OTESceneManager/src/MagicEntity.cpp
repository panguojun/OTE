#include "dymmagictoote.h"
#include "oteqtscenemanager.h"
#include "magicentity.h"
#include "OgreTagPoint.h"

using namespace Ogre;

namespace OTE
{
// ======================================================
// COTEMagicEntity
// ======================================================
COTEMagicEntity::COTEMagicEntity(const String& name, SceneManager *sm) : 
m_pMagicObj(NULL),
m_Name(name),
m_Visible(true),
m_AutoPlay(false)
{
	
}

COTEMagicEntity::~COTEMagicEntity(void)
{
}
// ------------------------------------------------------
// 在这个方法里 我们用于更新特效实体位置
void COTEMagicEntity::_notifyCurrentCamera(Camera* cam)
{	
	
}

// ------------------------------------------------------
void COTEMagicEntity::_updateRenderQueue(RenderQueue* queue)
{
	if(!m_Visible || m_pMagicObj == NULL)
		return;

	Vector3 pos = getParentNode()->getWorldPosition();
	Quaternion q = getParentNode()->getWorldOrientation();
	Vector3 rScale = getParentNode()->getScale();
		
	// 旋转
	Radian angle;
	Vector3 axis;
	q.ToAngleAxis(angle, axis);
	m_pMagicObj->SetRotation(angle.valueRadians(), axis.x, axis.y, axis.z); 

	// 位置
	m_pMagicObj->SetPosition(pos.x, pos.y, pos.z);
	
	//缩放
	m_pMagicObj->SetScaling(rScale.x, rScale.y, rScale.z);

	Ogre::Vector3 _min, _max;
	m_pMagicObj->GetBoundingBox(&_min.x, &_min.y, &_min.z, &_max.x, &_max.y, &_max.z);
	m_Box.setExtents(_min, _max);
}

//	----------------------------------------------
// 特效
CDymMagicToOTE* COTEMagicEntity::GetDymMagicInterface()
{
	return &g_dymMagicToOTE;
}

// ------------------------------------------------------
// 创建到场景
Ogre::MovableObject* COTEMagicEntity::CreateMagicEntity(const char* szName, const char* szFileName)
{
	if(!SCENE_MGR->GetSceneNode(szName))
	{
		GetDymMagicInterface()->CreateMagic(szFileName, szName);
		COTEMagicEntity* me = new COTEMagicEntity(szName, SCENE_MGR);
		me->m_pMagicObj = GetDymMagicInterface()->GetMagicObj(szName);
		Ogre::Vector3 _min, _max;
		me->m_pMagicObj->GetBoundingBox(&_min.x, &_min.y, &_min.z, &_max.x, &_max.y, &_max.z);
		me->m_Box.setExtents(_min, _max);
		me->SetVisible(true);

		// 跟场景节点邦定	
		
		SCENE_MGR->AttachObjToSceneNode(me, szName, "default");		

		return me;
	}

	return NULL;
}

// ------------------------------------------------------
// 删除场景特效
void COTEMagicEntity::DeleteMagicEntity(const char* szName)
{
	Ogre::SceneNode* sn = SCENE_MGR->GetSceneNode(szName);
	if(sn)
	{
		COTEMagicEntity* me = (COTEMagicEntity*)sn->getAttachedObject(0);
		if(me)	delete me;

		sn->getParentSceneNode()->removeChild(sn);
		SCENE_MGR->destroySceneNode(szName);	

		GetDymMagicInterface()->DeleteMagic(szName);
	}
}

// ------------------------------------------------------
void COTEMagicEntity::DeleteAllMagics()
{	
	int num = g_dymMagicToOTE.GetMagicNum();
	for(int i = 0; i < num; ++i)
	{
		g_dymMagicToOTE.DeleteMagic(g_dymMagicToOTE.GetMagicID(i));
	}
}

// ------------------------------------------------------
void COTEMagicEntity::SetAllMagicsVisible(bool isVisible)
{	
	int num = g_dymMagicToOTE.GetMagicNum();
	for(int i = 0; i < num; ++i)
	{
		Ogre::SceneNode* sn = SCENE_MGR->GetSceneNode(g_dymMagicToOTE.GetMagicID(i));
		if(sn)
		{
			COTEMagicEntity* me = (COTEMagicEntity*)sn->getAttachedObject(0);
			if(me)	
			{
				me->SetVisible(isVisible);
			}
		}		
	}
}

// ------------------------------------------------------
// 可见性	
void COTEMagicEntity::SetVisible(bool bVis)
{
	m_Visible = bVis;
	if(m_pMagicObj != NULL)
		m_pMagicObj->m_bVisible = m_Visible;
	setVisible(m_Visible);
}


// ======================================
// COTEDymEffFactory
// ======================================
Ogre::MovableObject* COTEDymEffFactory::Create(const std::string& ResName, const std::string& Name, bool cloneMat)
{	
	return COTEMagicEntity::CreateMagicEntity(Name.c_str(), ResName.c_str());
}

//-----------------------------------------------------------------------
void COTEDymEffFactory::Delete(const std::string& Name)
{
	return COTEMagicEntity::DeleteMagicEntity(Name.c_str());
}

}
