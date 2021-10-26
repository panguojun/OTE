#pragma once
#include "OTEActorEntity.h"
#include "OTEEntityPhysicsMgr.h"

namespace OTE
{
// ************************************
// COTEEntityVertexColorPainter
// 用于处理顶点色的工具类
// ************************************

class _OTEExport COTEEntityVertexColorPainter
{
public:

	// 获得光源信息的函数, 
	// 通过它可以获得光源信息，默认情况下调用原来的

	typedef void (*GetLightInforHndler_t)(const Ogre::Vector3& paintPos, Ogre::Vector3& lightPos, Ogre::Vector3& direction, Ogre::ColourValue& diffuse, float& reflectDis, float& reflectDis0/*最大影响范围*/, float& alpha/*混合强度　SRC * (1 - alpha) + DES * alpha*/);

public: /// 针对全场景操作
	
	// 计算顶点色

	static void GenLightDiffmap(						
						const Ogre::Vector3& lightdir, 
						const Ogre::ColourValue& diffuse, const Ogre::ColourValue& ambient, 
						float blendAlpha		 = 1.0f,
						const Ogre::String&		 hideEntityKeyName = "", // 排除在外的实体
						bool  bRenderShadow		 = true,
						GetLightInforHndler_t	 pLightInforHndler = NULL
						);	


public: /// 针对单个物件的操作

	// 计算光线

	static void GenLightDiffmap(
							COTEEntity* e,
							const Ogre::Vector3& lightdir, 
							const Ogre::ColourValue& diffuse, const Ogre::ColourValue& ambient,
							float blendAlpha		 = 1.0f,
							bool  bRenderShadow		 = true,
							GetLightInforHndler_t	 pLightInforHndler = NULL
							);	


	// 射线涂刷
	
	static void RayPaint(	COTEEntity* e, 
							const Ogre::Vector3& point,	Ogre::Real radious,
							const Ogre::Ray& ray, 
							const Ogre::ColourValue& diffuse, const Ogre::ColourValue&  ambient,
							float blendAlpha		 = 1.0f,
							bool  bRenderShadow		 = true,
							GetLightInforHndler_t	 pLightInforHndler = NULL
							);
	
	
	// 直接涂抹到物件的三角形上

	static void SetVerDiffOnTriangle(COTEEntity* e, Ogre::Ray ray, Ogre::ColourValue diffColor);

	// 查询物件的顶点颜色

	Ogre::ColourValue GetVertexDiffColor(COTEEntity* e, const Ogre::Ray& ray, Ogre::Real radious = 5.0f);

	// 获取顶点色数据

	static void GetVertexColorData(COTEEntity* e, DWORD** pData);

	// 保存顶点色数据

	static void SaveVertexColorData();

	static void SaveVertexColorData(COTEEntity* e);

	static void SaveVertexColorData(COTEEntity* e, const Ogre::String& fileName);

	// 整体设置模型的顶点色

	static void SetVertexColor(COTEEntity* e, const Ogre::ColourValue& diffColor);

	static void SetVertexColor(const Ogre::ColourValue& diffColor);


};

}
