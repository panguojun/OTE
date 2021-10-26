// --------------------------------------------------------
#include "stdafx.h"
#include "OTEStaticInclude.h"
#include "OgreD3D9RenderSystem.h" 
#include "OgreD3D9HardwareVertexBuffer.h" 
#include "OTEPluginManager.h"
#include "OTEPluginInterface.h"
#include "OTEQTSceneManager.h"
#include "otelineobj.h"
#include "OTETerrainLightmapMgr.h"
#include "OTETerrainSerializer.h"
#include "OTESceneXmlLoader.h"
#include "OTECollisionManager.h"

#include "EffDoc.h"
#include "ParticleEditMainDlg.h"
#include "ParticleCrtDlg.h"

#include "ParticleEditDlg.h"

using namespace Ogre;
using namespace OTE; 

// ========================================================
// 全局变量
// ========================================================

int gEditorMainToolID;

// ========================================================
// 渲染回调
// ========================================================

// 第一祯渲染
void RenderOnce()
{
	

}


// ========================================================
// CParticleEditorPluginInterface
// ========================================================

class  _OTEExport CParticleEditorPluginInterface : public COTEPluginInterface
{
private:	
	int   m_MenuID;
	int   m_ToolID;
	HashMap<unsigned int, OTEMenu_t>	m_MenuGroup;	
	HashMap<unsigned int, OTETool_t>	m_ToolGroup;	

public:
	CParticleEditorPluginInterface()
	{			
	}
	// --------------------------------------------------------
	virtual void OnToolEven(int toolID)
	{
		if(toolID == gEditorMainToolID)
		{
			AFX_MANAGE_STATE(AfxGetStaticModuleState());
			if(!CParticleEditMainDlg::s_Inst.m_hWnd)
			{				
				CParticleEditMainDlg::s_Inst.Create(IDD_PARTICLE_EDIT_MAINDLG);
			}
			CParticleEditMainDlg::s_Inst.ShowWindow(SW_SHOW);


		}
	}
	// --------------------------------------------------------
	virtual bool OnLButtonDown(HWND hwnd, int x, int y, bool isControlDown, bool isAltDown, bool isShiftDown)
	{	
		CRect r;	
		::GetWindowRect(hwnd, r);

		if( CParticleEditDlg::s_Inst.m_hWnd && 
			CParticleEditDlg::s_Inst.IsWindowVisible() )
		{
			if(isControlDown)
			{
				// 场景中拾取实体			

				Ogre::Ray tCameraRay = SCENE_MGR->getCamera("CamMain")->getCameraToViewportRay(
															(float)x / r.Width(), (float)y / r.Height() );
				std::list <Ogre::MovableObject*> list;
				COTEMagCollisionManager::PickAABB(tCameraRay, list);
				if(list.size() == 0)
					return false;

				Ogre::MovableObject* mo = *list.begin();

				if(mo && (Ogre::MovableObject*)CEffDoc::s_Inst.m_ParticleSystem != mo)
				{
					if(CEffDoc::s_Inst.m_ParticleSystem)
						CEffDoc::s_Inst.m_ParticleSystem->getParentSceneNode()->showBoundingBox(false);

					CEffDoc::s_Inst.m_ParticleSystem = (OTE::COTEParticleSystem*)(mo);
					SCENE_MGR->SetCurFocusObj(CEffDoc::s_Inst.m_ParticleSystem);

					CEffDoc::s_Inst.m_ParticleSystem->getParentSceneNode()->showBoundingBox(true);
					
					OTE::COTEParticleSystemRes* resPtr = CEffDoc::s_Inst.m_ParticleSystem->GetResPtr();
					OTE::COTESimpleEmitter* emitter = (OTE::COTESimpleEmitter*)resPtr->m_Emitter;
					emitter->Show(CEffDoc::s_Inst.m_ParticleSystem);

					CParticleEditDlg::s_Inst.UpdatePropertyList();					
					
					return true;
				}						
				
			}

		}

		return false;

	}
	// --------------------------------------------------------
	virtual bool OnMouseMove(HWND hwnd, int x, int y, bool isControlDown, bool isAltDown, bool isShiftDown)
	{	
		//CRect r;	
		//::GetWindowRect(hwnd, r);
		//static int ox, oy;

		//Ogre::Ray ray = SCENE_MGR->getCamera("CamMain")->getCameraToViewportRay(float(x) / r.Width(), float(y) / r.Height());
		//Ogre::Vector3 pos;
		//if(CEffDoc::s_Inst.m_ParticleSystem && COTECollisionManager::GetInstance()->PickZeroFloor(ray, pos))
		//{
		//	CEffDoc::s_Inst.m_ParticleSystem->getParentSceneNode()->setPosition(pos);

		//	//if(x - ox != 0 || y - oy != 0 )
		//	//	CEffDoc::s_Inst.m_ParticleSystem->m_Inertia = Ogre::Vector3(1.0f, 0, 0) * 2.0f;
		//	//else
		//	//	CEffDoc::s_Inst.m_ParticleSystem->m_Inertia = Ogre::Vector3(0, 0, 0);
		//}	


		//ox = x;oy = y;

		return false;
	}
	// --------------------------------------------------------
	virtual bool CreateLButtonUpListener(){return true;}
	// --------------------------------------------------------
	virtual bool CreateKeyEventListener(){return true;}
	
	// --------------------------------------------------------
	virtual int CreateOTEToolGroup(int startToolID, HashMap<int, OTETool_t>& rToolGroup)
	{
		OTETool_t tTool;
		tTool.pPI = this;
		
		// button1
		tTool.ToolID		= startToolID;
		tTool.ToolTitile	= "";
		tTool.ToolTips		= "粒子系统编辑器";
		tTool.IconFileName	= "gui/粒子编辑器.bmp";

		m_ToolGroup[tTool.ToolID]	= tTool;
		rToolGroup[tTool.ToolID]	= tTool;

		gEditorMainToolID = tTool.ToolID;
		

		// return last id
		return tTool.ToolID;
	}

	// --------------------------------------------------------
	virtual bool OnKeyDown(unsigned int keyCode, bool isControlDown, bool isAltDown, bool isShifDwon)
	{	
		return false;
	}
		
	// --------------------------------------------------------
	virtual void OnCustomMessage(const std::string& msg)
	{
		if(msg == "OM_OBJPROPERTY" || msg == "OM_OBJACTION")
		{
			AFX_MANAGE_STATE(AfxGetStaticModuleState());

			Ogre::MovableObject* mo = SCENE_MGR->GetCurObjSection();

			if(mo && mo->getMovableType() == "PS")
			{		
				if(CEffDoc::s_Inst.m_ParticleSystem)
					CEffDoc::s_Inst.m_ParticleSystem->getParentSceneNode()->showBoundingBox(false);

				CEffDoc::s_Inst.m_ParticleSystem = (OTE::COTEParticleSystem*)(mo);			

				CEffDoc::s_Inst.m_ParticleSystem->getParentSceneNode()->showBoundingBox(true);
	
				SCENE_MGR->SetCurFocusObj(CEffDoc::s_Inst.m_ParticleSystem);

				OTE::COTEParticleSystemRes* resPtr = CEffDoc::s_Inst.m_ParticleSystem->GetResPtr();
				OTE::COTESimpleEmitter* emitter = (OTE::COTESimpleEmitter*)resPtr->m_Emitter;
				emitter->Show(CEffDoc::s_Inst.m_ParticleSystem);

				if(!CParticleEditMainDlg::s_Inst.m_hWnd)
				{				
					CParticleEditMainDlg::s_Inst.Create(IDD_PARTICLE_EDIT_MAINDLG);
				}
				CParticleEditMainDlg::s_Inst.ShowWindow(SW_SHOW);
				CParticleEditDlg::s_Inst.UpdatePropertyList();					
			}
		}		
	}


};

// ========================================================
// Plugin 接口处理
// ========================================================
namespace Ogre
{	
	CParticleEditorPluginInterface gPI;
	// --------------------------------------------------------
	extern "C" void _OTEExport DllStartPlugin( void )
	{		
		COTEPluginManager::GetInstance()->AddPluginInterface(&gPI);		

	}
	// --------------------------------------------------------
	extern "C" void _OTEExport DllStopPlugin( void )
	{			
		// the plugin
		COTEPluginManager::GetInstance()->RemovePluginInterface(&gPI);

	}

}