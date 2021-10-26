#include "OTERenderLight.h"

using namespace Ogre;
using namespace OTE;

#define			DEFAULT_TIME		9
#define			TIME_SCALE			0.017f // ��Ϸ���뵽Сʱ�Ļ���ϵ��

// ------------------------------------------------------
COTERenderLight* COTERenderLight::s_pInst;
COTERenderLight* COTERenderLight::GetInstance()
{	
	if(!s_pInst)
		s_pInst = new COTERenderLight();	
	return s_pInst;
}

void COTERenderLight::Destroy()
{
	SAFE_DELETE(COTERenderLight::s_pInst)
}

// ------------------------------------------------------
COTERenderLight::COTERenderLight() :
m_CurrentTime(DEFAULT_TIME),
m_IsDymEnabled(false)
{

}

// ------------------------------------------------------
void COTERenderLight::Clear()
{
	if(!s_pInst)
		return;

	s_pInst->m_LightKeys.clear();

	s_pInst->m_CurrentTime = DEFAULT_TIME;
}

// ------------------------------------------------------
Ogre::Light* COTERenderLight::CreateLights()
{
	// ������

	SCENE_MGR->setAmbientLight(Ogre::ColourValue(0.2f, 0.2f, 0.2f));  

	// ����

    Ogre::Light* l = SCENE_MGR->createLight("MainLight");
	l->setType(Ogre::Light::LT_DIRECTIONAL);
	
	return l;
}

// ------------------------------------------------------
void COTERenderLight::InvalidDevice()
{
	SCENE_MGR->removeLight("MainLight");
}

// ------------------------------------------------------
void COTERenderLight::AdvancedFrameTime(float dTime)
{	
	UpdateFrame(m_CurrentTime);

	// ��̬

	if(m_IsDymEnabled)
	{
		m_CurrentTime += dTime * TIME_SCALE;

		if(m_CurrentTime > 24.0f) // ʱ��ѭ��
			m_CurrentTime = 0.0f;
	}
}

// ------------------------------------------------------
// ����
void COTERenderLight::UpdateFrame(float time)
{
	// �����ڹؼ�֡

	if(m_LightKeys.empty())
		GetLightKey(m_CurrentTime);

	// �ؼ�֡��ֵ

	LightKey_t* li =  GetLightKey(time, false);

	if(!li)
	{
		std::map<float, LightKey_t>::iterator itBegin	= m_LightKeys.begin();
		std::map<float, LightKey_t>::iterator itEnd		= m_LightKeys.end();;

		static LightKey_t sLI;
		
		LightKey_t* sli = NULL;
		LightKey_t* eli = NULL;

		float startTime = - 1.0f;
		float endTime	= MAX_FLOAT;

		std::map<float, LightKey_t>::iterator it = itBegin;
		while(it != itEnd)
		{	
			if(it->first > startTime && it->first <= time)
			{
				sli = &it->second;
				startTime = it->first;
				//OTE_TRACE("startTime : " << startTime)
			}

			if(it->first < endTime && it->first >= time)
			{
				eli = &it->second;
				endTime = it->first;
				//OTE_TRACE("endTime : " << endTime)

			}	

			//OTE_TRACE("sli : " << sli->time)
			//OTE_TRACE("eli : " << eli->time)

			++ it;
		}

		if(!sli) sli = eli;
		if(!eli) eli = sli;		

		li = &sLI;

	/*	OTE_TRACE("sli->time " << sli->time)
		OTE_TRACE("eli->time " << eli->time)*/

		LinearInp(sli, eli, time, li);
	
	}

	// ̫��
	
	Ogre::Light* l = SCENE_MAINLIGHT;
	if(!l)	
		l = CreateLights();	

	// ����ʱ�����̫���ķ�λ 

	Ogre::Vector3 dir;

	float fTime = time;
	float delta=(15.0f * fTime - 90.0f) * Ogre::Math::PI / 180.0f;
	if(delta == 0.0f)
		delta = 0.0;
	else if(delta > 3.0f)
		delta = 2.5f;

	float summerAngle = 73.5f * Ogre::Math::PI / 180.0f /*��������ʱ̫���߶�*/;
	dir = Ogre::Vector3(-cos(delta),-sin(delta), - sin(summerAngle));
	
	// ���ù����ɫ

	l->setDirection(dir.normalisedCopy());

	l->setDiffuseColour(li->diffuse.GetColor());		// ������

	SCENE_MGR->setAmbientLight(li->ambient.GetColor());	// ������

	m_CurrentTime = time;
	
}
