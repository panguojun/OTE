#include "dymproperty.h"

using namespace OTE;
using namespace Ogre;

// ===============================================
// CDymPropertyXMLSerializer
// ===============================================

#define CASE(name, attr) if(attr && std::string(attr) == name)
#define BREAKCASE	continue
#define ELM_CASE(elmName)	subChild = NULL;	do{	subChild = elm->IterateChildren(elmName,subChild);	if(subChild){TiXmlElement* subElm = subChild->ToElement();const char* attrName = subElm->Attribute("name");	const char* attrVal = subElm->Attribute("val");
#define ELM_BREAKCASE		}}while(subChild);

// ------------------------------------------------
bool CDymPropertyXMLSerializer::ReadXml(const std::string& xmlFile, CDymProperty* dmyPropty)
{		
	// 读入
	Ogre::String path, fileFullName;
	OTEHelper::GetFullPath(xmlFile, path, fileFullName);	

    TiXmlDocument doc(fileFullName.c_str());
	
	if(!doc.LoadFile())
	{
		OTE_MSG("CDymPropertyXMLSerializer::Read 文件没有找到! File : " << xmlFile, "失败");
		return false;		
	}	

	// 检查根节点

	TiXmlHandle docHandle( &doc );
	::TiXmlElement* rootElem = docHandle.FirstChildElement("root").Element();
	if(!rootElem)
	{
		OTE_MSG("CDymPropertyXMLSerializer::Read 文件格式错误! File : " << xmlFile, "错误");
		return false;		
	}

	// 分析文件数据

	ReadElem(rootElem, dmyPropty);

	return true;
}


// ------------------------------------------------
void CDymPropertyXMLSerializer::ReadElem(TiXmlElement* rootElem, CDymProperty* dmyPropty)
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
				dmyPropty->AddProperty(attrName, Ogre::String(attrVal));					
			ELM_BREAKCASE	

			int intVal;
			ELM_CASE("int")
				sscanf(attrVal, "%d", &intVal);		
				dmyPropty->AddProperty(attrName, intVal);
			ELM_BREAKCASE

			float floatVal;	
			ELM_CASE("float")	
				sscanf(attrVal, "%f", &floatVal);
				dmyPropty->AddProperty(attrName, floatVal);					
			ELM_BREAKCASE			

			Ogre::Vector3 vecVal;
			ELM_CASE("vector3")					
				sscanf(attrVal, "%f %f %f", &vecVal.x, &vecVal.y, &vecVal.z);	
				dmyPropty->AddProperty(attrName, vecVal);				
			ELM_BREAKCASE

			Ogre::ColourValue corVal;
			ELM_CASE("color4")					
				sscanf(attrVal, "%f %f %f %f", &corVal.r, &corVal.g, &corVal.b, &corVal.a);	
				dmyPropty->AddProperty(attrName, corVal);				
			ELM_BREAKCASE

		}		

	}
	while(child);	
}

// -------------------------------------------------------------
// 保存
#define SET_PROPERTY_ITEM(label, content, typeName)		{sstream << "<" << typeName << " name=\"" << label  <<"\" val=\"" << content << "\"/>\n"; }	
#define SET_VEC_PROPERTY_ITEM(label, content)			{sstream << "<vector3 name=\"" << label  <<"\" val=\"" << content.x << " " << content.y << " " << content.z << "\"/>\n"; }	
#define SET_COR_PROPERTY_ITEM(label, content)			{sstream << "<color4 name=\"" << label  <<"\" val=\"" << content.r << " " << content.g << " " << content.b << " " << content.a << "\"/>\n"; }	

#define SET_PROPERTY_GROUP(type, oplist)	 {std::map<Ogre::String, std::pair<int, type> >::iterator it = oplist.begin();	while(it != oplist.end()){	SET_PROPERTY_ITEM(it->first.c_str(),		it->second.second, #type)	++ it;}}	
#define SET_VEC_PROPERTY_GROUP(type, oplist) {std::map<Ogre::String, std::pair<int, type> >::iterator it = oplist.begin();	while(it != oplist.end()){	SET_VEC_PROPERTY_ITEM(it->first.c_str(),	it->second.second)	++ it;}}	
#define SET_COR_PROPERTY_GROUP(type, oplist) {std::map<Ogre::String, std::pair<int, type> >::iterator it = oplist.begin();	while(it != oplist.end()){	SET_COR_PROPERTY_ITEM(it->first.c_str(),	it->second.second)	++ it;}}	

// -------------------------------------------------------------
void CDymPropertyXMLSerializer::SaveXml(const Ogre::String& xmlFile, const CDymProperty* dmyPropty)
{
	TiXmlDocument doc("file");

	std::stringstream ss;
	ss << "<root>\n\t\t\t";	

		SaveToString(ss, dmyPropty);

	ss << "</root>";
	
	doc.Parse(ss.str().c_str());
	doc.SaveFile(xmlFile);
}

// -------------------------------------------------------------
void CDymPropertyXMLSerializer::SaveToString(std::stringstream& sstream, const CDymProperty* dmyPropty)
{
	sstream << "<attribute>\n\t\t";

		SET_PROPERTY_GROUP(int, dmyPropty->m_ints)

		SET_PROPERTY_GROUP(float, dmyPropty->m_floats)
		
		SET_VEC_PROPERTY_GROUP(Ogre::Vector3, dmyPropty->m_vector3s)

		SET_COR_PROPERTY_GROUP(Ogre::ColourValue, dmyPropty->m_colors)

	sstream << "</attribute>\n\t\t";	
}

