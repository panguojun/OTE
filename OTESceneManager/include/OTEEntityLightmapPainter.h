
#pragma once
#include "OTEActorEntity.h"
#include "OTEEntityPhysicsMgr.h"

namespace OTE
{
// ************************************
// COTEEntityLightmapPainter
// 用于计算光照图 
// 及其光照图其他相关操作的工具类
// ************************************
class _OTEExport COTEEntityLightmapPainter
{
public:

	// 获得光照图模板名称

	typedef std::string (*GetLightTemplateHndler_t)(const std::string& entityName);


	// 获得光源信息的函数, 
	// 通过它可以获得光源信息，默认情况下调用原来的

	typedef void (*GetLightInforHndler_t)(const Ogre::Vector3& paintPos, Ogre::Vector3& lightPos, Ogre::Vector3& direction, Ogre::ColourValue& diffuse, float& reflectDis, float& reflectDis0/*最大影响范围*/, float& alpha/*混合强度　SRC * (1 - alpha) + DES * alpha*/);

public:

	// 计算整个场景的物件光照图

	static bool GenSceneLightmaps(
		const Ogre::Vector3&	 lightDir,
		const Ogre::ColourValue& diffuse, const Ogre::ColourValue& ambient,			
		GetLightTemplateHndler_t pLMTempGenerHndler,
		const Ogre::String&		 hideEntityKeyName = "",  // 排除在外的实体
		bool  bRenderShadow		 = true,		
		GetLightInforHndler_t	 pLightInforHndler = NULL,
		float blend_alpha		 = 1.0f
		);		
	
	// 根据一个实体名称自动分配光照图名称

	static bool GenLightmap(COTEEntity* e,
		const Ogre::Ray&	 ray,
		const Ogre::ColourValue& diffuse, const Ogre::ColourValue& ambient,		
		const Ogre::String&      lightmapTemplate = "",
		bool  bRenderShadow      = true,
		GetLightInforHndler_t	 pLightInforHndler = NULL,
		float blend_alpha = 1.0f,
		float manualEditSize = 0.0f
		);

	// 吸取颜色
	
	static void PickLightmapColor(COTEEntity* e,
		const Ogre::Ray&	 ray,
		Ogre::ColourValue&   rColor  
		);


	static bool SaveLightmap();

	static bool SaveLightmap(COTEEntity* e);

	// 重新从文件读取光照图

	static void ReLoadLightmapFromFile(COTEEntity* e);

	static void ReLoadLightmapFromFile();

	// 得到光照图

	static std::string GetLightMapName(COTEEntity* e);

protected:	

	// 计算光照图

	static bool _GenLightmap(COTEEntity* e,
		const Ogre::Ray&	 ray,
		const Ogre::ColourValue& diffuse, const Ogre::ColourValue& ambient,		
		Ogre::String&            lightmapTextName,
		bool  bRenderShadow      = true,
		GetLightInforHndler_t	 pLightInforHndler = NULL,
		float blend_alpha = 1.0f,
		float manualEditSize = 0.0f
		);

};


}
