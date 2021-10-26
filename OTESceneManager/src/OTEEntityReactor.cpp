#include "OTEEntityReactor.h"
#include "OTED3DManager.h"
#include "OTEQTSceneManager.h"
#include "OgreD3D9RenderSystem.h" 
#include "OTEEntityRender.h" 

using namespace OTE;
using namespace Ogre;

// ------------------------------------
// 初始化
void COTEEntityReactor::Init(Ogre::MovableObject* mo)
{
	COTEActorEntity* ae = (COTEActorEntity*)mo;

	// shader 类型反应器	
	ae->m_pGPUReactor = this; // 邦定

	if(!m_HLSLName.empty())
	{
		std::vector<COTESubEntity*>* selist = ae->GetSubEntityList();
		std::vector<COTESubEntity*>::iterator ita = selist->begin();
		while(ita != selist->end())
		{	
			((COTESubEntity*)(*ita))->CreateVertexEffect(m_HLSLName);				
			++ ita;
		}
	}
	m_BindEntity = mo;
}

// ------------------------------------
void COTEEntityReactor::UpdateParams()
{
	// 传递参数	

	if( FAILED(g_pDevice->SetVertexShaderConstantF(4, &COTED3DManager::s_ElapsedTime, 1)))
	{
		OGRE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, 
			"Unable to upload vertex shader float parameters", 
			"COTEEntityReactor::UpdateParams");
	}
}

// ------------------------------------
COTEEntityReactor::~COTEEntityReactor()
{
	// shader 类型反应器

	((COTEActorEntity*)m_BindEntity)->m_pGPUReactor = NULL; // 邦定

	std::vector<COTESubEntity*>* selist = ((COTEActorEntity*)m_BindEntity)->GetSubEntityList();
	std::vector<COTESubEntity*>::iterator ita = selist->begin();
	while(ita != selist->end())
	{	
		((COTESubEntity*)(*ita))->ClearEffect();				
		++ ita;
	}				
}

