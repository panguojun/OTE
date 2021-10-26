#pragma once

#include "OTEQTSceneManager.h"
#include "OTERenderLight.h"

class TiXmlElement;

namespace OTE
{

// **********************************************
// 场景文件加载器
// **********************************************

class _OTEExport COTESceneXmlLoader
{
public:
	
	static COTESceneXmlLoader* GetInstance();

public:
	
	// 从xml文件创建

	bool LoadFromFile(const std::string& szFileName, bool needClearScene = true);

	// 从xml字符串创建

	bool ParseSceneXml(const std::string& szXml, bool needClearScene = true);
	
	//bool LoadFromFile(const std::string& szFileName, bool needClearScene = true);

	void SaveToFile(const std::string& szFileName);

	// 加载物体

	bool LoadObjectsFromFile(const std::string& szFileName, 
							   bool isClearOld = true);	
	// 导出物体

	void ExportEntityInfo(const std::string& fileName, const std::string& type);

	// 导入场景

	bool ImportSceneFromFile(const std::string& szFileName, 
								const std::string& group = "default");
	// NPC

	bool ImportNPCFromFile(const std::string& szFileName);

	bool ExportNPCToFile(const std::string& szFileName);

	// LIGHT

	bool ImportPtLgtFromFile(const std::string& szFileName);

	bool ExportPtLgtToFile(const std::string& szFileName);
	
	// 读入摄像机路径
	
	bool ImportCamPathFromFile(const std::string& szFileName);

	bool ExportCamPathToFile(const std::string& szFileName);
	
private:

	// 读取TERRAIN属性
	void ReadTerrainAttributes(::TiXmlElement* rootElem,
					const std::string& pageName = "default");

	// 读取场景属性
	void ReadSceneAttributes(::TiXmlElement* rootElem);

	// 读取实体属性
	void ReadEntityAttributes(::TiXmlElement* elem,
					const std::string& group = "default");

	// 读取各种类型物体信息

	void ReadObjects(::TiXmlElement* elem);
	
	// 特效对象属性
	void ReadMagicAttributes(::TiXmlElement* elem);

private:

	static COTESceneXmlLoader s_Inst;

};

}
