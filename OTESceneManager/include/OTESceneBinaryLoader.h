#pragma once

#include "OTEQTSceneManager.h"
#include "Ogre.h"
#include "OTERenderLight.h"
#include "OgreDataStream.h"
#include "OgreResourceGroupManager.h"

namespace OTE
{
// **********************************************
// 场景文件加载器
// **********************************************

class _OTEExport COTESceneBinaryLoader : public Ogre::Serializer
{
public:

	static COTESceneBinaryLoader* GetInstance();

public:

	bool LoadFromFile(const std::string& szFileName, bool needClearScene = true);

	// 读取场景灯光等信息
	void ReadSceneAttributes(Ogre::DataStreamPtr& stream);

	// 地形山体
	void ReadTerrainInfo(const std::string& szFileName, const std::string& pageName = "default");

	// Entity
	void LoadEntityFromFile(Ogre::DataStreamPtr& stream , const std::string& group = "default");

private:

	static COTESceneBinaryLoader s_Inst;

};
}