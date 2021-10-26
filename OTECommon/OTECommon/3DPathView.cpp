#include "3DPathView.h"
#include "OTEQTSceneManager.h"

using namespace Ogre;
using namespace OTE;

// ---------------------------------------------------
void C3DPathView::ClearRes()		
{	
	CLineObj* ent = NULL;
	if(!m_EntName.empty())
	{
		SceneNode* sn = SCENE_MGR->GetSceneNode(m_EntName);
		if(sn)
		{
			ent= (CLineObj*)(sn->getAttachedObject(0)); 				
			if(ent)								
				delete ent;					
			
			SCENE_MGR->getRootSceneNode()->removeChild(m_EntName.c_str());
			SCENE_MGR->destroySceneNode(m_EntName.c_str());

		}
		m_EntName = ""; 
	}
	
}
	
// ---------------------------------------------------
void C3DPathView::Update()
{	
	// clear first

	if(!m_EntName.empty())
	{
		ClearRes();			
	}

	if(!m_IsVisible || m_NodeList.size() == 0)
		return;

	// create 

	m_EntName = m_Name;

	CLineObj* ent = new CLineObj(m_EntName); 	
	
	SceneNode* node = SCENE_MGR->getRootSceneNode()->createChildSceneNode(m_EntName, Vector3( 0, 0, 0 ));
	node->attachObject(ent); 	

	// ªÊ÷∆

	std::vector<Ogre::Vector3>::iterator it = m_NodeList.begin();
	while(it != m_NodeList.end())
	{
		ent->addPoint(*it);

		++ it;
	}
	
	ent->changeColor(ColourValue(.8,.0,.8)); // ∆¨¿∂…´

	ent->drawLines(); 
}

// ---------------------------------------------------
void C3DPathView::AddPathNode(const Ogre::Vector3& nodePos)
{
	m_NodeList.push_back(nodePos);
}