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
	// OBJ 文件管理器
	// ********************************************************
	class _OTEExport COTEObjDataManager
	{		
	public:
		
		static COTEObjDataManager* GetInstance();	

		static void Destroy(void);

		// 获取文件信息

		ObjData_t* GetObjData(const std::string& FileName);

		// 保存文件信息
		void WriteObjData(const std::string& FileName, COTEActorEntity* pEntity);

		// 清空全部信息

		void ClearInfomation();		

	protected:

		static COTEObjDataManager s_Inst;

		std::map<std::string, ObjData_t*> m_ObjPathMap;	

	};

}