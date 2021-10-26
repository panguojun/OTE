#pragma once
#include "OTEActorEntity.h"
#include "OTEEntityPhysicsMgr.h"

namespace OTE
{
// ************************************
// COTEEntityVertexColorPainter
// ���ڴ�����ɫ�Ĺ�����
// ************************************

class _OTEExport COTEEntityVertexColorPainter
{
public:

	// ��ù�Դ��Ϣ�ĺ���, 
	// ͨ�������Ի�ù�Դ��Ϣ��Ĭ������µ���ԭ����

	typedef void (*GetLightInforHndler_t)(const Ogre::Vector3& paintPos, Ogre::Vector3& lightPos, Ogre::Vector3& direction, Ogre::ColourValue& diffuse, float& reflectDis, float& reflectDis0/*���Ӱ�췶Χ*/, float& alpha/*���ǿ�ȡ�SRC * (1 - alpha) + DES * alpha*/);

public: /// ���ȫ��������
	
	// ���㶥��ɫ

	static void GenLightDiffmap(						
						const Ogre::Vector3& lightdir, 
						const Ogre::ColourValue& diffuse, const Ogre::ColourValue& ambient, 
						float blendAlpha		 = 1.0f,
						const Ogre::String&		 hideEntityKeyName = "", // �ų������ʵ��
						bool  bRenderShadow		 = true,
						GetLightInforHndler_t	 pLightInforHndler = NULL
						);	


public: /// ��Ե�������Ĳ���

	// �������

	static void GenLightDiffmap(
							COTEEntity* e,
							const Ogre::Vector3& lightdir, 
							const Ogre::ColourValue& diffuse, const Ogre::ColourValue& ambient,
							float blendAlpha		 = 1.0f,
							bool  bRenderShadow		 = true,
							GetLightInforHndler_t	 pLightInforHndler = NULL
							);	


	// ����Ϳˢ
	
	static void RayPaint(	COTEEntity* e, 
							const Ogre::Vector3& point,	Ogre::Real radious,
							const Ogre::Ray& ray, 
							const Ogre::ColourValue& diffuse, const Ogre::ColourValue&  ambient,
							float blendAlpha		 = 1.0f,
							bool  bRenderShadow		 = true,
							GetLightInforHndler_t	 pLightInforHndler = NULL
							);
	
	
	// ֱ��ͿĨ���������������

	static void SetVerDiffOnTriangle(COTEEntity* e, Ogre::Ray ray, Ogre::ColourValue diffColor);

	// ��ѯ����Ķ�����ɫ

	Ogre::ColourValue GetVertexDiffColor(COTEEntity* e, const Ogre::Ray& ray, Ogre::Real radious = 5.0f);

	// ��ȡ����ɫ����

	static void GetVertexColorData(COTEEntity* e, DWORD** pData);

	// ���涥��ɫ����

	static void SaveVertexColorData();

	static void SaveVertexColorData(COTEEntity* e);

	static void SaveVertexColorData(COTEEntity* e, const Ogre::String& fileName);

	// ��������ģ�͵Ķ���ɫ

	static void SetVertexColor(COTEEntity* e, const Ogre::ColourValue& diffColor);

	static void SetVertexColor(const Ogre::ColourValue& diffColor);


};

}
