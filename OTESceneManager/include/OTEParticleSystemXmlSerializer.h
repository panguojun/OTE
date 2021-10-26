#pragma once
#include "OTEStaticInclude.h"
#include "tinyxml.h"
#include "OTEParticlesystem.h"
#include "dymproperty.h"

namespace OTE
{
// **************************************
// COTEParticleSystemXmlSerializer
// **************************************

class _OTEExport COTEParticleSystemXmlSerializer
{	

public:

	bool Read(const Ogre::String& name, const Ogre::String& xmlFile);

	void Save(const Ogre::String& name, const Ogre::String& xmlFile);

	COTEParticleSystemRes* GetParticleSystemRes()
	{
		return m_PSR;
	}

protected:

	void ReadAttributes(::TiXmlElement* rootElem);

private:

	COTEParticleSystemRes*	m_PSR;
	

};

}
