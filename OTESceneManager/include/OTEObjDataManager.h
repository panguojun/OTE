#pragma once

#include "OTEStaticInclude.h"
#include "OgreVector3.h"
#include "OgreSingleton.h"
#include "tinyxml.h"
#include "OTEActorEntity.h"

namespace OTE
{
	// ********************************************************
    // COTEObjDataManager
	// OBJ �ļ�������
	// ********************************************************
	class _OTEExport COTEObjDataManager
	{		
	public:
		
		static COTEObjDataManager* GetInstance();	

		static void Destroy(void);

		// ��ȡ�ļ���Ϣ

		ObjData_t* GetObjData(const std::string& FileName);

		// �����ļ���Ϣ
		void WriteObjData(const std::string& FileName, COTEActorEntity* pEntity);

		// ���ȫ����Ϣ

		void ClearInfomation();		

	protected:

		static COTEObjDataManager s_Inst;

		std::map<std::string, ObjData_t*> m_ObjPathMap;	

	};

}