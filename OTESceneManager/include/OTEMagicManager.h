#pragma once

#include "OTEStaticInclude.h"
#include "OTEQTSceneManager.h"
#include "otebillboardset.h"

namespace OTE
{
	#define MAGIC_MGR	COTEMagicManager::GetSingleton()

	// *************************************
	// MagicData_t
	// Magic数据
	// *************************************	

    struct MagicData_t
	{
		std::string				MagicFile;
		std::string				Type;

		Ogre::MovableObject*	Entity;		

		MagicData_t()
		{
			Entity	=	NULL;
		}
	};
	
	// *************************************
	// COTEMagicManager
	// 用于管理“动态对象(Magic)”的类
	// 无论是场景中 还是角色行动中
	// 动态创建 动态删除的 对象 称为 Magic
	// *************************************
	class _OTEExport COTEMagicManager
	{
	public:	

		/// 单体

		static COTEMagicManager* GetSingleton();

		static void DestroySingleton();	

		// 清理

		static void Clear();

	public:	

		/// 创建

		Ogre::MovableObject* CreateMagic(const std::string& ResName, 
										 const std::string& Name, bool cloneMat = true);	
		Ogre::MovableObject* AutoCreateMagic(const std::string& ResName, bool cloneMat = true);
		// 获取	
		Ogre::MovableObject* GetMagic(const std::string& Name);

		// 删除实体
		void DeleteMagic(const std::string& Name, bool Immediately = false);			

		// 获取MagicList
		void GetMagicList(std::vector<Ogre::MovableObject*>* elist);
	
		static std::string GetMagType(const std::string& rResName); 

		// 更新删除 

		void _UpdateGrave();

	public:

		// magic工厂列表
		HashMap<std::string, CMagicFactoryBase*> m_MagFactoryList;

	protected:
		
		//加载Magic
		MagicData_t* LoadMagic(	const std::string& ResName, 
								const std::string& Name, 										 
								bool cloneMat);

		//创建Magic
		void Create(const std::string& ResName, const std::string& Name, MagicData_t* pMagData, bool cloneMat);

		//初始化
		void Init();	

		// 特效数量
		unsigned int GetMagCount() { return m_MagDataList.size(); }

	protected:	
		
		// 单体对象
		static COTEMagicManager*	s_pSingleton;

		// MagicData_t 数据列表
		std::map<std::string, MagicData_t*> m_MagDataList;

		// 坟墓
		std::list<MagicData_t*> m_GraveList;

	};


}
