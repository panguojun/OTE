#include "OTEObjDataManager.h"
#include "OgreLogManager.h"
#include <windows.h>
#include "OTECommon.h"

namespace OTE
{
// ------------------------------------------------
COTEObjDataManager COTEObjDataManager::s_Inst;

COTEObjDataManager* COTEObjDataManager::GetInstance()
{	
	return &s_Inst;
}

void COTEObjDataManager::Destroy(void)
{
	s_Inst.ClearInfomation();
}

// ------------------------------------------------
//���ȫ����Ϣ
void COTEObjDataManager::ClearInfomation()
{
	std::map<std::string, ObjData_t*>::iterator it = m_ObjPathMap.begin();
	while(it != m_ObjPathMap.end())
	{
		ObjData_t *obi = it->second;

		obi->AniFileList.clear();

		std::list<HookData_t*>::iterator i = obi->HookList.begin();
		while(i != obi->HookList.end())
		{
			delete *i;
			i++;
		}
		obi->HookList.clear();

		delete obi;
		it++;
	}
	m_ObjPathMap.clear();
}

// ------------------------------------------------
//��ȡ��������

ObjData_t* COTEObjDataManager::GetObjData(const std::string& FileName)
{	
	std::map<std::string, ObjData_t*>::iterator it = m_ObjPathMap.find(FileName);
	if(it != m_ObjPathMap.end())
	{
		return it->second;
	}
	else
	{
		ObjData_t *oin = new ObjData_t();

		//  �ļ�������  TEMP ��������Ч����ʧ��������

		if(!Ogre::ResourceGroupManager::getSingleton().resourceExists(
															RES_GROUP, FileName))
		{				
			oin->MeshFile = FileName;
			if(oin->MeshFile.find(".obj") != std::string::npos)			
				oin->MeshFile.replace(oin->MeshFile.find(".obj"), 4, ".mesh");  	
			oin->bHasMesh = true;

			m_ObjPathMap[FileName] = oin;
			return oin;
		}	

		// ���ļ���Ϣ

		Ogre::String fileFullName;

		OTEHelper::GetFullPath(FileName, oin->FilePath, fileFullName);	

		TiXmlDocument xmlDoc;
		TiXmlElement *elem;

		if(!xmlDoc.LoadFile(fileFullName))
		{
			return NULL;
		}		
			
		const char* szVal = xmlDoc.RootElement()->Attribute("hasAnimation");
		
		if(szVal && strcmp(szVal, "true") == 0)
			oin->bHasSkeleton = true;

		//ȡMesh�ļ���
		elem = xmlDoc.RootElement()->FirstChildElement("mesh");
		if(elem)
		{
			oin->MeshFile = elem->Attribute("src");
			oin->bHasMesh = true;
		}
		else
		{
			oin->MeshFile = FileName;		
			oin->MeshFile.replace(oin->MeshFile.find(".obj"), 4, ".mesh");  
			oin->bHasMesh = false;
		}

		elem = xmlDoc.RootElement()->FirstChildElement("submeshes");	
		if(elem)
		{
			elem = elem->FirstChildElement("submesh");
			while(elem)
			{				
				oin->SubMeshList.push_back(elem->Attribute("src"));
				elem = elem->NextSiblingElement();
			}
		}			
		
		
		//ȡ�����ļ���
		if(oin->bHasSkeleton)
		{
			elem = xmlDoc.RootElement()->FirstChildElement("skeletonlink");
			if(elem)
				oin->SkeletonFile = elem->Attribute("name");
		}

		//��ȡ�����ļ��б�
		elem = xmlDoc.RootElement()->FirstChildElement("animations");
		if(elem)
		{
			elem = elem->FirstChildElement("animation");
			while(elem)
			{								
				oin->AniFileList[elem->Attribute("name")] = elem->Attribute("file");
				
				elem = elem->NextSiblingElement();
			}
		}		

		//ȡ�ҵ���Ϣ		
		
		Ogre::String hookFile = OTEHelper::GetOTESetting("Misc", "PotHookFile");
		Ogre::String filePath;
		Ogre::String fileFullName1;
		OTEHelper::GetFullPath(hookFile, filePath, fileFullName1);

		if(xmlDoc.LoadFile(fileFullName1))
		{
			elem = xmlDoc.RootElement()->FirstChildElement("PotHook");
			while(elem)
			{
				const char* attr = elem->Attribute("EntityId");		
				if(attr && FileName == attr)
				{
					const char *name = elem->Attribute("Name");
					const char *bone = elem->Attribute("Bone");
					
					Ogre::Vector3 vp;
					const char *val = elem->Attribute("OffestPos");
					if(val)
						sscanf(val, "%f %f %f", &(vp.x), &(vp.y), &(vp.z));

					Ogre::Quaternion qr;
					val = elem->Attribute("Rotation");					
					if(val)
						sscanf(val, "%f %f %f %f", &(qr.x), &(qr.y), &(qr.z), &(qr.w));

					Ogre::Vector3 vS;
					val = elem->Attribute("Scale");	
					if(val)
						sscanf(val, "%f %f %f", &(vS.x), &(vS.y), &(vS.z));

					OTE_TRACE(name << bone)

					HookData_t* hd = new HookData_t(name, bone, vp, qr, vS);
					oin->HookList.push_back(hd);
				}
				elem = elem->NextSiblingElement();
			}
		}
		else
		{
			OTE_TRACE("���عҵ�����ʧ��! :�ļ�û���ҵ� : " << fileFullName1)
		}
		
		//���ļ���Ϣ���뵽Map��
		m_ObjPathMap[FileName] = oin;

		return oin;
	}
}

// ------------------------------------------------
// �����������
void COTEObjDataManager::WriteObjData(const std::string& FileName, COTEActorEntity* pEntity)
{
	ObjData_t* oin = m_ObjPathMap[FileName];

    if(!oin || !pEntity)
		return;

	Ogre::String filePath;
	Ogre::String fileFullName;

	OTEHelper::GetFullPath(FileName, filePath, fileFullName);	

	TiXmlDocument doc("object");
	std::stringstream ss;

	if(pEntity->hasSkeleton())	
	{
		ss << "<object hasAnimation = \"true\">\n\t";

		// ��Mesh?

		if(oin->bHasMesh)
			ss << "<mesh src = " << "\"" << oin->MeshFile << "\"/>\n";

		ss << "<skeletonlink name = " << "\"" << pEntity->getMesh()->getSkeleton()->getName() << "\"/>\n";
	
		// ����ʵ����Ϣ

		short numanis = pEntity->getMesh()->getSkeleton()->getNumAnimations();

		std::string meshName = pEntity->getMesh()->getName();

		meshName.replace(meshName.find(".mesh"), 5, "");
		
		if(oin->SubMeshList.size() > 0)
		{
			std::list<std::string>::iterator it = oin->SubMeshList.begin();

			ss << "<submeshes>";
			while(it != oin->SubMeshList.end())
			{
				ss  << "<submesh src = \"" << (*it) << "\"/>";			
				++ it;
			}
			ss << "</submeshes>";
		}

		ss << "<animations>";	

		std::map<std::string, std::string>::iterator it = oin->AniFileList.begin();
		while(it != oin->AniFileList.end())
		{
			ss  << "<animation name = \"" 
				<< it->first
				<< "\" file = \"" << it->second << "\"/>";

			++ it;
		}		
		
		ss << "</animations>";
	}
	else	
	{
		ss << "<object hasAnimation = \"false\">\n\t";

		ss << "<mesh src = " << "\"" << pEntity->getMesh()->getName() <<"\"/>";

	}

	ss << "</object>";	

	doc.Parse(ss.str().c_str());
	doc.SaveFile(fileFullName);
	
}


}