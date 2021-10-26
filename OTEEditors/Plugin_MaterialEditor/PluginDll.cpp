#include "stdafx.h"

#include "OTEPluginManager.h"
#include "OTEPluginInterface.h"
#include "OTEQTSceneManager.h"
#include "MaterialEditorMainDlg.h"
#include "OTECollisionManager.h"
#include "OTETilePage.h"

using namespace Ogre;
using namespace OTE;

// ========================================================
// 全局变量
// ========================================================

int gMaterialEditorMainToolID = 0;

// ========================================================
// CMaterialEditorPluginInterface
// ========================================================

class  _OTEExport CMaterialEditorPluginInterface : public COTEPluginInterface
{
private:	
	int   m_MenuID;
	int   m_ToolID;
	HashMap<unsigned int, OTEMenu_t>	m_MenuGroup;	
	HashMap<unsigned int, OTETool_t>	m_ToolGroup;	

public:
	CMaterialEditorPluginInterface()
	{			
	}

	// --------------------------------------------------------
	virtual void OnMenuEven(int menuID)
	{

	}

	// --------------------------------------------------------
	virtual void OnToolEven(int toolID)
	{
		if(gMaterialEditorMainToolID == toolID)
		{
			AFX_MANAGE_STATE(AfxGetStaticModuleState());
			if(!CMaterialEditorMainDlg::s_Inst.m_hWnd)
				CMaterialEditorMainDlg::s_Inst.Create( IDD_MeterialEditor, NULL);			
			CMaterialEditorMainDlg::s_Inst.ShowWindow(SW_SHOW);		
		}
		
	}

	// --------------------------------------------------------	
	virtual bool OnLButtonDown(HWND hwnd, int x, int y, bool isControlDown, bool isAltDown, bool isShiftDown)
	{
		if(isAltDown)
			return false;

		CRect r;	
		::GetWindowRect(hwnd, r);
		if(CMaterialEditorMainDlg::s_Inst.m_hWnd && CMaterialEditorMainDlg::s_Inst.IsWindowVisible())
		{
			if(isShiftDown)
			{
				// 场景中拾取实体			

				Ogre::Ray tCameraRay = SCENE_MGR->getCamera("CamMain")->getCameraToViewportRay(
					(float)x / r.Width(), (float)y / r.Height() );
				std::list<Ogre::MovableObject*> ml;
				COTECollisionManager::PickAABB(tCameraRay, ml);
				std::list<Ogre::MovableObject*>::iterator it = ml.begin();
				bool bFind = false;
				while(it != ml.end())
				{
					std::string en = (*it)->getName();
					if((*it)->getMovableType() == "OTEEntity")
					{
						std::list<Ogre::MovableObject*>::iterator it2 = ml.begin();
						bool isSmall = false;
						while(it2 != ml.end())
						{
							if((*it)->getBoundingRadius() > (*it2)->getBoundingRadius())
							{
								isSmall = false;
								break;
							}
							else
								isSmall = true;
							++it2;
						}

						if(isSmall && CMaterialEditorMainDlg::s_pObj != (*it))
						{
							if(CMaterialEditorMainDlg::s_pObj && CMaterialEditorMainDlg::s_pObj->getParentSceneNode())
								CMaterialEditorMainDlg::s_pObj->getParentSceneNode()->showBoundingBox(false);

							CMaterialEditorMainDlg::s_pObj = (*it);	
							if(CMaterialEditorMainDlg::s_pObj->getParentSceneNode())
								CMaterialEditorMainDlg::s_pObj->getParentSceneNode()->showBoundingBox(true);
		
							CMaterialEditorMainDlg::s_Inst.UpdataAbout();
							bFind = true;
							break;
						}
					}
					++it;	
				}
				
				if(!bFind)
				{
					if(OTE::COTETilePage::GetCurrentPage())
					{
						Ogre::Vector3 pos;
						COTETile* tile = OTE::COTETilePage::GetCurrentPage()->TerrainHitTest(tCameraRay, pos);
						if(tile)
						{
							CMaterialEditorMainDlg::s_pObj = tile;
							CMaterialEditorMainDlg::s_Inst.UpdataAbout();
						}
					}
				}
			}

		}
		return false;

	}
	
	// --------------------------------------------------------
	virtual void OnCustomMessage(const std::string& msg)
	{
		if(msg == "OM_OBJMATERIAL")
		{
			AFX_MANAGE_STATE(AfxGetStaticModuleState());

			Ogre::MovableObject* mo = SCENE_MGR->GetCurObjSection();

			if(mo && mo->getMovableType() == "OTEEntity")
			{
				if(CMaterialEditorMainDlg::s_pObj && CMaterialEditorMainDlg::s_pObj->getParentSceneNode())
					CMaterialEditorMainDlg::s_pObj->getParentSceneNode()->showBoundingBox(false);

				CMaterialEditorMainDlg::s_pObj = mo;				

				CMaterialEditorMainDlg::s_pObj->getParentSceneNode()->showBoundingBox(true);
							
				if(!CMaterialEditorMainDlg::s_Inst.m_hWnd)
					CMaterialEditorMainDlg::s_Inst.Create( IDD_MeterialEditor, NULL);			
				CMaterialEditorMainDlg::s_Inst.ShowWindow(SW_SHOW);	
				CMaterialEditorMainDlg::s_Inst.UpdataAbout();
			}
		}		
	}	


	// --------------------------------------------------------
	virtual bool OnWheel(HWND hwnd, int x, int y, int delta, bool isControlDown, bool isAltDown, bool isShiftDown)
	{
		return false;
	}
	
	// --------------------------------------------------------
	virtual bool OnMouseMove(HWND hwnd, int x, int y, bool isControlDown, bool isAltDown, bool isShiftDown)
	{
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
		tTool.ToolTips		= "材质编辑器";
		tTool.IconFileName	= "gui/材质编辑器.bmp";

		m_ToolGroup[tTool.ToolID]	= tTool;		
		rToolGroup[tTool.ToolID]	= tTool;

		gMaterialEditorMainToolID = tTool.ToolID;			

		// return last id
		return tTool.ToolID;
	}
	// --------------------------------------------------------
	virtual bool OnKeyDown(unsigned int keyCode, bool isControlDown, bool isAltDown, bool isShifDwon)
	{
		return false;
	}
};

// ========================================================
// Plugin 接口处理
// ========================================================
namespace Ogre
{	
	CMaterialEditorPluginInterface gPI;
	// --------------------------------------------------------
	extern "C" void _OTEExport dllStartPlugin( void )
	{		
		COTEPluginManager::GetInstance()->AddPluginInterface(&gPI);
	
	}
	// --------------------------------------------------------
	extern "C" void _OTEExport dllStopPlugin( void )
	{	
		// the plugin
		COTEPluginManager::GetInstance()->RemovePluginInterface(&gPI);
	}

}