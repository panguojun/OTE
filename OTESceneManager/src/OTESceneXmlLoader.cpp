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
		OTE_LOGMSG("LoadFromFile 没有找到文件: " << szFileName)
		return false;
	}

	return ParseSceneXml(stream->getAsString(), needClearScene);
}

/* -------------------------------------------------------------

   文件格式：
		
	<scene>
		<attribute>
			<string name="name" val="TestLevel" />
			<string name="时间" val="9" />
			<vector3 name="环境光" val="0.521569 0.521569 0.521569 0.5" />
			<vector3 name="主光" val="1 1 1 0.5" />
			<vector3 name="补光" val="0.0509804 0.0509804 0.0509804 0.5" />		
			<vector3 name="雾颜色" val="1 1 1" />
			<float name="雾近距" val="0" />
			<float name="雾远距" val="1000" />
			<float name="草贴图" val="grassTexture.tga" />
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
	
	// 清理到原来的场景

	if(needClearScene)
		SCENE_MGR->clearScene();

	// 检查根节点

	TiXmlHandle docHandle( &doc );
	::TiXmlElement* rootElem = docHandle.FirstChildElement("scene").Element();
	if(!rootElem)
	{
		OTE_MSG("文件格式错误!", "错误");
		return false;		
	}

	/// 分析文件数据

	// -------------------------------------------------------------
	// 场景整体的属性配置
	// -------------------------------------------------------------

	// 读取场景数据

	ReadSceneAttributes(rootElem);

	OTE_LOGMSG("场景属性设置完毕！");

	// -------------------------------------------------------------
	// 场景中的对象
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
	
	// 清理到原来的场景

	if(isClearOld)
		SCENE_MGR->RemoveAllEntities();

	// 检查根节点

	TiXmlHandle docHandle(&doc);
	::TiXmlElement* rootElem = docHandle.FirstChildElement("scene").Element();
	if(!rootElem)
	{
		return false;		
	}

	// -------------------------------------------------------------
	// 场景中的对象
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
// 山体属性

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
// 场景属性

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
			CASE("时间", attrName)	
			{
				world_time = attrVal;
				COTERenderLight::GetInstance()->SetCurrentTime(atof(world_time.c_str()));
				
				BREAKCASE;	
			}
			ELM_BREAKCASE			

			ELM_CASE("vector3")
			CASE("环境光", attrName)	
			{
				sscanf(attrVal, "%f %f %f %f", &light_cor.baseCor.r, &light_cor.baseCor.g, &light_cor.baseCor.b, &light_cor.gama); 
				COTERenderLight::GetInstance()->GetLightKey()->ambient = light_cor;
				BREAKCASE;	
			}	
			CASE("主光", attrName)	
			{
				sscanf(attrVal, "%f %f %f %f", &light_cor.baseCor.r, &light_cor.baseCor.g, &light_cor.baseCor.b, &light_cor.gama);	
				COTERenderLight::GetInstance()->GetLightKey()->diffuse = light_cor;
				BREAKCASE;	
			}
			CASE("补光", attrName)	
			{
				sscanf(attrVal, "%f %f %f %f", &light_cor.baseCor.r, &light_cor.baseCor.g, &light_cor.baseCor.b, &light_cor.gama); 	
				COTERenderLight::GetInstance()->GetLightKey()->reflect = light_cor;
				BREAKCASE;	
			}	
		
			CASE("雾颜色", attrName)	
			{
				sscanf(attrVal, "%f %f %f", &fog_cor.r, &fog_cor.g, &fog_cor.b);		
				BREAKCASE;	
			}
			ELM_BREAKCASE	

			ELM_CASE("float")
			CASE("雾近距", attrName)	
			{
				sscanf(attrVal, "%f", &fog_minDis);		
				BREAKCASE;	
			}	
			CASE("雾远距", attrName)	
			{
				sscanf(attrVal, "%f", &fog_maxDis);		
				BREAKCASE;	
			}	
			CASE("草贴图", attrName)	
			{
				grassTexture = attrVal;		
				BREAKCASE;	
			}	
			ELM_BREAKCASE

		}		

	}
	while(child);

	// 设置场景的属性
	
	COTEFog::GetInstance()->SetFogColor(fog_cor);
	COTEFog::GetInstance()->SetFogLinear(fog_minDis, fog_maxDis);

	// 草

	COTEVegManager::GetInstance()->SetGrassTexture(grassTexture);

}

// -------------------------------------------------------------
// 实体属性
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
			CASE("sound", attrName)	// 声音
			{	
				if(attrVal)
					entSoundName = attrVal;			
				
				BREAKCASE;
			}		
			ELM_BREAKCASE	

			// vector3
			ELM_CASE("vector3")	

			CASE("position", attrName)	// 位置
			{									
				sscanf(attrVal, "%f %f %f", &position.x, &position.y, &position.z); 				

				BREAKCASE;	
			}	
			
			CASE("rotation", attrName)	// 旋转
			{									
				sscanf(attrVal, "%f %f %f %f", &rotation.x, &rotation.y, &rotation.z, &rotation.w); 				

				BREAKCASE;	
			}				
			
			CASE("scaling", attrName)	// 缩放
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
// 特效属性

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

			CASE("name", attrName)	// 实体名
			{
				if(attrVal)
					name = attrVal;		

				BREAKCASE;	
			}	
			CASE("File", attrName)	// 资源名
			{
				if(attrVal)
					resName = attrVal;		

				BREAKCASE;	
			}	

			CASE("sound", attrName)	// 声音
			{	
				if(attrVal)
					soundName = attrVal;			
				
				BREAKCASE;
			}		

			ELM_BREAKCASE	

			// vector3
			ELM_CASE("vector3")
			CASE("position", attrName)	// 位置
			{									
				sscanf(attrVal, "%f %f %f", &position.x, &position.y, &position.z); 				

				BREAKCASE;	
			}	
			CASE("rotation", attrName)	// 旋转
			{									
				sscanf(attrVal, "%f %f %f %f", &rotation.x, &rotation.y, &rotation.z, &rotation.w); 				

				BREAKCASE;	
			}	
			CASE("scaling", attrName)	// 缩放
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
// 保存

void COTESceneXmlLoader::SaveToFile(const std::string& szFileName)
{
	LightColor_t cor;
    float fval;

	TiXmlDocument doc("scene");
	std::stringstream ss;
	ss << "<scene>\n\t";

	// 保存场景基本属性
	
	ss << "<attribute>\n\t\t";
		
	ss << "<string name=\"name\" val=\""			<< (COTETilePage::s_CurrentPage ? COTETilePage::s_CurrentPage->m_TerrainName : szFileName) << "\"/>";
		cor = COTERenderLight::GetInstance()->GetLightKey()->ambient;
		ss << "<string name=\"时间\" val=\""		<< 0/*COTERenderLight::GetInstance()->m_CurrentTime*/ <<"\"/>";
		
		ss << "<vector3 name=\"环境光\" val=\""		<< cor.baseCor.r <<" " << cor.baseCor.g << " " << cor.baseCor.b << " " << cor.gama <<"\"/>";
		cor = COTERenderLight::GetInstance()->GetLightKey()->diffuse;
		ss << "<vector3 name=\"主光\" val=\""		<< cor.baseCor.r <<" " << cor.baseCor.g << " " << cor.baseCor.b << " " << cor.gama <<"\"/>";
		cor = COTERenderLight::GetInstance()->GetLightKey()->reflect;
		ss << "<vector3 name=\"补光\" val=\""		<< cor.baseCor.r <<" " << cor.baseCor.g << " " << cor.baseCor.b << " " << cor.gama <<"\"/>";
		
		Ogre::ColourValue fogCor = COTEFog::GetInstance()->m_ForColour;
		ss << "<vector3 name=\"雾颜色\" val=\"" << fogCor.r <<" " << fogCor.g << " " << fogCor.b <<"\"/>";
		fval = COTEFog::GetInstance()->m_FogStart;
		ss << "<float name=\"雾近距\" val=\"" << fval <<"\"/>";
		fval = COTEFog::GetInstance()->m_FogEnd;
		ss << "<float name=\"雾远距\" val=\"" << fval <<"\"/>";

		ss << "<float name=\"草贴图\" val=\"" << COTEVegManager::GetInstance()->GetGrassTextureName() <<"\"/>";

	ss << "</attribute>\n\t\t";
		
	if(COTETilePage::s_CurrentPage)
	{
		// 保存山体

		ss << "<node type=\"terrain\">";
		ss << "<attribute>\n\t\t";

		std::string terName = COTETilePage::s_CurrentPage->m_TerrainName;
		
		ss << "<string name=\"terrainFile\" val=\"" << terName << ".ter.xml" << "\"/>";
					
		ss << "</attribute>\n\t\t";
		ss << "</node>";
	}

	// 实体信息

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

	// 粒子效果

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

	// 公告牌效果

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
// 导出 实体信息

void COTESceneXmlLoader::ExportEntityInfo(const std::string& fileName, const std::string& type)
{	
	TiXmlDocument doc("EntityList");
	std::stringstream ss;
	ss << "<EntityList>\n\t";

	// 保存实体信息

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
// 导入场景数据

bool COTESceneXmlLoader::ImportSceneFromFile(const std::string& szFileName, 
											  const std::string& group)
{
	TiXmlDocument doc(szFileName);
	
	if(!doc.LoadFile())
	{
		//MessageBox(NULL, "文件没有找到!", "错误", MB_OK);
		return false;		
	}

	// 检查根节点

	TiXmlHandle docHandle( &doc );
	::TiXmlElement* rootElem = docHandle.FirstChildElement("scene").Element();
	if(!rootElem)
	{
//			MessageBox(NULL, "文件格式错误!", "错误", MB_ICONERROR);
		return false;		
	}
	
	/// 分析文件数据

	// -------------------------------------------------------------
	// 场景中的对象
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
// 给逻辑实体（npc）单独制作
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

		COTEActorEntity* ca = SCENE_MGR->CreateEntity("红头发女NPC.obj", ss.str().c_str(), 
									position3d, 
									q, 
									Ogre::Vector3::UNIT_SCALE);	
	}
	fclose(file);

	return true;
}

// -------------------------------------------------------------
// 导出Npc数据
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
// 点光源信息的文件操作
// -------------------------------------------------------------

bool COTESceneXmlLoader::ImportPtLgtFromFile(const std::string& szFileName)
{
	static const std::string c_light_helper_mesh = "lighthelper.mesh"; // 灯光帮助物体

	TiXmlDocument doc(szFileName);
	
	if(!doc.LoadFile())
	{
		return false;		
	}

	// 检查根节点

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

				CASE("constant", attrName)		// 常数
				{									
					sscanf(attrVal, "%f", &constant); 				

					BREAKCASE;	
				}		
						
				CASE("linear", attrName)		// 一次系数
				{									
					sscanf(attrVal, "%f", &linear); 				

					BREAKCASE;	
				}	

				CASE("quadratic", attrName)		// 二次系数
				{									
					sscanf(attrVal, "%f", &quadratic); 				

					BREAKCASE;	
				}	

				ELM_BREAKCASE	

				// vector3

				ELM_CASE("vector3")	

				CASE("position", attrName)	// 位置
				{									
					sscanf(attrVal, "%f %f %f", &position.x, &position.y, &position.z); 				

					BREAKCASE;	
				}				
				CASE("diffuse", attrName)		// 颜色
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
	static const std::string c_light_helper_mesh = "摄影机.mesh"; // 摄像机帮助物体

	TiXmlDocument doc(szFileName);
	
	if(!doc.LoadFile())
	{
		return false;		
	}

	// 检查根节点

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

				CASE("speed", attrName)			// 速度
				{									
					sscanf(attrVal, "%f", &speed); 				

					BREAKCASE;	
				}	

				ELM_BREAKCASE	

				// vector3

				ELM_CASE("vector3")	

				CASE("position", attrName)		// 位置
				{									
					sscanf(attrVal, "%f %f %f", &position.x, &position.y, &position.z); 				

					BREAKCASE;	
				}				
				CASE("quaternion", attrName)	// 旋转	
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

	// 保存实体信息

	std::vector<Ogre::MovableObject*> eList;
	SCENE_MGR->GetEntityList(&eList);
	for(unsigned int i = 0; i < eList.size(); ++ i)
	{			
		if(eList[i]->getName().find("Cam_") == std::string::npos)
		{
			continue;
		}	

		// 摄像机信息

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
// 导出点光源数据

bool COTESceneXmlLoader::ExportPtLgtToFile(const std::string& szFileName)
{
	TiXmlDocument doc("PtLgtList");
	std::stringstream ss;
	ss << "<PtLgtList>\n\t";

	// 保存实体信息

	std::vector<Ogre::MovableObject*> eList;
	SCENE_MGR->GetEntityList(&eList);
	for(unsigned int i = 0; i < eList.size(); ++ i)
	{			
		if(eList[i]->getName().find("Lgt_") == std::string::npos)
		{
			continue;
		}	

		// 点光源信息

		COTEActorEntity* ptLgt = (COTEActorEntity*)eList[i];

		Ogre::Vector3 lightPos = ptLgt->GetParentSceneNode()->getPosition();

		// 计算点光源扩散强度
		
		float constant	= ptLgt->m_DymProperty.GetfloatVal("constant");
		float linear	= ptLgt->m_DymProperty.GetfloatVal("linear");
		float quadratic	= ptLgt->m_DymProperty.GetfloatVal("quadratic");
		
		// 光颜色

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
