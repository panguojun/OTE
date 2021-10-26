#pragma once
#include "tinyxml.h"
#include "OTEStaticInclude.h"
#include "OTECommon.h"

// **************************************
// ��̬����
// CDymProperty
// **************************************
namespace OTE
{

class CDymProperty
{
public:	

	/// �������

	void AddProperty(const Ogre::String& name, const Ogre::String& val)
	{		
		m_strings[name] = std::pair<int, Ogre::String>(OTEHelper::HashString(name), val);
	}

	void AddProperty(const Ogre::String& name, Ogre::Real val)
	{		
		m_floats[name] = std::pair<int, Ogre::Real>(OTEHelper::HashString(name), val);

	}
	void AddProperty(const Ogre::String& name, Ogre::Vector3 val)
	{		
		m_vector3s[name] = std::pair<int, Ogre::Vector3>(OTEHelper::HashString(name), val);
	}
	void AddProperty(const Ogre::String& name, int val)
	{		
		m_ints[name] = std::pair<int, int>(OTEHelper::HashString(name), val);
	}

	void AddProperty(const Ogre::String& name, const Ogre::ColourValue& val)
	{	
		m_colors[name] = std::pair<int, Ogre::ColourValue>(OTEHelper::HashString(name), val);
	}	

	/// ��������ֵ

	void SetVal(const Ogre::String& name, const Ogre::String& val)
	{
		m_strings[name].second = val;
	}

	void SetVal(const Ogre::String& name, float val)
	{
		m_floats[name].second = val;
	}

	void SetVal(const Ogre::String& name, Ogre::Vector3 val)
	{
		m_vector3s[name].second = val;
	}

	void SetVal(const Ogre::String& name, int val)
	{
		m_ints[name].second = val;
	}

	void SetVal(const Ogre::String& name, const Ogre::ColourValue& val)
	{
		m_colors[name].second = val;
	}

	/// ��ѯ����ֵ

	const Ogre::String& GetStringVal(const Ogre::String& name)
	{
		return m_strings[name].second;
	}

	float GetfloatVal(const Ogre::String& name)
	{
		return m_floats[name].second;
	}

	const Ogre::Vector3& GetVector3Val(const Ogre::String& name)
	{
		return m_vector3s[name].second;
	}

	int GetIntVal(const Ogre::String& name)
	{
		return m_ints[name].second;
	}

	const Ogre::ColourValue& GetCorVal(const Ogre::String& name)
	{
		return m_colors[name].second;
	}	

public:

	// �������Ͷ�������б�

	mutable std::map<Ogre::String, std::pair<int, Ogre::String> >		m_strings;	
	mutable std::map<Ogre::String, std::pair<int, Ogre::Real> >			m_floats;
	mutable std::map<Ogre::String, std::pair<int, Ogre::Vector3> >		m_vector3s;
	mutable std::map<Ogre::String, std::pair<int, int> >				m_ints;
	mutable std::map<Ogre::String, std::pair<int, Ogre::ColourValue> >	m_colors;	

};

// **************************************
// ��̬���Ի���
// CDymPropertyXMLSerializer
// **************************************

class CDymPropertyXMLSerializer
{
public:
	
	/// read

	static bool ReadXml(const std::string& xmlFile, CDymProperty* dmyPropty);

	static void ReadElem(TiXmlElement* rootElem, CDymProperty* dmyPropty);

	/// save

	static void SaveXml(const std::string& xmlFile, const CDymProperty* dmyPropty);

	static void SaveToString(std::stringstream& sstream, const CDymProperty* dmyPropty);
};


}