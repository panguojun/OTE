#pragma once
#include "OTEQTSceneManager.h"
#include "OTED3DManager.h"

namespace OTE
{
// **************************************
// 雾效
// **************************************
class _OTEExport COTEFog
{
public:	

	static COTEFog* GetInstance();

	// 设置雾效状态

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

	// 设置雾效颜色

	void SetFogColor(const Ogre::ColourValue& color)
	{
		m_ForColour = color;

		UpdateFogState();
	}

	// 设置雾效颜色

	void SetFogLinear(float fogStart, float fogEnd)
	{		
		m_FogStart	= fogStart;
		m_FogEnd	= fogEnd;

		UpdateFogState();
	}

	// 雾模式

	void SetFogMode(Ogre::FogMode fogMode)
	{		
		m_FogMode = fogMode;

		UpdateFogState();
	}

	// 开关

	void SetFogEnable(bool isEnable)
	{			
		m_FogEnabled = isEnable;

		UpdateFogState();
	}

	bool IsFogEnabled()
	{
		return m_FogEnabled;
	}

	/// 文件数据接口
	
	void CreateFromFile(const Ogre::String& rFileName);

	void SaveToFile(const Ogre::String& rFileName);

	/// 雾的管理
	
	void SetFromFile(const Ogre::String& rFileName);

	void SaveSettingsToFile(const Ogre::String& rFileName);

private:

	static COTEFog s_Inst;

	// 雾

	COTEFog();

public:

	// 雾相关值

	Ogre::ColourValue	m_ForColour;

	// 雾距

	float				m_FogStart;

	float				m_FogEnd;
	
	// 是否打开

	bool				m_FogEnabled;
	
	// 雾模式

	Ogre::FogMode		m_FogMode;

};

}