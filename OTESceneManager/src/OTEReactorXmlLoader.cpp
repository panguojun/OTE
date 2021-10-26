#include "otereactorxmlloader.h"
#include "OTEReactorManager.h"

using namespace OTE;
using namespace Ogre;

// ----------------------------------------------
COTEReactorXmlLoader::COTEReactorXmlLoader(const std::string& ReactorFile)
{	
	Init(ReactorFile);
}

COTEReactorXmlLoader::COTEReactorXmlLoader() :
m_pXmlDoc(NULL)
{

}

COTEReactorXmlLoader::~COTEReactorXmlLoader()
{
	Destroy();
}

// ----------------------------------------------
void COTEReactorXmlLoader::Destroy()
{
	SAFE_DELETE(m_pXmlDoc)
}

// ----------------------------------------------
void COTEReactorXmlLoader::Init(const std::string& ReactorFile)
{
	if(!ReactorFile.empty())
	{
		m_pXmlDoc = new TiXmlDocument(ReactorFile);
		m_pXmlDoc->LoadFile();
	}
}

// ----------------------------------------------
// 加载

#define ELM_CASE(elmName)	subChild = NULL;	do{	subChild = elm->IterateChildren(elmName,subChild);	if(subChild){TiXmlElement* subElm = subChild->ToElement();const char* attrName = subElm->Attribute("name");	const char* attrVal = subElm->Attribute("val");
#define ELM_BREAKCASE		}}while(subChild);

CReactorBase* COTEReactorXmlLoader::LoadReactor(unsigned int id) 
{	
	if(!m_pXmlDoc)
		return NULL;

	TiXmlElement *elem = m_pXmlDoc->RootElement()->FirstChildElement("Reactor");
	while(elem)
	{			
		if(id == atoi(elem->Attribute("Id")))
		{
			//创建

			CReactorBase* reactor = COTEReactorManager::GetSingleton()->CreateReactor(elem->Attribute("Type"), 0);				
			reactor->m_nID = id;		
			
			::TiXmlNode* child = NULL;
			do
			{
				child = elem->IterateChildren("attribute", child);	
				TiXmlElement* elm = child->ToElement();

				if(elm)
				{
					::TiXmlNode* subChild = NULL;		

					ELM_CASE("string")					
						reactor->AddProperty(attrName, Ogre::String(attrVal));					
					ELM_BREAKCASE	

					int intVal;
					ELM_CASE("int")
						sscanf(attrVal, "%d", &intVal);		
						reactor->AddProperty(attrName, intVal);
					ELM_BREAKCASE

					float floatVal;	
					ELM_CASE("float")	
						sscanf(attrVal, "%f", &floatVal);
						reactor->AddProperty(attrName, floatVal);					
					ELM_BREAKCASE			

					Ogre::Vector3 vecVal;
					ELM_CASE("vector3")					
						sscanf(attrVal, "%f %f %f", &vecVal.x, &vecVal.y, &vecVal.z);	
						reactor->AddProperty(attrName, vecVal);				
					ELM_BREAKCASE

					Ogre::ColourValue corVal;
					ELM_CASE("color4")					
						sscanf(attrVal, "%f %f %f %f", &corVal.r, &corVal.g, &corVal.b, &corVal.a);	
						reactor->AddProperty(attrName, corVal);				
					ELM_BREAKCASE

				}	

			}while(child);				

            return reactor;						
		}

		elem = elem->NextSiblingElement();
	}

	return NULL;
}

// --------------------------------------
//保存Reactor文件
void COTEReactorXmlLoader::SaveFile(const std::string& FileName)
{	
	if(FileName.empty())
		m_pXmlDoc->SaveFile();	
	else
		m_pXmlDoc->SaveFile(FileName);	
}

// --------------------------------------
// 保存指定Id的Reactor
void COTEReactorXmlLoader::ReactorWriteXml(int id)
{
	ReactorWriteXml( id, COTEReactorManager::GetSingleton()->GetReactor(id) );
}

// --------------------------------------
// 保存指定Id的Reactor
#define SET_PROPERTY_ITEM(label, content)				{ss.str(""); ss << content; typeElem->SetAttribute(label, ss.str()); }	
#define SET_VEC_PROPERTY_ITEM(label, content)			{ss.str(""); ss << content.x << " " << content.y << " " << content.z; typeElem->SetAttribute(label, ss.str().c_str()); }	
#define SET_COR_PROPERTY_ITEM(label, content)			{ss.str(""); ss << content.r << " " << content.g << " " << content.b << " " << content.a;  typeElem->SetAttribute(label, ss.str().c_str()); }	

#define SET_PROPERTY_GROUP(type, oplist)	 {HashMap<Ogre::String, std::pair<int, type> >::iterator it = oplist.begin();	while(it != oplist.end()){	SET_PROPERTY_ITEM(it->first.c_str(),		it->second.second)	++ it;}}	
#define SET_VEC_PROPERTY_GROUP(type, oplist) {HashMap<Ogre::String, std::pair<int, type> >::iterator it = oplist.begin();	while(it != oplist.end()){	SET_VEC_PROPERTY_ITEM(it->first.c_str(),	it->second.second)	++ it;}}	
#define SET_COR_PROPERTY_GROUP(type, oplist) {HashMap<Ogre::String, std::pair<int, type> >::iterator it = oplist.begin();	while(it != oplist.end()){	SET_COR_PROPERTY_ITEM(it->first.c_str(),	it->second.second)	++ it;}}	

void COTEReactorXmlLoader::ReactorWriteXml(int id, CReactorBase* pReactor)
{	
	assert(pReactor);
	if(m_pXmlDoc != NULL)
	{
		// 先删除已经存在的记录
		TiXmlElement *elem = m_pXmlDoc->RootElement()->FirstChildElement("Reactor");
		while(elem)
		{
			if(id == atoi(elem->Attribute("Id")))
			{
				m_pXmlDoc->RootElement()->RemoveChild(elem);
				break;
			}

			elem = elem->NextSiblingElement();
		}		

		// 插入新的记录

		elem = new TiXmlElement("Reactor");
		elem->SetAttribute("Id", id);
		elem->SetAttribute("FactoryName", pReactor->m_FactoryName.c_str());		

		TiXmlElement* attrElem = new TiXmlElement("attribute");		
		{
			std::stringstream ss;
			TiXmlElement* typeElem = NULL;
			
			if(!pReactor->m_ints.empty())
			{
				typeElem = new TiXmlElement("int");					
				SET_PROPERTY_GROUP(int, pReactor->m_ints)				
			}
			if(!pReactor->m_floats.empty())
			{
				typeElem = new TiXmlElement("float");					
				SET_PROPERTY_GROUP(float, pReactor->m_floats)
			}
			if(!pReactor->m_vector3s.empty())
			{
				typeElem = new TiXmlElement("Vector3");					
				SET_VEC_PROPERTY_GROUP(Vector3, pReactor->m_vector3s)
			}
			if(!pReactor->m_colors.empty())
			{
				typeElem = new TiXmlElement("ColourValue");					
				SET_COR_PROPERTY_GROUP(ColourValue, pReactor->m_colors)
			}

			if(typeElem)
				attrElem->InsertEndChild(*typeElem);			
			
		}

		elem->InsertEndChild(*attrElem);

		m_pXmlDoc->RootElement()->InsertEndChild(*elem);				
	}	
			
}

// --------------------------------------
// 从文件中移除指定的

void COTEReactorXmlLoader::RemoveReactor(int ID)
{
	if(m_pXmlDoc != NULL)
	{
		// 先删除已经存在的记录
		TiXmlElement *elem = m_pXmlDoc->RootElement()->FirstChildElement("Reactor");
		while(elem)
		{
			if(ID == atoi(elem->Attribute("Id")))
			{
				m_pXmlDoc->RootElement()->RemoveChild(elem);
				break;
			}

			elem = elem->NextSiblingElement();
		}	
	}
}