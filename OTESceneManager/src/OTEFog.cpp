#include "OTEFog.h"

using namespace OTE;
using namespace Ogre;

// -------------------------------------
COTEFog::COTEFog() : 
m_FogStart(0.0f), 
m_FogEnd(1000.0f),
m_FogEnabled(true),
m_FogMode(Ogre::FOG_LINEAR)
{	
}

// ------------------------------------------------------
COTEFog COTEFog::s_Inst;
COTEFog* COTEFog::GetInstance(){return &s_Inst;} 

// ------------------------------------------------------
void COTEFog::CreateFromFile(const Ogre::String& rFileName)
{
	FILE* file = fopen(rFileName.c_str(), "r");
	if(!file)
	{
		OTE_MSG_ERR("���������ļ�ʧ�ܣ�"); 
		return;
	}

	fscanf(file, "FogStart: %.2f\n",				&m_FogStart	);	
	fscanf(file, "FogEnd: %.2f\n",					&m_FogEnd	);	
	fscanf(file, "FogColor: %.2f %.2f %.2f\n",		&m_ForColour.r, &m_ForColour.g, &m_ForColour.b);	

	fclose(file);		
}

// ------------------------------------------------------
void COTEFog::SaveToFile(const Ogre::String& rFileName)
{
	FILE* file = fopen(rFileName.c_str(), "w");
	if(!file)
	{
		OTE_MSG_ERR("�����������ļ�ʧ�ܣ�"); 
		return;
	}

	fprintf(file, "FogStart: %.2f\n",	m_FogStart	);	
	fprintf(file, "FogEnd: %.2f\n",		m_FogEnd	);	
	fprintf(file, "FogColor: %.2f %.2f %.2f\n",		m_ForColour.r, m_ForColour.g, m_ForColour.b);	

	fclose(file);
}


// -----------------------------------------------
// �ļ���ȡ

void COTEFog::SetFromFile(const Ogre::String& rFileName)
{	
	if(!rFileName.empty())
	{
		Ogre::DataStreamPtr stream = RESMGR_LOAD(rFileName);
		
		if(stream.isNull())
		{
			OTE_MSG_ERR("���������ļ�ʧ��! FILE: " << rFileName)
			return;
		}

		float fogNear, fogFar;
		Ogre::ColourValue fogColour;

		// ��ľ���,��ɫ

		stream->read(&fogNear,sizeof(float));		
		stream->read(&fogFar,sizeof(float));	

		stream->read(&fogColour,sizeof(Ogre::ColourValue));	

		SetFogLinear(fogNear, fogFar);
		SetFogColor(fogColour);
	}

}

// -----------------------------------------------
// �ļ�����

void COTEFog::SaveSettingsToFile(const Ogre::String& rFileName)
{	
	if(!rFileName.empty())
	{
		FILE* file = fopen(rFileName.c_str(), "wb");

		if(!file)
		{
			OTE_MSG_ERR("����ʱ,���������ļ�ʧ��! FILE: " << rFileName)
			return;
		}

		float fogNear = m_FogStart;
		float fogFar = m_FogEnd;
		Ogre::ColourValue fogColour = COTEFog::m_ForColour;

		// ��ľ���,��ɫ

		fwrite(&fogNear,sizeof(float), 1, file);		
		fwrite(&fogFar,sizeof(float), 1, file);		

		fwrite(&fogColour,sizeof(Ogre::ColourValue), 1, file);		

		fclose(file);
		
	}

}
