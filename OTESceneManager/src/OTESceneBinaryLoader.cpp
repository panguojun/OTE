#include "OTEActorEntity.h"
#include "oteterrainserializer.h"
#include "OTESceneBinaryLoader.h"
#include "OTEMagicManager.h"
#include "OTEFog.h"
#include "OTECommon.h"
#include "OTEMathLib.h"
#include "OTEActorActionMgr.h"

// -------------------------------------------------------------

using namespace OTE;
using namespace Ogre;

// -------------------------------------------------------------
COTESceneBinaryLoader COTESceneBinaryLoader::s_Inst;
COTESceneBinaryLoader* COTESceneBinaryLoader::GetInstance()
{	
	return &s_Inst;
}

// -------------------------------------------------------------
void COTESceneBinaryLoader::ReadSceneAttributes(Ogre::DataStreamPtr& stream)
{
	LightColor_t light_cor;
	Ogre::ColourValue fog_cor;
	float fog_minDis = 0;
	float fog_maxDis = 1000;

	// 场景名称
	std::string ver = readString(stream);
	
	// 时间
	COTERenderLight::GetInstance()->m_CurrentTime = readString(stream);
	ver = COTERenderLight::GetInstance()->m_CurrentTime;

	Ogre::Quaternion q;

	// 环境光、主光、补光、地形环境光、地形主光
	for (int i = 0; i < 5; ++i)
	{
		readObject(stream, q);
		light_cor.baseCor.r = q.x;
		light_cor.baseCor.g = q.y;
		light_cor.baseCor.b = q.z;
		light_cor.gama	    = q.w;

		switch (i)
		{
			case 0:
				COTERenderLight::GetInstance()->GetLightInfo()->ambient = light_cor;
				break;
			case 1:
				COTERenderLight::GetInstance()->GetLightInfo()->diffuse = light_cor;
				break;
			case 2:
				COTERenderLight::GetInstance()->GetLightInfo()->reflect = light_cor;
				break;
			case 3:
				COTERenderLight::GetInstance()->GetLightInfo()->terrAmb = light_cor;
				break;
			case 4:
				COTERenderLight::GetInstance()->GetLightInfo()->terrDiff = light_cor;
				break;
		}
	}

	// 雾颜色
	Ogre::Vector3 v;
	readObject(stream, v);
	fog_cor.r = v.x;
	fog_cor.g = v.y;
	fog_cor.b = v.z;

	// 雾近距、雾远距
	readFloats(stream, &fog_minDis, 1);
	readFloats(stream, &fog_maxDis, 1);

	COTEFog::GetInstance()->SetFogColor(fog_cor);
	COTEFog::GetInstance()->SetFogLinear(fog_minDis, fog_maxDis);

	COTERenderLight::GetInstance()->Update(ver);
}
// -------------------------------------------------------------
// 读取山体文件
void COTESceneBinaryLoader::ReadTerrainInfo(const std::string& szFileName, const std::string& pageName)
{
	Ogre::DataStreamPtr stream = Ogre::ResourceGroupManager::getSingleton().openResource(szFileName);

	if(!COTETilePage::s_CurrentPage)
	{
		COTETilePage::CreateTerrain(pageName, szFileName);
	}
	else
	{					
		COTETilePage::ImportTerrain(pageName, szFileName);
	}
}
// -------------------------------------------------------------
// 加载Entity
void COTESceneBinaryLoader::LoadEntityFromFile(Ogre::DataStreamPtr& stream, const std::string& group)
{
	unsigned int size;
	readInts(stream, &size, 1);

	std::string entityName;
	std::string meshId;
	std::string ActionName;

	Ogre::Vector3 position;
	Ogre::Quaternion rotation;	
	Ogre::Vector3 scaling = Ogre::Vector3(1, 1, 1);

	for (int i = 0; i < size; ++i)
	{
		entityName = readString(stream);
		meshId = readString(stream);
		readObject(stream, position);
		readObject(stream, rotation);
		readObject(stream, scaling);

		COTEActorEntity* ca = SCENE_MGR->CreateEntity(meshId.c_str(), entityName.c_str(), position, rotation, scaling, group);
		if(!ActionName.empty())
			COTEActorActionMgr::SetAction(ca, ActionName.c_str());			
	}
}
// -------------------------------------------------------------
bool COTESceneBinaryLoader::LoadFromFile(const std::string& szFileName, bool needClearScene)
{
	Ogre::DataStreamPtr stream = Ogre::ResourceGroupManager::getSingleton().openResource(szFileName);
	
	if(needClearScene)
		SCENE_MGR->clearScene();

	// 读取场景数据
	ReadSceneAttributes(stream);

	// 读取山体信息
	ReadTerrainInfo(readString(stream));

	LoadEntityFromFile(stream);

	return true;
}
