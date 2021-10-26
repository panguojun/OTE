#include "otereactorbase.h"
#include "OTEParticlesystem.h"
#include "OTEBillBoardSet.h"
#include "OTEParticleSystemXmlSerializer.h"

namespace OTE
{

// ����Ԫ��

struct ParticleElement_t
{
	Ogre::MovableObject* MO;
	float TimeEsp;
	Ogre::Vector3 Velocity;
	Ogre::Vector3 OffsetPos;
	float Size;
};

// ����/Ӱ��������

struct EmitterAffectorData_t
{
	// ����������

	int		emission_rate; 
	float	time_to_live ;

	float	width ;
	float	height;
	float	depth ;

	float	velocity_min; 
	float	velocity_max; 

	float	size_min;
	float	size_max;

	Ogre::Vector3 direction_min;
	Ogre::Vector3 direction_max;

	Ogre::ColourValue colour_range_start;
	Ogre::ColourValue colour_range_end; 

	// Ӱ��������

	Ogre::Vector3 direction; 
	float strength;
	Ogre::ColourValue colorData; 

	EmitterAffectorData_t(COTESimpleEmitter* pemitter, COTEParticleSystemAffector* affector)
	{
		// ����������

		emission_rate = pemitter->GetIntVal("������");
		time_to_live  = pemitter->GetfloatVal("����");	

		width  = pemitter->GetfloatVal("������_���");
		height = pemitter->GetfloatVal("������_�߶�");
		depth  = pemitter->GetfloatVal("������_����");	

		velocity_min = pemitter->GetfloatVal("��С����");
		velocity_max = pemitter->GetfloatVal("�������");	

		size_min = pemitter->GetfloatVal("��С�ߴ�");
		size_max = pemitter->GetfloatVal("���ߴ�");

		direction_min = pemitter->GetVector3Val("��С����");
		direction_max = pemitter->GetVector3Val("�����");

		colour_range_start = pemitter->GetCorVal("��С��ɫ");
		colour_range_end   = pemitter->GetCorVal("�����ɫ");

		// Ӱ��������
		
		direction = affector->GetVector3Val("direction");

		strength = affector->GetfloatVal("strength");

		colorData = affector->GetCorVal("colorData");
	}



};

// ***********************************************************
// class COTEEmiterReactor
// ***********************************************************
class COTEEmiterAffectorReactorBase : public CReactorBase
{
public:
	// ��ʼ��
	virtual void Init(Ogre::MovableObject* mo);

	// ����
	virtual void Update(Ogre::MovableObject* mo, float dTime);	
		
	// ����
	virtual void Clear();

protected:

	virtual void CreateElement(Ogre::MovableObject* mo, const Ogre::Vector3& rPos) = 0;

	virtual void InitElement(Ogre::MovableObject* mo, ParticleElement_t& pe) = 0;

	virtual void UpdateElement(Ogre::MovableObject* mo, ParticleElement_t& pe) = 0;


public:

	std::string				m_ParticleResName;

protected:

	std::vector<ParticleElement_t>			m_ElmList;

	EmitterAffectorData_t*					m_EmitterAffectorData;

};

// ***********************************************************
// COTEBBSEmiterAffector
// ***********************************************************
class COTEBBSEmiterAffector : public COTEEmiterAffectorReactorBase
{
protected:
	
	virtual void CreateElement(Ogre::MovableObject* mo, const Ogre::Vector3& rPos);

	virtual void InitElement(Ogre::MovableObject* mo, ParticleElement_t& pe);

	virtual void UpdateElement(Ogre::MovableObject* mo, ParticleElement_t& pe);
};
// ***********************************************************
// COTEEntityEmiterAffector
// ***********************************************************
class COTEEntityEmiterAffector : public COTEEmiterAffectorReactorBase
{
protected:
	
	virtual void CreateElement(Ogre::MovableObject* mo, const Ogre::Vector3& rPos);

	virtual void InitElement(Ogre::MovableObject* mo, ParticleElement_t& pe);

	virtual void UpdateElement(Ogre::MovableObject* mo, ParticleElement_t& pe);
};

// ***********************************************
// COTEEmiterReactorFactory 
// ***********************************************
class COTEEmiterReactorFactory : public CReactorFactoryBase
{
public:	

	virtual CReactorBase* Create(const Ogre::MovableObject* mo, const std::string& rResName)
	{			
		COTEEmiterAffectorReactorBase* emitter = NULL;

		if(mo->getMovableType() == "BBS")
		{
			emitter = new COTEBBSEmiterAffector();	
			emitter->m_ParticleResName = rResName;			
		}
		else if(mo->getMovableType() == "OTEEntity")
		{			
			emitter = new COTEEntityEmiterAffector();	
			emitter->m_ParticleResName = rResName;	
		}

		return emitter;
	}
	
	virtual void Delete(CReactorBase* rb)
	{
		SAFE_DELETE(rb)
	}

	virtual void GetDescription(std::string& rDesc)
	{
		rDesc = "���ܷ�����";
	}
};


}
