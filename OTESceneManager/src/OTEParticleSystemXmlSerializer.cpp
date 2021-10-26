#include "OTECommon.h"
#include "dymproperty.h"
#include "OTEParticleSystemXmlSerializer.h"

using namespace OTE;
using namespace Ogre;

// -------------------------------------------------------------
#define CASE(name, attr) if(attr && std::string(attr) == name)
#define BREAKCASE	continue
#define ELM_CASE(elmName)	subChild = NULL;	do{	subChild = elm->IterateChildren(elmName,subChild);	if(subChild){TiXmlElement* subElm = subChild->ToElement();const char* attrName = subElm->Attribute("name");	const char* attrVal = subElm->Attribute("val");
#define ELM_BREAKCASE		}}while(subChild);

// ------------------------------------------------
bool COTEParticleSystemXmlSerializer::Read(const Ogre::String& name, const Ogre::String& xmlFile)
{	
	m_PSR = COTEParticleSystemManager::GetInstance()->CreateParticleSystemRes(name);
	
	// 读入
	Ogre::String path, fileFullName;
	OTEHelper::GetFullPath(xmlFile, path, fileFullName);	

    TiXmlDocument doc(fileFullName.c_str());
	
	if(!doc.LoadFile())
	{
		OTE_MSG("COTEParticleSystemXmlSerializer::Read 文件没有找到! File : " << xmlFile, "失败");
		return false;		
	}	

	// 检查根节点

	TiXmlHandle docHandle( &doc );
	::TiXmlElement* rootElem = docHandle.FirstChildElement("root").Element();
	if(!rootElem)
	{
		OTE_MSG("COTEParticleSystemXmlSerializer::Read 文件格式错误! File : " << xmlFile, "错误");
		return false;		
	}

	// 分析文件数据

	// -------------------------------------------------------------
	// 属性配置
	// -------------------------------------------------------------

	ReadAttributes(rootElem);


	// -------------------------------------------------------------
	// 逐个节点
	// -------------------------------------------------------------

	::TiXmlNode* child = NULL;
	do{			
		child = rootElem->IterateChildren("node", child);	
		TiXmlElement* elm = child->ToElement();
		if(elm)
		{
			const char* szType = elm->Attribute("type");
			CASE("emitter", szType)
			{
				CDymPropertyXMLSerializer::ReadElem(elm, (COTESimpleEmitter*)m_PSR->m_Emitter);
				BREAKCASE;
			}
			CASE("affector", szType)
			{			
				CDymPropertyXMLSerializer::ReadElem(elm, m_PSR->m_ParticleSystemAffector);
				BREAKCASE;
			}		
			
		}
	}
	while(child);

	return true;
}


// -------------------------------------------------------------
// 属性

void COTEParticleSystemXmlSerializer::ReadAttributes(::TiXmlElement* rootElem)
{
	::TiXmlNode* child = NULL;
	do
	{
		child = rootElem->IterateChildren("attribute", child);	
		TiXmlElement* elm = child->ToElement();

		if(elm)
		{
			::TiXmlNode* subChild = NULL;
			
			ELM_CASE("string")				
				CASE("textureName", attrName)	
				{				
					m_PSR->m_TextureName = attrVal;
					BREAKCASE;	
				}				
				
			ELM_BREAKCASE				

			ELM_CASE("int")
				CASE("poolSize", attrName)	
				{
					sscanf(attrVal, "%d", &m_PSR->m_PoolSize);		
					BREAKCASE;	
				}		
				
				
			ELM_BREAKCASE

			ELM_CASE("float")
				CASE("liftTime", attrName)	
				{
					sscanf(attrVal, "%f", &m_PSR->m_LiftTime);		
					BREAKCASE;	
				}			
				
				
			ELM_BREAKCASE				
			
		}		

	}
	while(child);

}

// -------------------------------------------------------------
// 保存
#define SET_PROPERTY_ITEM(label, content, typeName)		{ss << "<" << typeName << " name=\"" << label  <<"\" val=\"" << content << "\"/>\n"; }	
#define SET_VEC_PROPERTY_ITEM(label, content)			{ss << "<vector3 name=\"" << label  <<"\" val=\"" << content.x << " " << content.y << " " << content.z << "\"/>\n"; }	
#define SET_COR_PROPERTY_ITEM(label, content)			{ss << "<color4 name=\"" << label  <<"\" val=\"" << content.r << " " << content.g << " " << content.b << " " << content.a << "\"/>\n"; }	

#define SET_PROPERTY_GROUP(type, oplist)	 {HashMap<Ogre::String, std::pair<int, type> >::iterator it = oplist.begin();	while(it != oplist.end()){	SET_PROPERTY_ITEM(it->first.c_str(),		it->second.second, #type)	++ it;}}	
#define SET_VEC_PROPERTY_GROUP(type, oplist) {HashMap<Ogre::String, std::pair<int, type> >::iterator it = oplist.begin();	while(it != oplist.end()){	SET_VEC_PROPERTY_ITEM(it->first.c_str(),	it->second.second)	++ it;}}	
#define SET_COR_PROPERTY_GROUP(type, oplist) {HashMap<Ogre::String, std::pair<int, type> >::iterator it = oplist.begin();	while(it != oplist.end()){	SET_COR_PROPERTY_ITEM(it->first.c_str(),	it->second.second)	++ it;}}	


void COTEParticleSystemXmlSerializer::Save(const Ogre::String& name, const Ogre::String& xmlFile)
{
	m_PSR = COTEParticleSystemManager::GetInstance()->GetParticleSystemRes(name);
	if(!m_PSR)
		return;

	COTESimpleEmitter* emitter = ((COTESimpleEmitter*)m_PSR->m_Emitter);
	COTEParticleSystemAffector* affector = m_PSR->m_ParticleSystemAffector;

	TiXmlDocument doc("paritcle");

	std::stringstream ss;
	ss << "<root>\n\t\t\t";

	{			
		ss << "<attribute>\n\t\t";

			SET_PROPERTY_ITEM("textureName",				m_PSR->m_TextureName, "string")
			SET_PROPERTY_ITEM("poolSize",					m_PSR->m_PoolSize, "int")
			SET_PROPERTY_ITEM("liftTime",					m_PSR->m_LiftTime, "float")				
			
		ss << "</attribute>\n\t\t";

		COTESimpleEmitter* emitter = (COTESimpleEmitter*)m_PSR->m_Emitter;		

		ss << "<node type=\"emitter\">\n";	
			CDymPropertyXMLSerializer::SaveToString(ss, emitter);
		ss << "</node>\n";

		COTEParticleSystemAffector* affector = m_PSR->m_ParticleSystemAffector;

		ss << "<node type=\"affector\">\n";	
			CDymPropertyXMLSerializer::SaveToString(ss, affector);
		ss << "</node>\n";

	}
	ss << "</root>";
	
	doc.Parse(ss.str().c_str());
	doc.SaveFile(xmlFile);
}

