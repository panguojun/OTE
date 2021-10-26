#pragma once
#include "OTEQTSceneManager.h"
#include "OTED3DManager.h"

namespace OTE
{
// **************************************
// ��Ч
// **************************************
class _OTEExport COTEFog
{
public:	

	static COTEFog* GetInstance();

	// ������Ч״̬

	void UpdateFogState()
	{	
		if(m_FogEnabled)
		{
			SCENE_MGR->setFog( m_FogMode, m_ForColour, 0.1, m_FogStart, m_FogEnd);		
			RENDERSYSTEM->_setFog( m_FogMode, m_ForColour, 0.1, m_FogStart, m_FogEnd);			
		}
		else
		{
			//SCENE_MGR->setFog(Ogre::FOG_NONE);
			RENDERSYSTEM->_setFog(Ogre::FOG_NONE);
		}
	}

	// ������Ч��ɫ

	void SetFogColor(const Ogre::ColourValue& color)
	{
		m_ForColour = color;

		UpdateFogState();
	}

	// ������Ч��ɫ

	void SetFogLinear(float fogStart, float fogEnd)
	{		
		m_FogStart	= fogStart;
		m_FogEnd	= fogEnd;

		UpdateFogState();
	}

	// ��ģʽ

	void SetFogMode(Ogre::FogMode fogMode)
	{		
		m_FogMode = fogMode;

		UpdateFogState();
	}

	// ����

	void SetFogEnable(bool isEnable)
	{			
		m_FogEnabled = isEnable;

		UpdateFogState();
	}

	bool IsFogEnabled()
	{
		return m_FogEnabled;
	}

	/// �ļ����ݽӿ�
	
	void CreateFromFile(const Ogre::String& rFileName);

	void SaveToFile(const Ogre::String& rFileName);

	/// ��Ĺ���
	
	void SetFromFile(const Ogre::String& rFileName);

	void SaveSettingsToFile(const Ogre::String& rFileName);

private:

	static COTEFog s_Inst;

	// ��

	COTEFog();

public:

	// �����ֵ

	Ogre::ColourValue	m_ForColour;

	// ���

	float				m_FogStart;

	float				m_FogEnd;
	
	// �Ƿ��

	bool				m_FogEnabled;
	
	// ��ģʽ

	Ogre::FogMode		m_FogMode;

};

}