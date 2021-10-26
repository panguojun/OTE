#pragma once

#include "OTEQTSceneManager.h"
#include "OTERenderLight.h"

class TiXmlElement;

namespace OTE
{

// **********************************************
// �����ļ�������
// **********************************************

class _OTEExport COTESceneXmlLoader
{
public:
	
	static COTESceneXmlLoader* GetInstance();

public:
	
	// ��xml�ļ�����

	bool LoadFromFile(const std::string& szFileName, bool needClearScene = true);

	// ��xml�ַ�������

	bool ParseSceneXml(const std::string& szXml, bool needClearScene = true);
	
	//bool LoadFromFile(const std::string& szFileName, bool needClearScene = true);

	void SaveToFile(const std::string& szFileName);

	// ��������

	bool LoadObjectsFromFile(const std::string& szFileName, 
							   bool isClearOld = true);	
	// ��������

	void ExportEntityInfo(const std::string& fileName, const std::string& type);

	// ���볡��

	bool ImportSceneFromFile(const std::string& szFileName, 
								const std::string& group = "default");
	// NPC

	bool ImportNPCFromFile(const std::string& szFileName);

	bool ExportNPCToFile(const std::string& szFileName);

	// LIGHT

	bool ImportPtLgtFromFile(const std::string& szFileName);

	bool ExportPtLgtToFile(const std::string& szFileName);
	
	// ���������·��
	
	bool ImportCamPathFromFile(const std::string& szFileName);

	bool ExportCamPathToFile(const std::string& szFileName);
	
private:

	// ��ȡTERRAIN����
	void ReadTerrainAttributes(::TiXmlElement* rootElem,
					const std::string& pageName = "default");

	// ��ȡ��������
	void ReadSceneAttributes(::TiXmlElement* rootElem);

	// ��ȡʵ������
	void ReadEntityAttributes(::TiXmlElement* elem,
					const std::string& group = "default");

	// ��ȡ��������������Ϣ

	void ReadObjects(::TiXmlElement* elem);
	
	// ��Ч��������
	void ReadMagicAttributes(::TiXmlElement* elem);

private:

	static COTESceneXmlLoader s_Inst;

};

}
