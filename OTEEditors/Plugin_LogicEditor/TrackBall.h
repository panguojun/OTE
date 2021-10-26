#pragma once
#include "OTEQTSceneManager.h"
#include "Ogre.h"
#include "OTEActorEntity.h"
#include "OTEMagicManager.h"

// ********************************************************
using namespace Ogre;
using namespace OTE;

// ********************************************************
// 实体数据
// ********************************************************
struct EntityEditData_t
{
	// 数据

	std::string Name;
	std::string ResName;
	Ogre::Vector3 Pos;
	Ogre::Quaternion Rot;
	Ogre::Vector3 Scale;
	std::string Type;	
	std::string EditDataType;

	// construction
	EntityEditData_t(){}

	EntityEditData_t(Ogre::MovableObject* m)
	{
		if(m->getName().find("Ent") != -1)
		{
			ResName = ((OTE::COTEActorEntity*)m)->m_ResFile;
			Type = ".obj" ;
		}		

		Name	= m->getName();
		Pos		= m->getParentNode()->getPosition();		
		Rot		= m->getParentNode()->getOrientation();
		Scale	= m->getParentNode()->getScale();
	}

	// --------------------------------------------
	// 从数据创建

	Ogre::MovableObject* CreateEntity()
	{
		if(Type.find(".obj") != -1)
		{
			
			Ogre::MovableObject* m = SCENE_MGR->CreateEntity(Name.c_str(), ResName.c_str());	
			m->getParentNode()->setPosition(Pos);
			m->getParentNode()->setOrientation(Rot);
			m->getParentNode()->setScale(Scale);

			return m;
		}
	}

	// --------------------------------------------	
	// 复制创建

	Ogre::MovableObject* CloneEntity()
	{
		if(Type.find(".obj") != -1)
		{			
			Ogre::MovableObject* m = SCENE_MGR->AutoCreateEntity(ResName);	
			SCENE_MGR->AttachObjToSceneNode(m);

			WAIT_LOADING_RES(m)
			
			m->getParentNode()->setPosition(Ogre::Vector3(Pos.x, Pos.y + 0.5f, Pos.z));
			m->getParentNode()->setOrientation(Rot);
			m->getParentNode()->setScale(Scale);

			return m;
		}

		return NULL;
	}
	

	virtual void UpdateEntity()	{}
	
};

// ********************************************************
// 后退
// ********************************************************

// 编辑
struct EditTrack_t : public EntityEditData_t
{
	EditTrack_t(Ogre::MovableObject* m) : EntityEditData_t(m){ EditDataType = "edit";}

	// 更新

	virtual void UpdateEntity()
	{
		if(Type.find(".obj") != -1)
		{
			if(SCENE_MGR->IsEntityPresent(Name))
			{
				Ogre::MovableObject* m = SCENE_MGR->GetEntity(Name.c_str());	
				
				m->getParentNode()->setPosition(Ogre::Vector3(Pos.x, Pos.y, Pos.z));
				m->getParentNode()->setOrientation(Rot);
				m->getParentNode()->setScale(Scale);
			}
		}
	}

};

// --------------------------------------------
// 创建
struct CreateTrack_t : public EntityEditData_t
{
	CreateTrack_t(Ogre::MovableObject* m) : EntityEditData_t(m){EditDataType = "create";}
	virtual void UpdateEntity()
	{		
		if(Type.find(".obj") != -1)
		{
			//创建实体
			SCENE_MGR->RemoveEntity(Name.c_str());
	
		}		
	}
};

// --------------------------------------------
// 删除
struct DeleteTrack_t : public EntityEditData_t
{
	DeleteTrack_t(Ogre::MovableObject* m) : EntityEditData_t(m){EditDataType = "delete";}
	virtual void UpdateEntity()
	{
		if(Type.find(".obj") != -1)
		{
			//创建实体
			Ogre::MovableObject* m = SCENE_MGR->CreateEntity(ResName, Name);		
			
			m->getParentNode()->setPosition(Ogre::Vector3(Pos.x, Pos.y, Pos.z));
			m->getParentNode()->setOrientation(Rot);
			m->getParentNode()->setScale(Scale);	
		}
	}
};

extern std::list<EntityEditData_t*>		    g_pTrackBallGroup;
extern void TrackBallTrack(EntityEditData_t* ee);
extern void ClearTrackBall();
extern void ClearTrackBall(const std::string&);