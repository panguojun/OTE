#include "stdafx.h"

#include "OTEStaticInclude.h"
#include "OTEPluginManager.h"
#include "OTEPluginInterface.h"
#include "OTEQTSceneManager.h"
#include "ActorEditorMainDlg.h"
#include "HookEditDlg.h"
#include "KeyFrameDlg.h"
#include "OTECollisionManager.h"
#include "AnimationEdit.h"
#include "ActionEdit.h"
#include "MeshEditDlg.h"

using namespace Ogre;
using namespace OTE;

// ========================================================
// 全局变量
// ========================================================

int gActorEditorMainToolID = 0;

int gActorTestToolID = 0;

OTE::COTEQTSceneManager*		gSceneMgr;			// 场景管理器

// ========================================================
// 渲染回调
// ========================================================

void OnSceneRender(int ud)
{	
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if(!CActorEditorMainDlg::s_Inst.m_hWnd || !CActorEditorMainDlg::s_Inst.IsWindowVisible())
		return;

	CActorEditorMainDlg::s_TransMgr.DrawDummies();
}


// --------------------------------------------------------
void OnClearRender(int ud)
{
	CActorEditorMainDlg::s_pActorEntity = NULL;
}


// ========================================================
// CActorEditorPluginInterface
// ========================================================

class  CActorEditorPluginInterface : public COTEPluginInterface
{
private:	
	int   m_MenuID;
	int   m_ToolID;
	HashMap<unsigned int, OTEMenu_t>	m_MenuGroup;	
	HashMap<unsigned int, OTETool_t>	m_ToolGroup;	

public:
	CActorEditorPluginInterface()
	{			
	}

	// --------------------------------------------------------
	virtual void OnMenuEven(int menuID)
	{

	}

	// --------------------------------------------------------
	virtual void OnToolEven(int toolID)
	{
		if(gActorEditorMainToolID == toolID)
		{
			AFX_MANAGE_STATE(AfxGetStaticModuleState());
			if(!CActorEditorMainDlg::s_Inst.m_hWnd)
				CActorEditorMainDlg::s_Inst.Create( IDD_ActorEditorMain, NULL);			
			CActorEditorMainDlg::s_Inst.ShowWindow(SW_SHOW);		
		}
		
	}
	// --------------------------------------------------------
	virtual bool CreateLButtonUpListener(){return true;}
	// --------------------------------------------------------
	virtual bool CreateKeyEventListener(){return true;}
	// --------------------------------------------------------	
	virtual bool OnLButtonDown(HWND hwnd, int x, int y, bool isControlDown, bool isAltDown, bool isShiftDown)
	{	
		if(isAltDown)
			return false;		

		AFX_MANAGE_STATE(AfxGetStaticModuleState());	

		CRect r;	
		::GetWindowRect(hwnd, r);
	
		bool result = false;	
		
		if( CActorEditorMainDlg::s_Inst.m_hWnd && 
			CActorEditorMainDlg::s_Inst.IsWindowVisible() )
		{
			if(CActorEditorMainDlg::s_TransMgr.IsVisible())
				CActorEditorMainDlg::s_TransMgr.PickDummies((float)x / r.Width(), (float)y / r.Height());
			
			if(isControlDown || CActorEditorMainDlg::s_StartSelection)
			{
				// 场景中拾取实体			

				Ogre::Ray tCameraRay = SCENE_CAM->getCameraToViewportRay(
					(float)x / r.Width(), (float)y / r.Height() );
				Ogre::MovableObject* mo = 					
						OTE::COTEObjectIntersection::TriangleIntersection(tCameraRay, "", "", LONG_MAX, true).first;
									
				if(mo && CActorEditorMainDlg::s_pActorEntity != (OTE::COTEActorEntity*)(mo))
				{
					if(mo->getMovableType() != "OTEEntity")
					{
						return false;
					}

					if(CActorEditorMainDlg::s_pActorEntity)
					{
						/// 清理

						CActorEditorMainDlg::s_pActorEntity->getParentSceneNode()->showBoundingBox(false);
						
						CActionEdit::s_Inst.m_pAct = NULL;					
						
						CKeyFrameDlg::s_Inst.DestoryMe();						
					}

					CActorEditorMainDlg::s_pActorEntity = (OTE::COTEActorEntity*)(mo);

					SCENE_MGR->SetCurFocusObj(CActorEditorMainDlg::s_pActorEntity);
					
					CActorEditorMainDlg::s_Inst.UpdateUI();	

					CActorEditorMainDlg::s_StartSelection = false;					
				}	

				result |= 1;
			}

		}

		if(CHookEditDlg::s_Inst.m_hWnd && CHookEditDlg::s_Inst.IsWindowVisible())
		{
			CHookEditDlg::s_Inst.OnSceneLButtonDown((float)x / r.Width(), (float)y / r.Height());
			result |= 1;
		}

		if(CActionEdit::s_Inst.m_hWnd && CActionEdit::s_Inst.IsWindowVisible())
		{
			CActionEdit::s_Inst.OnSceneLButtonDown((float)x / r.Width(), (float)y / r.Height());
			result |= 1;
		}

		if(CKeyFrameDlg::s_Inst.m_hWnd && CKeyFrameDlg::s_Inst.IsWindowVisible())
		{
			CKeyFrameDlg::s_Inst.OnSceneLButtonDown(x, y);
			result |= 1;
		}



		return result;
	}

	// --------------------------------------------------------	
	virtual bool OnLButtonUp(HWND hwnd, int x, int y, bool isControlDown, bool isAltDown, bool isShiftDown)
	{	
		if(isAltDown)
			return false;

		AFX_MANAGE_STATE(AfxGetStaticModuleState());

		CRect r;	
		::GetWindowRect(hwnd, r);

		if(CKeyFrameDlg::s_Inst.m_hWnd && CKeyFrameDlg::s_Inst.IsWindowVisible())
		{
			CKeyFrameDlg::s_Inst.OnSceneLButtonUp(x, y);			
		}

		if(CHookEditDlg::s_Inst.m_hWnd && CHookEditDlg::s_Inst.IsWindowVisible())
		{
			CHookEditDlg::s_Inst.OnSceneLButtonUp((float)x / r.Width(), (float)y / r.Height());
			
		}		

		if(CActionEdit::s_Inst.m_hWnd && CActionEdit::s_Inst.IsWindowVisible())
		{
			CActionEdit::s_Inst.OnSceneLButtonUp((float)x / r.Width(), (float)y / r.Height());
			
		}

		return false;
	}

	// --------------------------------------------------------	
	virtual bool OnRButtonUp(HWND hwnd, int x, int y, bool isControlDown, bool isAltDown, bool isShiftDown)
	{	
		if(isAltDown)
			return false;

		AFX_MANAGE_STATE(AfxGetStaticModuleState());

		CRect r;	
		::GetWindowRect(hwnd, r);

		if(CActionEdit::s_Inst.m_hWnd && CActionEdit::s_Inst.IsWindowVisible())
		{
			CActionEdit::s_Inst.OnSceneRButtonUp((float)x / r.Width(), (float)y / r.Height());
			return false;
		}

		return false;
	}
	// --------------------------------------------------------
	virtual bool OnWheel(HWND hwnd, int x, int y, int delta, bool isControlDown, bool isAltDown, bool isShiftDown)
	{	
		return false;
	}
	
	// --------------------------------------------------------
	virtual bool OnMouseMove(HWND hwnd, int x, int y, bool isControlDown, bool isAltDown, bool isShiftDown)
	{
		if(isAltDown)
			return false;		

		AFX_MANAGE_STATE(AfxGetStaticModuleState());
		if(CKeyFrameDlg::s_Inst.m_hWnd && CKeyFrameDlg::s_Inst.IsWindowVisible()/*&& CKeyFrameDlg::s_Inst.m_bPush*/)
		{
			if(!isControlDown && !isAltDown)
				CKeyFrameDlg::s_Inst.OnSceneMouseMove(x, y, isShiftDown);
			
		}
		else if(CHookEditDlg::s_Inst.m_hWnd && CHookEditDlg::s_Inst.mHook != NULL &&
				CHookEditDlg::s_Inst.IsWindowVisible())
		{
			if(!isControlDown && !isAltDown && !isShiftDown)
				CHookEditDlg::s_Inst.OnMouseMove(x, y);
			
		}

		// HOVER

		if( CActorEditorMainDlg::s_Inst.m_hWnd && 
			CActorEditorMainDlg::s_Inst.IsWindowVisible() )
		{
			if(CActorEditorMainDlg::s_TransMgr.IsVisible() && !(::GetKeyState(VK_LBUTTON) & 0x80) )
			{
				CRect r;	
				::GetWindowRect(hwnd, r);
				CActorEditorMainDlg::s_TransMgr.HoverDummies((float)x / r.Width(), (float)y / r.Height());
			}

			COTEPluginManager::GetInstance()->SetUserString("EditState",  "true");	
		}

		return false;
	}

	// --------------------------------------------------------
	virtual int CreateOTEToolGroup(int startToolID, HashMap<int, OTETool_t>& rToolGroup)
	{
		OTETool_t tTool;
		tTool.pPI = this;
		
		// button1
		tTool.ToolID		= startToolID;
		tTool.ToolTitile	= "";
		tTool.ToolTips		= "角色编辑器";
		tTool.IconFileName	= "gui/角色编辑器.bmp";

		m_ToolGroup[tTool.ToolID]	= tTool;		
		rToolGroup[tTool.ToolID]	= tTool;

		gActorEditorMainToolID = tTool.ToolID;			

		// return last id
		return tTool.ToolID;
	}

	// --------------------------------------------------------
	virtual bool OnKeyDown(unsigned int keyCode, bool isControlDown, bool isAltDown, bool isShifDwon)
	{
		if(::GetKeyState(VK_ESCAPE) & 0x80)
		{
			SCENE_MGR->ClearCurFocusObj();			
		}
		else if(::GetKeyState('I') & 0x80)
		{
			if(CActorEditorMainDlg::s_Inst.m_hWnd && CActorEditorMainDlg::s_Inst.IsWindowVisible() && 
				SCENE_MGR->IsEntityPresent("PlayerHelper")
				)
			{
				CActorEditorMainDlg::s_Inst.ShowWindow(SW_HIDE);
				CKeyFrameDlg::s_Inst.ShowWindow(SW_HIDE);
				CActorEditorMainDlg::s_pActorEntity = NULL;
			}
		}

		return false;
	}

	// --------------------------------------------------------
	virtual void OnCustomMessage(const std::string& msg)
	{
		if(msg == "OM_OBJPROPERTY" || msg == "OM_OBJACTION")
		{
			AFX_MANAGE_STATE(AfxGetStaticModuleState());
			if(SCENE_MGR->GetCurObjSection() && SCENE_MGR->GetCurObjSection()->getMovableType().find("OTEEntity") != std::string::npos)
			{
				if(!CActorEditorMainDlg::s_Inst.m_hWnd)
					CActorEditorMainDlg::s_Inst.Create( IDD_ActorEditorMain, NULL);			
				CActorEditorMainDlg::s_Inst.ShowWindow(SW_SHOW);
				CActorEditorMainDlg::s_pActorEntity = (COTEActorEntity*)SCENE_MGR->GetCurObjSection();
				
				std::stringstream ss;
				ss << "编辑角色对象 - " << CActorEditorMainDlg::s_pActorEntity->getName();
				CActorEditorMainDlg::s_Inst.SetWindowText(ss.str().c_str());

				if(msg == "OM_OBJACTION")
				{
					CActorEditorMainDlg::s_Inst.m_TabCtrl.SetCurSel(3);
					CActorEditorMainDlg::s_Inst.OnTcnSelchangeTab(NULL, NULL);
				}

				CActorEditorMainDlg::s_Inst.UpdateUI();					

				COTEPluginManager::GetInstance()->SetUserString("EditState", "true");
			}
		}
		else if(msg == "EM_HIDEWINDOWS")
		{
			AFX_MANAGE_STATE(AfxGetStaticModuleState());

			CActorEditorMainDlg::s_Inst.ShowWindow(SW_HIDE);
		}
	}	

};

// ========================================================
// Plugin 接口处理
// ========================================================
namespace Ogre
{	
	CActorEditorPluginInterface gPI;
	// --------------------------------------------------------
	extern "C" void __declspec(dllexport) dllStartPlugin( void )
	{		
		COTEPluginManager::GetInstance()->AddPluginInterface(&gPI);	
			
		// 渲染回调
			
		SCENE_MGR->AddListener(COTEQTSceneManager::eRenderCB, OnSceneRender);
		
		SCENE_MGR->AddListener(COTEQTSceneManager::eSceneClear, OnClearRender);

	}
	// --------------------------------------------------------
	extern "C" void __declspec(dllexport) dllStopPlugin( void )
	{
		// 回调函数

		SCENE_MGR -> RemoveListener(OnSceneRender);	

		SCENE_MGR -> RemoveListener(OnClearRender);			

		AFX_MANAGE_STATE(AfxGetStaticModuleState());

		CKeyFrameDlg::s_Inst.OnEndPlugin();
		CActionEdit::s_Inst.OnEndPlugin();
		CMeshEditDlg::s_Inst.OnEndPlugin();		

		// the plugin
		COTEPluginManager::GetInstance()->RemovePluginInterface(&gPI);
	}

}
