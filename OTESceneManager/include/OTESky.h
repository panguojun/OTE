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

	// �ݻ�

	static void Destroy(int ca);	
	
#ifdef __ZH
	static void OnRender(Ogre::Camera* cam, const Ogre::Matrix4* reflectMatrix = NULL);	// ��Ⱦ�ص�
#else
	static void OnRender(int ca);	// ��Ⱦ�ص�
#endif	

	// ����/����

	void Create();	

	void Clear(bool invalidDevice);	

	/// ����

	//	���������ͼ

	void SetSkyTextures(
					const Ogre::String& rBaseTexture		= "",	// ��ջ�����ͼ		
					const Ogre::String& rStaticCloudTexture = "",	// ��վ�̬�Ʋ���ͼ
					const Ogre::String& rDymCloudTexture	= "",	// ��ն�̬�Ʋ���ͼ
					const Ogre::String& rTopTexture			= ""	// ���񷶥��ͼ
					);
	
	// �����Ʋ��ٶ�

	void SetSkyCloudRollSpeed(float speed = 2.0f)
	{
		m_CloudRollSpeed	=	speed;
	}
	
	// ����̫����ͼ

	void SetSunTexture(const Ogre::String& rSunTexture);

	void SetSunColor(const Ogre::ColourValue& rSunColor);

	// ������ʾ/����

	void SetVisible(bool isVisible = true){m_IsVisible = isVisible;}

	// ����״̬

	void SetReflectionHgt(float reflectionHgt){m_ReflectionHgt = reflectionHgt;}

protected:
		
	COTESky();

	~COTESky(){}

	void InvalidateDevice();		

public:

#ifdef __OTE
	COTEBillBoardSet*		m_SunBBS;					// ̫��
#endif

protected:

	static COTESky			s_Inst;

	/// ��Ա

	bool					m_IsVisible			;		// �Ƿ�ɼ�

	float					m_ReflectionHgt		;		// ����ƽ��߶�(���������)

	COTEEntity*				m_SkyBaseEntity		;		// 񷶥

	COTEEntity*				m_SkyTopBaseEntity	;		// 񷶥ͷ�ǹ�

	COTEEntity*				m_SkyCylinderEntity	;		// ���Ͳ

	COTEEntity*				m_SkyCloudEntity	;		// �Ʋ�

	COTEEntity*				m_SkyMoveCloudEntity;		// ��Ʋ�

#ifdef __OTE
	COTESunEffect*			m_SunEffect			;		// ̫��������Ч
#endif
	LPD3DXEFFECT			m_pEffect			;		// Ч���ű�

public:

	// ����

	float					m_CloudRollSpeed	;		// �Ʋ���ת�ٶ�

	Ogre::String			m_BaseTexture		;		// ������ͼ
	
	Ogre::String			m_StaticCloudTexture;		// ��̬�Ʋ�

	Ogre::String			m_DymCloudTexture	;		// ��̬�Ʋ�

	Ogre::String			m_TopTexture		;		// �����Ʋ�	

};


// ******************************************************
// COTESkyMgr
// ��յĹ���༭ģ��
// ******************************************************

class _OTEExport COTESkyMgr
{
public:	
	static COTESkyMgr* GetInstance();

private:

	COTESkyMgr(){}
	~COTESkyMgr(){}

public: // ���ýӿ�
	
	void CreateFromFile(const Ogre::String& rFileName);

	void SaveToFile(const Ogre::String& rFileName);

private:

	static COTESkyMgr s_Inst;

};

#ifdef __OTE

// ******************************************************
// ̫������Ч��������
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
// ̫������Ч��
// ******************************************************

class COTESunEffect
{
public:
	
	COTESunEffect();
	~COTESunEffect();

	// ����

	void Create();

	// ����

	void Update();

public:

	COTESunEffBillBoardSet*		m_SunEffectUpper;	// �ϲ����

	COTESunEffBillBoardSet*		m_SunEffectLower;	// �²����

};

#endif

}