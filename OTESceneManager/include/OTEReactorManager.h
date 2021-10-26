#pragma once
#include "OTEReactorBase.h"

namespace OTE
{
// ***********************************
// COTEReactorManager
// 生化反应效果器　的容器及其管理
// ***********************************

class _OTEExport COTEReactorManager
{
public:
	
	static COTEReactorManager* GetSingleton();

	static void DestroySingleton();	

public:

	void Init();

	void Clear();

	CReactorBase*	GetReactor(int id);

	bool AddReactor(CReactorBase* reactor, int id);	

	void RemoveReactor(CReactorBase* reactor);

	void RemoveReactor(int id);

	void RemoveAllReactors();

	static std::string GetReactorName(const std::string& rResName); 

	CReactorBase* CreateReactor(const std::string& rResName, int id = -1, Ogre::MovableObject* mo = NULL);
	
	CReactorBase* CreateReactor();
	
	void DestroyReactor(CReactorBase* reactor);	

public:

	// 反应器工厂列表

	HashMap<std::string, CReactorFactoryBase*> m_ReactorFactoryList;

protected:

	static COTEReactorManager* s_pSingleton;

	// 反应器列表

	HashMap<int, CReactorBase*>			m_ReactorList;
};

}
