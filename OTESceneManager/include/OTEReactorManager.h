#pragma once
#include "OTEReactorBase.h"

namespace OTE
{
// ***********************************
// COTEReactorManager
// ������ӦЧ�������������������
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

	// ��Ӧ�������б�

	HashMap<std::string, CReactorFactoryBase*> m_ReactorFactoryList;

protected:

	static COTEReactorManager* s_pSingleton;

	// ��Ӧ���б�

	HashMap<int, CReactorBase*>			m_ReactorList;
};

}
