#pragma once

#include "OTEStaticInclude.h"
#include "OTEQTSceneManager.h"
#include "otebillboardset.h"

namespace OTE
{
	#define MAGIC_MGR	COTEMagicManager::GetSingleton()

	// *************************************
	// MagicData_t
	// Magic����
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
	// ���ڹ�����̬����(Magic)������
	// �����ǳ����� ���ǽ�ɫ�ж���
	// ��̬���� ��̬ɾ���� ���� ��Ϊ Magic
	// *************************************
	class _OTEExport COTEMagicManager
	{
	public:	

		/// ����

		static COTEMagicManager* GetSingleton();

		static void DestroySingleton();	

		// ����

		static void Clear();

	public:	

		/// ����

		Ogre::MovableObject* CreateMagic(const std::string& ResName, 
										 const std::string& Name, bool cloneMat = true);	
		Ogre::MovableObject* AutoCreateMagic(const std::string& ResName, bool cloneMat = true);
		// ��ȡ	
		Ogre::MovableObject* GetMagic(const std::string& Name);

		// ɾ��ʵ��
		void DeleteMagic(const std::string& Name, bool Immediately = false);			

		// ��ȡMagicList
		void GetMagicList(std::vector<Ogre::MovableObject*>* elist);
	
		static std::string GetMagType(const std::string& rResName); 

		// ����ɾ�� 

		void _UpdateGrave();

	public:

		// magic�����б�
		HashMap<std::string, CMagicFactoryBase*> m_MagFactoryList;

	protected:
		
		//����Magic
		MagicData_t* LoadMagic(	const std::string& ResName, 
								const std::string& Name, 										 
								bool cloneMat);

		//����Magic
		void Create(const std::string& ResName, const std::string& Name, MagicData_t* pMagData, bool cloneMat);

		//��ʼ��
		void Init();	

		// ��Ч����
		unsigned int GetMagCount() { return m_MagDataList.size(); }

	protected:	
		
		// �������
		static COTEMagicManager*	s_pSingleton;

		// MagicData_t �����б�
		std::map<std::string, MagicData_t*> m_MagDataList;

		// ��Ĺ
		std::list<MagicData_t*> m_GraveList;

	};


}
