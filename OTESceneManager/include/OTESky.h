#pragma once
#include "OTEStaticInclude.h"
#include "OTEEntity.h" 
#include "OTED3DManager.h"

#ifdef __OTE
#include "OTEActorEntity.h"
#include "OTEBillBoardSet.h"
#endif

namespace OTE
{
class COTESunEffect;

// ******************************************************
// COTESky
// ******************************************************

class _OTEExport COTESky
{
public:

	static COTESky* GetInstance();

	// 摧毁

	static void Destroy(int ca);	
	
#ifdef __ZH
	static void OnRender(Ogre::Camera* cam, const Ogre::Matrix4* reflectMatrix = NULL);	// 渲染回调
#else
	static void OnRender(int ca);	// 渲染回调
#endif	

	// 创建/清理

	void Create();	

	void Clear(bool invalidDevice);	

	/// 设置

	//	设置天空贴图

	void SetSkyTextures(
					const Ogre::String& rBaseTexture		= "",	// 天空基础贴图		
					const Ogre::String& rStaticCloudTexture = "",	// 天空静态云层贴图
					const Ogre::String& rDymCloudTexture	= "",	// 天空动态云层贴图
					const Ogre::String& rTopTexture			= ""	// 天空穹顶贴图
					);
	
	// 设置云层速度

	void SetSkyCloudRollSpeed(float speed = 2.0f)
	{
		m_CloudRollSpeed	=	speed;
	}
	
	// 设置太阳贴图

	void SetSunTexture(const Ogre::String& rSunTexture);

	void SetSunColor(const Ogre::ColourValue& rSunColor);

	// 设置显示/隐藏

	void SetVisible(bool isVisible = true){m_IsVisible = isVisible;}

	// 反射状态

	void SetReflectionHgt(float reflectionHgt){m_ReflectionHgt = reflectionHgt;}

protected:
		
	COTESky();

	~COTESky(){}

	void InvalidateDevice();		

public:

#ifdef __OTE
	COTEBillBoardSet*		m_SunBBS;					// 太阳
#endif

protected:

	static COTESky			s_Inst;

	/// 成员

	bool					m_IsVisible			;		// 是否可见

	float					m_ReflectionHgt		;		// 反射平面高度(必须大于零)

	COTEEntity*				m_SkyBaseEntity		;		// 穹顶

	COTEEntity*				m_SkyTopBaseEntity	;		// 穹顶头盖骨

	COTEEntity*				m_SkyCylinderEntity	;		// 天空筒

	COTEEntity*				m_SkyCloudEntity	;		// 云层

	COTEEntity*				m_SkyMoveCloudEntity;		// 活动云层

#ifdef __OTE
	COTESunEffect*			m_SunEffect			;		// 太阳光晕特效
#endif
	LPD3DXEFFECT			m_pEffect			;		// 效果脚本

public:

	// 数据

	float					m_CloudRollSpeed	;		// 云层旋转速度

	Ogre::String			m_BaseTexture		;		// 基础贴图
	
	Ogre::String			m_StaticCloudTexture;		// 静态云层

	Ogre::String			m_DymCloudTexture	;		// 动态云层

	Ogre::String			m_TopTexture		;		// 顶部云层	

};


// ******************************************************
// COTESkyMgr
// 天空的管理编辑模块
// ******************************************************

class _OTEExport COTESkyMgr
{
public:	
	static COTESkyMgr* GetInstance();

private:

	COTESkyMgr(){}
	~COTESkyMgr(){}

public: // 调用接口
	
	void CreateFromFile(const Ogre::String& rFileName);

	void SaveToFile(const Ogre::String& rFileName);

private:

	static COTESkyMgr s_Inst;

};

#ifdef __OTE

// ******************************************************
// 太阳光晕效果公告牌
// ******************************************************

class COTESunEffBillBoardSet : public COTEBillBoardSet
{
public:

	COTESunEffBillBoardSet(const std::string& name, int poolSize);

	virtual void getRenderOperation(Ogre::RenderOperation& op);

	virtual void _notifyCurrentCamera(Ogre::Camera* cam);

	virtual ~COTESunEffBillBoardSet(){};

};

// ******************************************************
// 太阳光晕效果
// ******************************************************

class COTESunEffect
{
public:
	
	COTESunEffect();
	~COTESunEffect();

	// 创建

	void Create();

	// 更新

	void Update();

public:

	COTESunEffBillBoardSet*		m_SunEffectUpper;	// 上侧光晕

	COTESunEffBillBoardSet*		m_SunEffectLower;	// 下侧光晕

};

#endif

}