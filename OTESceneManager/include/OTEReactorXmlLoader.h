#pragma once
#include "OTEStaticInclude.h"
#include "OTEReactorBase.h"
#include "tinyxml.h"

namespace OTE
{
// ***********************************
// class COTEReactorXmlLoader
// ������Ӧ ������
// 
// ***********************************

class COTEReactorXmlLoader
{

public:
	COTEReactorXmlLoader();	
	COTEReactorXmlLoader(const std::string& ReactorFile);
	~COTEReactorXmlLoader();

public:
	void Init(const std::string& ReactorFile);
	void Destroy();

public:
	
	// ��Reactor��Ϣд��xml
	void ReactorWriteXml(int id);

	// ����Track�ļ�
	void SaveFile(const std::string& FileName = "");
			
	// ����ָ��ID��
	CReactorBase* LoadReactor(unsigned int id);

	// �Ƴ�
	void RemoveReactor(int id);

protected:

	void ReactorWriteXml(int ID, CReactorBase* Reactor);
	
protected:

	// Xml��Դ�ļ�
	TiXmlDocument*		m_pXmlDoc;

};

}
