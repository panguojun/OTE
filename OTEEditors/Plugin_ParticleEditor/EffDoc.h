#pragma once
#include "OTEParticleSystem.h"
// *************************************
// ��Ч����
// *************************************

class CEffDoc
{
public:
	static CEffDoc	s_Inst;
	
public:
	CEffDoc(void);
	~CEffDoc(void);

public:
	OTE::COTEParticleSystem*		m_ParticleSystem;
	
	std::string					m_ParticleSystemTempName;

};
