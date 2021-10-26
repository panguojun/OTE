#include "tinyxml.h"
#include "OTEActorEntity.h"
#include "oteterrainserializer.h"
#include "OTESceneXmlLoader.h"

#include "OTEMagicManager.h"
#include "OTEFog.h"
#include "OTECommon.h"
#include "OTEMathLib.h"
#include "OTEParticlesystem.h"
#include "OTEVegManager.h"
#include "OTEActorActionMgr.h"
#include "OTETilePage.h"
#include "OTEEntityRender.h"

// -------------------------------------------------------------
using namespace OTE;
using namespace Ogre;

// -------------------------------------------------------------
COTESceneXmlLoader COTESceneXmlLoader::s_Inst;
COTESceneXmlLoader* COTESceneXmlLoader::GetInstance()
{	
	return &s_Inst;
}

// -------------------------------------------------------------
#define CASE(name, attr) if(attr && std::string(attr) == name)
#define BREAKCASE	continue
#define ELM_CASE(elmName)	subChild = NULL;	do{	subChild = elm->IterateChildren(elmName,subChild);	if(subChild){TiXmlElement* subElm = subChild->ToElement();const char* attrName = subElm->Attribute("name");	const char* attrVal = subElm->Attribute("val");
#define ELM_BREAKCASE		}}while(subChild);

bool COTESceneXmlLoader::LoadFromFile(const std::string& szFileName, bool needClearScene)
{
	Ogre::DataStreamPtr stream = RESMGR_LOAD(szFileName);
	
	if(stream.isNull())
	{
		OTE_LOGMSG("LoadFromFile û���ҵ��ļ�: " << szFileName)
		return false;
	}

	return ParseSceneXml(stream->getAsString(), needClearScene);
}

/* -------------------------------------------------------------

   �ļ���ʽ��
		
	<scene>
		<attribute>
			<string name="name" val="TestLevel" />
			<string name="ʱ��" val="9" />
			<vector3 name="������" val="0.521569 0.521569 0.521569 0.5" />
			<vector3 name="����" val="1 1 1 0.5" />
			<vector3 name="����" val="0.0509804 0.0509804 0.0509804 0.5" />		
			<vector3 name="����ɫ" val="1 1 1" />
			<float name="�����" val="0" />
			<float name="��Զ��" val="1000" />
			<float name="����ͼ" val="grassTexture.tga" />
		</attribute>
		<node type="terrain">
			<attribute>
				<string name="terrainFile" val="TestLevel.ter.xml" />
			</attribute>
		</node>
		<node type="entity">
			<attribute>
				<string name="name" val="Ent_aap00005.mesh_10" />
				<string name="File" val="aap00005.obj" />
				<vector3 name="position" val="56.0028 15.0357 38.5729" />
				<vector3 name="rotation" val="0 0 0 1" />
				<vector3 name="scaling" val="1 1 1" />
			</attribute>
		</node>    
   <scene>

	
   ------------------------------------------------------------- */

bool COTESceneXmlLoader::ParseSceneXml(const std::string& szXml, bool needClearScene)
{
	TiXmlDocument doc;
	
	doc.Parse(szXml.c_str());
	
	// ����ԭ���ĳ���

	if(needClearScene)
		SCENE_MGR->clearScene();

	// �����ڵ�

	TiXmlHandle docHandle( &doc );
	::TiXmlElement* rootElem = docHandle.FirstChildElement("scene").Element();
	if(!rootElem)
	{
		OTE_MSG("�ļ���ʽ����!", "����");
		return false;		
	}

	/// �����ļ�����

	// -------------------------------------------------------------
	// �����������������
	// -------------------------------------------------------------

	// ��ȡ��������

	ReadSceneAttributes(rootElem);

	OTE_LOGMSG("��������������ϣ�");

	// -------------------------------------------------------------
	// �����еĶ���
	// -------------------------------------------------------------
	
	ReadObjects(rootElem);

	return true;
}


// -------------------------------------------------------------
bool COTESceneXmlLoader::LoadObjectsFromFile(const std::string& szFileName,												 
											 bool isClearOld )
{
	TiXmlDocument doc(szFileName);
	
	if(!doc.LoadFile())
	{
		return false;		
	}
	
	// ����ԭ���ĳ���

	if(isClearOld)
		SCENE_MGR->RemoveAllEntities();

	// �����ڵ�

	TiXmlHandle docHandle(&doc);
	::TiXmlElement* rootElem = docHandle.FirstChildElement("scene").Element();
	if(!rootElem)
	{
		return false;		
	}

	// -------------------------------------------------------------
	// �����еĶ���
	// -------------------------------------------------------------

	ReadObjects(rootElem);

	return true;
}

// -------------------------------------------------------------
void COTESceneXmlLoader::ReadObjects(::TiXmlElement* rootElem)
{
	::TiXmlNode* child = NULL;
	do{			
		child = rootElem->IterateChildren("node",child);	
		TiXmlElement* elm = child->ToElement();
		if(elm)
		{
			const char* szType = elm->Attribute("type");
			CASE("terrain", szType)
			{
				ReadTerrainAttributes(elm);
				BREAKCASE;
			}
			CASE("entity", szType)
			{			
				ReadEntityAttributes(elm);
				BREAKCASE;
			}				
			CASE("particle", szType)
			{
				ReadMagicAttributes(elm);
				BREAKCASE;
			}		
			CASE("billboard", szType)
			{
				ReadMagicAttributes(elm);
				BREAKCASE;
			}					
			CASE("objectGroup", szType)
			{	
				::TiXmlNode* child = NULL;
				child = rootElem->IterateChildren("attribute", child);	
				TiXmlElement* elm = child->ToElement();

				if(elm)
				{
					::TiXmlNode* subChild = NULL;
					
					ELM_CASE("string")
					CASE("fileName", attrName)
					{
						LoadObjectsFromFile(attrName);
						BREAKCASE;
					}
					ELM_BREAKCASE;
				}

				BREAKCASE;
			}
			
		}
	}
	while(child);

}

// -------------------------------------------------------------
// ɽ������

void COTESceneXmlLoader::ReadTerrainAttributes(::TiXmlElement* rootElem, const std::string& pageName)
{
	::TiXmlNode* child = NULL;

	child = rootElem->IterateChildren("attribute", child);	
	TiXmlElement* elm = child->ToElement();

	if(elm)
	{
		::TiXmlNode* subChild = NULL;
		
		ELM_CASE("string")
		CASE("terrainFile", attrName)
		{
			if(attrVal)
			{
				if(!COTETilePage::s_CurrentPage)
				{
					COTETilePage::CreateTerrain(pageName, attrVal);
				}
				else
				{					
					COTETilePage::ImportTerrain(pageName, attrVal);
				}
							
			}				
		}

		ELM_BREAKCASE	
	}
}

// -------------------------------------------------------------
// ��������

void COTESceneXmlLoader::ReadSceneAttributes(::TiXmlElement* rootElem)
{	
	LightColor_t light_cor;
	
	Ogre::ColourValue fog_cor;
	float fog_minDis = 0;
	float fog_maxDis = 1000;
	std::string world_time = "9";
	std::string grassTexture = "";

	::TiXmlNode* child = NULL;
	do
	{
		child = rootElem->IterateChildren("attribute", child);	
		TiXmlElement* elm = child->ToElement();

		if(elm)
		{
			::TiXmlNode* subChild = NULL;
			
			ELM_CASE("string")
			CASE("name", attrName)	
			{					
				BREAKCASE;	
			}	
			CASE("ʱ��", attrName)	
			{
				world_time = attrVal;
				COTERenderLight::GetInstance()->SetCurrentTime(atof(world_time.c_str()));
				
				BREAKCASE;	
			}
			ELM_BREAKCASE			

			ELM_CASE("vector3")
			CASE("������", attrName)	
			{
				sscanf(attrVal, "%f %f %f %f", &light_cor.baseCor.r, &light_cor.baseCor.g, &light_cor.baseCor.b, &light_cor.gama); 
				COTERenderLight::GetInstance()->GetLightKey()->ambient = light_cor;
				BREAKCASE;	
			}	
			CASE("����", attrName)	
			{
				sscanf(attrVal, "%f %f %f %f", &light_cor.baseCor.r, &light_cor.baseCor.g, &light_cor.baseCor.b, &light_cor.gama);	
				COTERenderLight::GetInstance()->GetLightKey()->diffuse = light_cor;
				BREAKCASE;	
			}
			CASE("����", attrName)	
			{
				sscanf(attrVal, "%f %f %f %f", &light_cor.baseCor.r, &light_cor.baseCor.g, &light_cor.baseCor.b, &light_cor.gama); 	
				COTERenderLight::GetInstance()->GetLightKey()->reflect = light_cor;
				BREAKCASE;	
			}	
		
			CASE("����ɫ", attrName)	
			{
				sscanf(attrVal, "%f %f %f", &fog_cor.r, &fog_cor.g, &fog_cor.b);		
				BREAKCASE;	
			}
			ELM_BREAKCASE	

			ELM_CASE("float")
			CASE("�����", attrName)	
			{
				sscanf(attrVal, "%f", &fog_minDis);		
				BREAKCASE;	
			}	
			CASE("��Զ��", attrName)	
			{
				sscanf(attrVal, "%f", &fog_maxDis);		
				BREAKCASE;	
			}	
			CASE("����ͼ", attrName)	
			{
				grassTexture = attrVal;		
				BREAKCASE;	
			}	
			ELM_BREAKCASE

		}		

	}
	while(child);

	// ���ó���������
	
	COTEFog::GetInstance()->SetFogColor(fog_cor);
	COTEFog::GetInstance()->SetFogLinear(fog_minDis, fog_maxDis);

	// ��

	COTEVegManager::GetInstance()->SetGrassTexture(grassTexture);

}

// -------------------------------------------------------------
// ʵ������
void COTESceneXmlLoader::ReadEntityAttributes(::TiXmlElement* elem, 
											  const std::string& group)
{	
	::TiXmlNode* child = NULL;
	//do
	{
		std::string entityName;
		std::string meshId;
		std::string ActionName;
		std::string entSoundName;

		Ogre::Vector3 position;
		Ogre::Quaternion rotation;	
		Ogre::Vector3 scaling = Ogre::Vector3(1, 1, 1);

		child = elem->IterateChildren("attribute",child);	
		TiXmlElement* elm = child->ToElement();

		if(elm)
		{
			::TiXmlNode* subChild = NULL;
			
			// string
			ELM_CASE("string")
			CASE("name", attrName)	
			{
				if(attrVal)
					entityName = attrVal;		

				BREAKCASE;	
			}	
			CASE("File", attrName)	
			{
				if(attrVal)
					meshId = attrVal;		

				BREAKCASE;	
			}
			CASE("action", attrName)	
			{
				if(attrVal)
					ActionName = attrVal;		

				BREAKCASE;	
			}
			CASE("sound", attrName)	// ����
			{	
				if(attrVal)
					entSoundName = attrVal;			
				
				BREAKCASE;
			}		
			ELM_BREAKCASE	

			// vector3
			ELM_CASE("vector3")	

			CASE("position", attrName)	// λ��
			{									
				sscanf(attrVal, "%f %f %f", &position.x, &position.y, &position.z); 				

				BREAKCASE;	
			}	
			
			CASE("rotation", attrName)	// ��ת
			{									
				sscanf(attrVal, "%f %f %f %f", &rotation.x, &rotation.y, &rotation.z, &rotation.w); 				

				BREAKCASE;	
			}				
			
			CASE("scaling", attrName)	// ����
			{									
				sscanf(attrVal, "%f %f %f", &scaling.x, &scaling.y, &scaling.z); 				
				
				BREAKCASE;	
			}			

			ELM_BREAKCASE	
			
		}	
	
		COTEActorEntity* ca = SCENE_MGR->CreateEntity(meshId.c_str(), entityName.c_str(), position, rotation, scaling, group);
		//COTEActorEntity* ca = SCENE_MGR->CreateTerrainEntity(meshId.c_str(), entityName.c_str(), position, rotation, scaling);
	
		if(!ActionName.empty())
			COTEActorActionMgr::SetAction(ca, ActionName.c_str());
		
	}
	//while(child);
}

// -------------------------------------------------------------
// ��Ч����

void COTESceneXmlLoader::ReadMagicAttributes(::TiXmlElement* elem)
{
	::TiXmlNode* child = NULL;
	//do
	{
		std::string name;
		std::string resName;
		std::string soundName;		
		Ogre::Vector3 position;
		Ogre::Quaternion rotation;
		Ogre::Vector3 scaling = Ogre::Vector3(1, 1, 1);

		child = elem->IterateChildren("attribute",child);	
		TiXmlElement* elm = child->ToElement();

		if(elm)
		{
			::TiXmlNode* subChild = NULL;
			
			// string
			ELM_CASE("string")

			CASE("name", attrName)	// ʵ����
			{
				if(attrVal)
					name = attrVal;		

				BREAKCASE;	
			}	
			CASE("File", attrName)	// ��Դ��
			{
				if(attrVal)
					resName = attrVal;		

				BREAKCASE;	
			}	

			CASE("sound", attrName)	// ����
			{	
				if(attrVal)
					soundName = attrVal;			
				
				BREAKCASE;
			}		

			ELM_BREAKCASE	

			// vector3
			ELM_CASE("vector3")
			CASE("position", attrName)	// λ��
			{									
				sscanf(attrVal, "%f %f %f", &position.x, &position.y, &position.z); 				

				BREAKCASE;	
			}	
			CASE("rotation", attrName)	// ��ת
			{									
				sscanf(attrVal, "%f %f %f %f", &rotation.x, &rotation.y, &rotation.z, &rotation.w); 				

				BREAKCASE;	
			}	
			CASE("scaling", attrName)	// ����
			{									
				sscanf(attrVal, "%f %f %f", &scaling.x, &scaling.y, &scaling.z); 				

				BREAKCASE;	
			}	

			ELM_BREAKCASE	
			
		}	

		//create magic

		if(!name.empty() && !resName.empty())
		{
			Ogre::MovableObject* mo = MAGIC_MGR->CreateMagic(resName, name, false);		
			if(mo)
			{
				mo->getParentSceneNode()->setPosition(position);
				mo->getParentSceneNode()->setOrientation(rotation);
				mo->getParentSceneNode()->setScale(scaling);				
			}
			
		}
	}
	//while(child);
}

// -------------------------------------------------------------
// ����

void COTESceneXmlLoader::SaveToFile(const std::string& szFileName)
{
	LightColor_t cor;
    float fval;

	TiXmlDocument doc("scene");
	std::stringstream ss;
	ss << "<scene>\n\t";

	// ���泡����������
	
	ss << "<attribute>\n\t\t";
		
	ss << "<string name=\"name\" val=\""			<< (COTETilePage::s_CurrentPage ? COTETilePage::s_CurrentPage->m_TerrainName : szFileName) << "\"/>";
		cor = COTERenderLight::GetInstance()->GetLightKey()->ambient;
		ss << "<string name=\"ʱ��\" val=\""		<< 0/*COTERenderLight::GetInstance()->m_CurrentTime*/ <<"\"/>";
		
		ss << "<vector3 name=\"������\" val=\""		<< cor.baseCor.r <<" " << cor.baseCor.g << " " << cor.baseCor.b << " " << cor.gama <<"\"/>";
		cor = COTERenderLight::GetInstance()->GetLightKey()->diffuse;
		ss << "<vector3 name=\"����\" val=\""		<< cor.baseCor.r <<" " << cor.baseCor.g << " " << cor.baseCor.b << " " << cor.gama <<"\"/>";
		cor = COTERenderLight::GetInstance()->GetLightKey()->reflect;
		ss << "<vector3 name=\"����\" val=\""		<< cor.baseCor.r <<" " << cor.baseCor.g << " " << cor.baseCor.b << " " << cor.gama <<"\"/>";
		
		Ogre::ColourValue fogCor = COTEFog::GetInstance()->m_ForColour;
		ss << "<vector3 name=\"����ɫ\" val=\"" << fogCor.r <<" " << fogCor.g << " " << fogCor.b <<"\"/>";
		fval = COTEFog::GetInstance()->m_FogStart;
		ss << "<float name=\"�����\" val=\"" << fval <<"\"/>";
		fval = COTEFog::GetInstance()->m_FogEnd;
		ss << "<float name=\"��Զ��\" val=\"" << fval <<"\"/>";

		ss << "<float name=\"����ͼ\" val=\"" << COTEVegManager::GetInstance()->GetGrassTextureName() <<"\"/>";

	ss << "</attribute>\n\t\t";
		
	if(COTETilePage::s_CurrentPage)
	{
		// ����ɽ��

		ss << "<node type=\"terrain\">";
		ss << "<attribute>\n\t\t";

		std::string terName = COTETilePage::s_CurrentPage->m_TerrainName;
		
		ss << "<string name=\"terrainFile\" val=\"" << terName << ".ter.xml" << "\"/>";
					
		ss << "</attribute>\n\t\t";
		ss << "</node>";
	}

	// ʵ����Ϣ

	std::vector<Ogre::MovableObject*> eList;
	SCENE_MGR->GetEntityList(&eList);
	for(unsigned int i = 0; i < eList.size(); ++ i)
	{			
/*		if( eList[i]->getName().find("Ent") == std::string::npos)
		{
			continue;
		}*/	

		COTEActorEntity* actor = (COTEActorEntity*)eList[i];

		ss << "<node type=\"entity\">";
			ss << "<attribute>\n\t\t";

				ss << "<string name=\"name\" val=\"" << eList[i]->getName() << "\"/>";
				ss << "<string name=\"File\" val=\"" << actor->m_ResFile << "\"/>";
				if(/*((COTEActorEntity*)eList[i])->hasSkeleton() && */((COTEActorEntity*)eList[i])->m_CurAction != NULL)
                    ss << "<string name=\"action\" val=\"" << actor->m_CurAction->GetName() << "\"/>";
				Ogre::SceneNode* n = (Ogre::SceneNode*)eList[i]->getParentNode();
				
				if(actor->m_pTrackOrgSceneNode)			
					n = actor->m_pTrackOrgSceneNode;			
				
				ss << "<vector3 name=\"position\" val=\"" << n->getPosition().x <<" " << n->getPosition().y << " " << n->getPosition().z <<"\"/>";
				ss << "<vector3 name=\"rotation\" val=\"" << n->getOrientation().x <<" " << n->getOrientation().y << " " << n->getOrientation().z << " " << n->getOrientation().w << "\"/>";
				ss << "<vector3 name=\"scaling\" val=\"" << n->getScale().x <<" " << n->getScale().y << " " << n->getScale().z <<"\"/>";
							
			ss << "</attribute>\n\t\t";
		ss << "</node>";
	}	

	// ����Ч��

	{
		HashMap<std::string, COTEParticleSystem*>& pss = COTEParticleSystemManager::GetInstance()->m_ParticleSystems;
		HashMap<std::string, COTEParticleSystem*>::iterator it = pss.begin();
		while(it != pss.end())
		{
			COTEParticleSystem* ps = (*it).second;
			Ogre::Vector3 v = Ogre::Vector3::ZERO;
			Ogre::Quaternion q = Ogre::Quaternion::IDENTITY;

			ss << "<node type=\"particle\">";
				ss << "<attribute>\n\t\t";
					
					ss << "<string name=\"name\" val=\"" << ps->getName() << "\"/>";
					ss << "<string name=\"File\" val=\"" << ps->GetResPtr()->m_Name<< "\"/>";
					v = ps->getParentNode()->getWorldPosition();
					ss << "<vector3 name=\"position\" val=\"" << v.x <<" " << v.y << " " << v.z <<"\"/>";
					q = ps->getParentNode()->getWorldOrientation();
					ss << "<vector3 name=\"rotation\" val=\"" << q.x <<" " << q.y << " " << q.z << " " << q.w << "\"/>";
					v = ps->getParentNode()->getScale();
					ss << "<vector3 name=\"scaling\" val=\"" << v.x <<" " << v.y << " " << v.z <<"\"/>";
				
				ss << "</attribute>\n\t\t";
			ss << "</node>";

			++ it;
		}
	}

	// ������Ч��

	/*{
		HashMap<std::string, COTEParticleSystem*>& pss = SCENE_MGR->m_ParticleSystems;
		HashMap<std::string, COTEParticleSystem*>::iterator it = pss.begin();
		while(it != pss.end())
		{
			COTEParticleSystem* ps = (*it).second;
			Ogre::Vector3 v = Ogre::Vector3::ZERO;
			Ogre::Quaternion q = Ogre::Quaternion::IDENTITY;

			ss << "<node type=\"billboard\">";
				ss << "<attribute>\n\t\t";
					
					ss << "<string name=\"name\" val=\"" << ps->getName() << "\"/>";
					ss << "<string name=\"File\" val=\"" << ps->GetResPtr()->m_Name<< "\"/>";
					v = ps->getParentNode()->getWorldPosition();
					ss << "<vector3 name=\"position\" val=\"" << v.x <<" " << v.y << " " << v.z <<"\"/>";
					q = ps->getParentNode()->getWorldOrientation();
					ss << "<vector3 name=\"rotation\" val=\"" << q.x <<" " << q.y << " " << q.z << " " << q.w << "\"/>";
					v = ps->getParentNode()->getScale();
					ss << "<vector3 name=\"scaling\" val=\"" << v.x <<" " << v.y << " " << v.z <<"\"/>";
				
				ss << "</attribute>\n\t\t";
			ss << "</node>";

			++ it;
		}
	}*/

	doc.Parse(ss.str().c_str());
	doc.SaveFile(szFileName);
}

// -------------------------------------------------------------
// ���� ʵ����Ϣ

void COTESceneXmlLoader::ExportEntityInfo(const std::string& fileName, const std::string& type)
{	
	TiXmlDocument doc("EntityList");
	std::stringstream ss;
	ss << "<EntityList>\n\t";

	// ����ʵ����Ϣ

	std::vector<Ogre::MovableObject*> eList;
	SCENE_MGR->GetEntityList(&eList);
	for(unsigned int i = 0; i < eList.size(); ++ i)
	{			
		if(eList[i]->getName().find(type) == std::string::npos)
		{
			continue;
		}	

		ss << "<node type=\"entity\">";
			ss << "<attribute>\n\t\t";

				ss << "<string name=\"name\" val=\"" << eList[i]->getName() << "\"/>";
				ss << "<string name=\"File\" val=\"" << ((COTEActorEntity*)eList[i])->m_ResFile << "\"/>";
				if(/*((COTEActorEntity*)eList[i])->hasSkeleton() && */((COTEActorEntity*)eList[i])->m_CurAction != NULL)
                    ss << "<string name=\"action\" val=\"" << ((COTEActorEntity*)eList[i])->m_CurAction->GetName() << "\"/>";
				Ogre::SceneNode* n = (Ogre::SceneNode*)eList[i]->getParentNode();
				ss << "<vector3 name=\"position\" val=\"" << n->getPosition().x <<" " << n->getPosition().y << " " << n->getPosition().z <<"\"/>";				
				ss << "<vector4 name=\"rotation\" val=\"" << n->getOrientation().x <<" " << n->getOrientation().y << " " << n->getOrientation().z << " " << n->getOrientation().w << "\"/>";
				ss << "<vector3 name=\"scaling\" val=\"" << n->getScale().x <<" " << n->getScale().y << " " << n->getScale().z <<"\"/>";
				ss << "<float name=\"angle\" val=\"" << MathLib::FixRadian( MathLib::GetRadian(n->getOrientation() * Ogre::Vector3::UNIT_Z ) ).valueDegrees() <<"\"/>";
				
			ss << "</attribute>\n\t\t";
		ss << "</node>";
	}
	

	doc.Parse(ss.str().c_str());
	doc.SaveFile(fileName);
}

// -------------------------------------------------------------
// ���볡������

bool COTESceneXmlLoader::ImportSceneFromFile(const std::string& szFileName, 
											  const std::string& group)
{
	TiXmlDocument doc(szFileName);
	
	if(!doc.LoadFile())
	{
		//MessageBox(NULL, "�ļ�û���ҵ�!", "����", MB_OK);
		return false;		
	}

	// �����ڵ�

	TiXmlHandle docHandle( &doc );
	::TiXmlElement* rootElem = docHandle.FirstChildElement("scene").Element();
	if(!rootElem)
	{
//			MessageBox(NULL, "�ļ���ʽ����!", "����", MB_ICONERROR);
		return false;		
	}
	
	/// �����ļ�����

	// -------------------------------------------------------------
	// �����еĶ���
	// -------------------------------------------------------------

	::TiXmlNode* child = NULL;
	do{			
		child = rootElem->IterateChildren("node",child);	
		TiXmlElement* elm = child->ToElement();
		if(elm)
		{
			const char* szType = elm->Attribute("type");
			CASE("terrain", szType)
			{
				ReadTerrainAttributes(elm, group);
				BREAKCASE;
			}
			CASE("entity", szType)
			{
				ReadEntityAttributes(elm, group);
				BREAKCASE;
			}		
		}
	}
	while(child);

	return true;
}

// -------------------------------------------------------------
// ���߼�ʵ�壨npc����������
// -------------------------------------------------------------
bool COTESceneXmlLoader::ImportNPCFromFile(const std::string& szFileName)
{
	int  id;
	char name[32];
	float  position[2];
	int  angle;
	char buff[256];

	FILE* file = fopen(szFileName.c_str(), "r");
	while(fgets(buff, 256, file))
	{
		sscanf(buff, "%d, %f, %f, %d, %s\n", &id, &position[0], &position[1], &angle, name);
		
		Ogre::Vector3 position3d(position[0], 0, position[1]);
		if(COTETilePage::s_CurrentPage)
			position3d.y = COTETilePage::s_CurrentPage->GetHeightAt(position3d.x, position3d.z);

		std::stringstream ss;
		ss << "IsNpc_" << id << "_" << name;
		Ogre::Quaternion q(- Ogre::Radian(Ogre::Degree(angle - 90)), Ogre::Vector3::UNIT_Y);		

		COTEActorEntity* ca = SCENE_MGR->CreateEntity("��ͷ��ŮNPC.obj", ss.str().c_str(), 
									position3d, 
									q, 
									Ogre::Vector3::UNIT_SCALE);	
	}
	fclose(file);

	return true;
}

// -------------------------------------------------------------
// ����Npc����
bool COTESceneXmlLoader::ExportNPCToFile(const std::string& szFileName)
{
	int  id;
	char name[32] = {0};
	float  position[2];
	int  angle;

	FILE* file = fopen(szFileName.c_str(), "w");	
	std::vector<Ogre::MovableObject*> eList;
	SCENE_MGR->GetEntityList(&eList);
	for(unsigned int i = 0; i < eList.size(); ++ i)
	{			
		Ogre::MovableObject* m = eList[i];
		std::string ename = eList[i]->getName();
		if(ename.find("IsNpc") == std::string::npos)		
			continue;
					
		sscanf(ename.c_str(), "IsNpc_%d_%s", &id, name);		
		position[0] = m->getParentSceneNode()->getPosition().x;
		position[1] = m->getParentSceneNode()->getPosition().z;

		angle = (int)MathLib::FixRadian( MathLib::GetRadian(m->getParentSceneNode()->getOrientation() * Ogre::Vector3::UNIT_Z) ).valueDegrees();

		fprintf(file, "%d, %.2f, %.2f, %d, %s\n", id, position[0], position[1], angle, name);
		
	}

	fclose(file);

	return true;
}

// -------------------------------------------------------------
// ���Դ��Ϣ���ļ�����
// -------------------------------------------------------------

bool COTESceneXmlLoader::ImportPtLgtFromFile(const std::string& szFileName)
{
	static const std::string c_light_helper_mesh = "lighthelper.mesh"; // �ƹ��������

	TiXmlDocument doc(szFileName);
	
	if(!doc.LoadFile())
	{
		return false;		
	}

	// �����ڵ�

	TiXmlHandle docHandle( &doc );
	::TiXmlElement* rootElem = docHandle.FirstChildElement("PtLgtList").Element();
	if(!rootElem)
	{
		return false;		
	}

	::TiXmlNode* child1 = NULL;
	do
	{
		child1 = rootElem->IterateChildren("node",child1);			
		if(child1)
		{
			TiXmlElement* elem = child1->ToElement();
			Ogre::Vector3		position;		
			Ogre::ColourValue	diffuse;	
		
			float				constant;
			float				linear;		
			float				quadratic;

			::TiXmlNode* child = NULL;
			child = elem->IterateChildren("attribute",child);	
			if(child)		
			{
				TiXmlElement* elm = child->ToElement();
				::TiXmlNode* subChild = NULL;				

				// float

				ELM_CASE("float")	

				CASE("constant", attrName)		// ����
				{									
					sscanf(attrVal, "%f", &constant); 				

					BREAKCASE;	
				}		
						
				CASE("linear", attrName)		// һ��ϵ��
				{									
					sscanf(attrVal, "%f", &linear); 				

					BREAKCASE;	
				}	

				CASE("quadratic", attrName)		// ����ϵ��
				{									
					sscanf(attrVal, "%f", &quadratic); 				

					BREAKCASE;	
				}	

				ELM_BREAKCASE	

				// vector3

				ELM_CASE("vector3")	

				CASE("position", attrName)	// λ��
				{									
					sscanf(attrVal, "%f %f %f", &position.x, &position.y, &position.z); 				

					BREAKCASE;	
				}				
				CASE("diffuse", attrName)		// ��ɫ
				{									
					sscanf(attrVal, "%f %f %f", &diffuse.r, &diffuse.g, &diffuse.b); 				

					BREAKCASE;	
				}		
				ELM_BREAKCASE				
				
			}	
		
			COTEActorEntity* ptlgt = SCENE_MGR->AutoCreateEntity(c_light_helper_mesh, "Lgt");
		
			ptlgt->getParentSceneNode()->setPosition(position);
			ptlgt->m_DymProperty.AddProperty("diffuse", diffuse);
			ptlgt->m_DymProperty.AddProperty("constant", constant);
			ptlgt->m_DymProperty.AddProperty("linear", linear);
			ptlgt->m_DymProperty.AddProperty("quadratic", quadratic);

		}
	}
	while(child1);

	return true;
}
// -------------------------------------------------------------
bool COTESceneXmlLoader::ImportCamPathFromFile(const std::string& szFileName)
{
	static const std::string c_light_helper_mesh = "��Ӱ��.mesh"; // �������������

	TiXmlDocument doc(szFileName);
	
	if(!doc.LoadFile())
	{
		return false;		
	}

	// �����ڵ�

	TiXmlHandle docHandle( &doc );
	::TiXmlElement* rootElem = docHandle.FirstChildElement("CamPath").Element();
	if(!rootElem)
	{
		return false;		
	}

	::TiXmlNode* child1 = NULL;
	do
	{
		child1 = rootElem->IterateChildren("node",child1);			
		if(child1)
		{
			TiXmlElement* elem = child1->ToElement();
			Ogre::Vector3		position;		
			Ogre::Quaternion	q;
			float speed;
			::TiXmlNode* child = NULL;
			child = elem->IterateChildren("attribute",child);	
			if(child)		
			{
				TiXmlElement* elm = child->ToElement();
				::TiXmlNode* subChild = NULL;				

				// float

				ELM_CASE("float")	

				CASE("speed", attrName)			// �ٶ�
				{									
					sscanf(attrVal, "%f", &speed); 				

					BREAKCASE;	
				}	

				ELM_BREAKCASE	

				// vector3

				ELM_CASE("vector3")	

				CASE("position", attrName)		// λ��
				{									
					sscanf(attrVal, "%f %f %f", &position.x, &position.y, &position.z); 				

					BREAKCASE;	
				}				
				CASE("quaternion", attrName)	// ��ת	
				{									
					sscanf(attrVal, "%f %f %f %f", &q.x, &q.y, &q.z, &q.w); 				

					BREAKCASE;	
				}		
				ELM_BREAKCASE				
				
			}	
		
			COTEActorEntity* e = SCENE_MGR->AutoCreateEntity(c_light_helper_mesh, "Cam");
		
			e->getParentSceneNode()->setPosition(position);
			e->getParentSceneNode()->setOrientation(q);
			e->m_DymProperty.AddProperty("speed", speed);

		}
	}
	while(child1);

	return true;
}
// -------------------------------------------------------------
bool COTESceneXmlLoader::ExportCamPathToFile(const std::string& szFileName)
{
	TiXmlDocument doc("CamPath");
	std::stringstream ss;
	ss << "<CamPath>\n\t";

	// ����ʵ����Ϣ

	std::vector<Ogre::MovableObject*> eList;
	SCENE_MGR->GetEntityList(&eList);
	for(unsigned int i = 0; i < eList.size(); ++ i)
	{			
		if(eList[i]->getName().find("Cam_") == std::string::npos)
		{
			continue;
		}	

		// �������Ϣ

		COTEActorEntity* ptCam = (COTEActorEntity*)eList[i];

		const Ogre::Vector3& rPos = ptCam->GetParentSceneNode()->getPosition();
		const Ogre::Quaternion& rQ = ptCam->GetParentSceneNode()->getOrientation();

		float speed = ptCam->m_DymProperty.GetfloatVal("speed");

		ss << "<node type=\"cam point\">";
			ss << "<attribute>\n\t\t";			
				
				ss << "<vector3 name=\"position\" val=\""	<< rPos.x <<" " << rPos.y << " " << rPos.z <<"\"/>";				
				ss << "<vector3 name=\"quaternion\" val=\""	<< rQ.x <<" " << rQ.y << " " << rQ.z << " " << rQ.w << "\"/>";				
				ss << "<float name=\"speed\" val=\""	<< speed <<"\"/>";				
						
			ss << "</attribute>\n\t\t";
		ss << "</node>";
	}	

	doc.Parse(ss.str().c_str());
	doc.SaveFile(szFileName);

	return true;
}
// -------------------------------------------------------------
// �������Դ����

bool COTESceneXmlLoader::ExportPtLgtToFile(const std::string& szFileName)
{
	TiXmlDocument doc("PtLgtList");
	std::stringstream ss;
	ss << "<PtLgtList>\n\t";

	// ����ʵ����Ϣ

	std::vector<Ogre::MovableObject*> eList;
	SCENE_MGR->GetEntityList(&eList);
	for(unsigned int i = 0; i < eList.size(); ++ i)
	{			
		if(eList[i]->getName().find("Lgt_") == std::string::npos)
		{
			continue;
		}	

		// ���Դ��Ϣ

		COTEActorEntity* ptLgt = (COTEActorEntity*)eList[i];

		Ogre::Vector3 lightPos = ptLgt->GetParentSceneNode()->getPosition();

		// ������Դ��ɢǿ��
		
		float constant	= ptLgt->m_DymProperty.GetfloatVal("constant");
		float linear	= ptLgt->m_DymProperty.GetfloatVal("linear");
		float quadratic	= ptLgt->m_DymProperty.GetfloatVal("quadratic");
		
		// ����ɫ

		Ogre::ColourValue diffuse = ptLgt->m_DymProperty.GetfloatVal("diffuse");

		ss << "<node type=\"pointLight\">";
			ss << "<attribute>\n\t\t";			
				
				ss << "<vector3 name=\"position\" val=\""	<< lightPos.x <<" " << lightPos.y << " " << lightPos.z <<"\"/>";				
				ss << "<float name=\"constant\" val=\""		<< constant <<"\"/>";				
				ss << "<float name=\"linear\" val=\""		<< linear <<"\"/>";
				ss << "<float name=\"quadratic\" val=\""	<< quadratic <<"\"/>";
				ss << "<vector3 name=\"diffuse\" val=\""	<< diffuse.r << " " << diffuse.g << " " << diffuse.b <<"\"/>";				
						
			ss << "</attribute>\n\t\t";
		ss << "</node>";
	}
	

	doc.Parse(ss.str().c_str());
	doc.SaveFile(szFileName);

	return true;
}
