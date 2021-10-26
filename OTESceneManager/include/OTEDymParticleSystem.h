#pragma once
#include "OTEStaticInclude.h"

// ********************************************
// COTEParticle
// ���ӵ��壬 ���ӿ�������ʵ��
// ********************************************
class COTEParticle
{
public:

	float			m_LifeTime;		// ����

	void*			m_EntityPtr;	// ���ӿ������κ�ʵ��

public:
	
};

// ********************************************
// COTEEmitter
// ������������Ƚ϶�
// ********************************************
class COTEEmitter
{
public:
	void Emiter(){}
};

// ********************************************
// COTEAffector
// Ӱ���� 
// ********************************************
class COTEAffector
{
public:
	void Affector(){}
};


// ********************************************
// COTEDymParticleSystem
// ����ϵͳ������һ������������
// ��������ϵͳҪ�ѷ�������Ӱ����Ҳ����������
// һ������ϵͳ��ʵ���Ӧһ������������Ч����
// ********************************************
class COTEDymParticleSystem
{
public:
	
	void Init();

	void Destroy();

	void Proccess();

public:

	COTEDymParticleSystem(void){}
	~COTEDymParticleSystem(void){}

public:

	// ϵͳ�������б�

	std::vector<COTEParticle*>	m_Particles;
};
