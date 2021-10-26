#pragma once

#include "OTEQTSceneManager.h"
#include "Ogre.h"
#include "OTERenderLight.h"
#include "OgreDataStream.h"
#include "OgreResourceGroupManager.h"

namespace OTE
{
// **********************************************
// �����ļ�������
// **********************************************

class _OTEExport COTESceneBinaryLoader : public Ogre::Serializer
{
public:

	static COTESceneBinaryLoader* GetInstance();

public:

	bool LoadFromFile(const std::string& szFileName, bool needClearScene = true);

	// ��ȡ�����ƹ����Ϣ
	void ReadSceneAttributes(Ogre::DataStreamPtr& stream);

	// ����ɽ��
	void ReadTerrainInfo(const std::string& szFileName, const std::string& pageName = "default");

	// Entity
	void LoadEntityFromFile(Ogre::DataStreamPtr& stream , const std::string& group = "default");

private:

	static COTESceneBinaryLoader s_Inst;

};
}