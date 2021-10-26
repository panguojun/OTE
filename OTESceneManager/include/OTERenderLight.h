#pragma once
#include "OTEQTSceneManager.h"

namespace OTE
{
// 光照颜色

struct LightColor_t
{
	Ogre::ColourValue	baseCor;
	float				gama;

	LightColor_t()
	{
		gama = 0.5f;
	}

	Ogre::ColourValue GetColor()
	{
		return Ogre::ColourValue(baseCor.r * gama, baseCor.g * gama, baseCor.b * gama);
	}

	LightColor_t operator * ( float factor )
	{
		LightColor_t rlc;
		rlc.baseCor	= baseCor	* factor;
		rlc.gama		= gama		* factor;
		return rlc;
	}
	LightColor_t operator + ( const LightColor_t& lc )
	{
		LightColor_t rlc;
		rlc.baseCor	= baseCor	+ lc.baseCor;
		rlc.gama	= gama		+ lc.gama;
		return rlc;
	}
};

// 光照信息

struct LightKey_t
{
	LightColor_t ambient;			
	LightColor_t diffuse;
	LightColor_t reflect;

	float   	 time;
};	

// ***********************************************
// COTERenderLight
// 渲染灯光
// ***********************************************

class _OTEExport COTERenderLight
{
public:

	static COTERenderLight* GetInstance();

	static void Destroy();

	// 清理

	static void Clear();

	// 设备丢失

	static void InvalidDevice();

public :

	COTERenderLight();

	~COTERenderLight()
	{
		Clear();
	}

public:		

	void AdvancedFrameTime(float dTime);

	void SetCurrentTime(float time)
	{
		m_CurrentTime = time;
	}
	void EnableDym(bool enable = true)
	{
		m_IsDymEnabled = enable;
	}	

	// 得到光照帧信息

	LightKey_t* GetLightKey()
	{
		return GetLightKey(m_CurrentTime);
	}

	LightKey_t* GetLightKey(float time, bool isAutoCrt = true)
	{
		if(m_LightKeys.find(time) == m_LightKeys.end())
		{
			if(isAutoCrt)
			{
				static LightKey_t sLt;
				sLt.time = time;

				m_LightKeys[time] = sLt;
			}
			else
				return NULL;
		}

		return &m_LightKeys[time];
	}	

	// 创建场景的光源

	Ogre::Light* CreateLights();  

protected:
	
	// 更新

	void UpdateFrame(float time);


	// 插值运算

	void LinearInp(LightKey_t* sli, LightKey_t* eli, float time, LightKey_t* li)
	{
		float alpha = 0.0f;
		if(eli->time > sli->time)
		{
			alpha = (time - sli->time) / (eli->time - sli->time);
		}	

		li->ambient.baseCor = sli->ambient.baseCor * (1.0f - alpha) + eli->ambient.baseCor * alpha;
		li->diffuse.baseCor = sli->diffuse.baseCor * (1.0f - alpha) + eli->diffuse.baseCor * alpha;
		li->reflect.baseCor = sli->reflect.baseCor * (1.0f - alpha) + eli->reflect.baseCor * alpha;
	}

public:	

	std::map<float, LightKey_t> m_LightKeys; // <时间， 光照信息>

	float  m_CurrentTime;

protected:	
		
	static COTERenderLight* s_pInst;

	bool   m_IsDymEnabled;			// 是否随时间变化 

};

}