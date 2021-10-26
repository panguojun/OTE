
#pragma once
#include "OTEActorEntity.h"
#include "OTEEntityPhysicsMgr.h"

namespace OTE
{
// ************************************
// COTEEntityLightmapPainter
// ���ڼ������ͼ 
// �������ͼ������ز����Ĺ�����
// ************************************
class _OTEExport COTEEntityLightmapPainter
{
public:

	// ��ù���ͼģ������

	typedef std::string (*GetLightTemplateHndler_t)(const std::string& entityName);


	// ��ù�Դ��Ϣ�ĺ���, 
	// ͨ�������Ի�ù�Դ��Ϣ��Ĭ������µ���ԭ����

	typedef void (*GetLightInforHndler_t)(const Ogre::Vector3& paintPos, Ogre::Vector3& lightPos, Ogre::Vector3& direction, Ogre::ColourValue& diffuse, float& reflectDis, float& reflectDis0/*���Ӱ�췶Χ*/, float& alpha/*���ǿ�ȡ�SRC * (1 - alpha) + DES * alpha*/);

public:

	// ���������������������ͼ

	static bool GenSceneLightmaps(
		const Ogre::Vector3&	 lightDir,
		const Ogre::ColourValue& diffuse, const Ogre::ColourValue& ambient,			
		GetLightTemplateHndler_t pLMTempGenerHndler,
		const Ogre::String&		 hideEntityKeyName = "",  // �ų������ʵ��
		bool  bRenderShadow		 = true,		
		GetLightInforHndler_t	 pLightInforHndler = NULL,
		float blend_alpha		 = 1.0f
		);		
	
	// ����һ��ʵ�������Զ��������ͼ����

	static bool GenLightmap(COTEEntity* e,
		const Ogre::Ray&	 ray,
		const Ogre::ColourValue& diffuse, const Ogre::ColourValue& ambient,		
		const Ogre::String&      lightmapTemplate = "",
		bool  bRenderShadow      = true,
		GetLightInforHndler_t	 pLightInforHndler = NULL,
		float blend_alpha = 1.0f,
		float manualEditSize = 0.0f
		);

	// ��ȡ��ɫ
	
	static void PickLightmapColor(COTEEntity* e,
		const Ogre::Ray&	 ray,
		Ogre::ColourValue&   rColor  
		);


	static bool SaveLightmap();

	static bool SaveLightmap(COTEEntity* e);

	// ���´��ļ���ȡ����ͼ

	static void ReLoadLightmapFromFile(COTEEntity* e);

	static void ReLoadLightmapFromFile();

	// �õ�����ͼ

	static std::string GetLightMapName(COTEEntity* e);

protected:	

	// �������ͼ

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
