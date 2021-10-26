#include "stdafx.h"

#include "OTEStaticInclude.h"

#include "OTEPluginManager.h"
#include "OTEPluginInterface.h"

#include "EntEditorDlg.h"
#include "EntCrtDlg.h"

#include "OTEQTSceneManager.h"
#include "OTECollisionManager.h"

using namespace Ogre;
using namespace OTE;

int gMeshEditorMenuID = 0;
int gMeshEditorToolID = 0;
int gMeshEditorGridToolID = 0; 


// ========================================================
// CEntityEditorPluginInterface
// ========================================================

class  _OTEExport CEntityEditorPluginInterface : public COTEPluginInterface
{
private:	
	int   m_MenuID; 
	int   m_ToolID;
	HashMap<unsigned int, OTEMenu_t>	m_MenuGroup;	
	HashMap<unsigned int, OTETool_t>	m_ToolGroup;	

public:
	CEntityEditorPluginInterface()
	{			
	}
	// --------------------------------------------------------
	virtual void OnMenuEven(int menuID)
	{
		if(gMeshEditorMenuID == menuID)
		{
			AFX_MANAGE_STATE(AfxGetStaticModuleState());
			if(!CMeshEditorDlg::s_Inst.m_hWnd || !CMeshEditorDlg::s_Inst.IsWindowEnabled())
			   CMeshEditorDlg::s_Inst.Create( IDD_EntEditMain, NULL);
			CMeshEditorDlg::s_Inst.ShowWindow(SW_SHOW);		
		
		}
	}

	// --------------------------------------------------------
	virtual void OnToolEven(int toolID)
	{
		if(gMeshEditorToolID == toolID)
		{			
			AFX_MANAGE_STATE(AfxGetStaticModuleState());
			if(!CMeshEditorDlg::s_Inst.m_hWnd)
				CMeshEditorDlg::s_Inst.Create( IDD_EntEditMain, NULL);			
			CMeshEditorDlg::s_Inst.ShowWindow(SW_SHOW);
		}	
	}
	// --------------------------------------------------------
	virtual bool OnLButtonDown(HWND hwnd, int x, int y, bool isControlDown, bool isAltDown, bool isShiftDown)
	{	
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
		if(isAltDown) // 如果处于观察状态就不进行操作
		{
			return false;
		}
	
		CRect r;	
		::GetWindowRect(hwnd, r);
		
		bool createSuccess = false;
		if(CEntCrtDlg::sInst.m_hWnd && CEntCrtDlg::sInst.IsWindowVisible())	
			createSuccess = CEntCrtDlg::sInst.OnSceneLBDown((float)x / r.Width(), (float)y / r.Height());	
	
		if(!createSuccess && CMeshEditorDlg::s_Inst.m_hWnd && CMeshEditorDlg::s_Inst.IsWindowVisible())
			CMeshEditorDlg::s_Inst.OnLButtonDown((float)x / r.Width(), (float)y / r.Height());
	
		return false;
	}

	// --------------------------------------------------------
	virtual bool OnRButtonDown(HWND hwnd, int x, int y, bool isControlDown, bool isAltDown, bool isShiftDown)
	{	
		if(isAltDown) // 如果处于观察状态就不进行操作
		{
			return false;
		}
			
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
		if(!CMeshEditorDlg::s_Inst.m_hWnd || !CMeshEditorDlg::s_Inst.IsWindowVisible())
			return false;

		CRect r;	
		::GetWindowRect(hwnd, r);

		if(CEntCrtDlg::sInst.m_hWnd)
			CEntCrtDlg::sInst.ClearMouseEntity();		

		return false;
	}		
	
	// --------------------------------------------------------
	virtual bool OnLeftDClick(HWND hwnd, int x, int y, bool isControlDown, bool isAltDown, bool isShiftDown)
	{	
		if(isAltDown) // 如果处于观察状态就不进行操作
		{
			return false;
		}

		AFX_MANAGE_STATE(AfxGetStaticModuleState());		
		if(!CMeshEditorDlg::s_Inst.m_hWnd || !CMeshEditorDlg::s_Inst.IsWindowVisible())
			return false;	

		CRect r;	
		::GetWindowRect(hwnd, r);

		if(CMeshEditorDlg::s_Inst.m_hWnd)
			CMeshEditorDlg::s_Inst.OnLButtonDClick((float)x / r.Width(), (float)y / r.Height());
		
		return false;
	}
	// --------------------------------------------------------
	virtual bool OnLButtonUp(HWND hwnd, int x, int y, bool isControlDown, bool isAltDown, bool isShiftDown)
	{	
		if(isAltDown) // 如果处于观察状态就不进行操作
		{
			return false;
		}	

		AFX_MANAGE_STATE(AfxGetStaticModuleState());	
		if(!CMeshEditorDlg::s_Inst.m_hWnd || !CMeshEditorDlg::s_Inst.IsWindowVisible())
			return false;

		CRect r;	
		::GetWindowRect(hwnd, r);

		if(CMeshEditorDlg::s_Inst.m_hWnd)
			CMeshEditorDlg::s_Inst.OnLButtonUp((float)x / r.Width(), (float)y / r.Height());
		
		return false;
	}
	// --------------------------------------------------------
	virtual bool OnMouseMove(HWND hwnd, int x, int y, bool isControlDown, bool isAltDown, bool isShiftDown)
	{		
		if(isAltDown) // 如果处于观察状态就不进行操作
		{
			return false;
		}

		AFX_MANAGE_STATE(AfxGetStaticModuleState());
		if(!CMeshEditorDlg::s_Inst.m_hWnd || !CMeshEditorDlg::s_Inst.IsWindowVisible())
			return false;

		CRect r;	
		::GetWindowRect(hwnd, r);		

		static CPoint oldPos = CPoint(x, y);

		if(CMeshEditorDlg::s_Inst.m_hWnd && CMeshEditorDlg::s_Inst.IsWindowVisible())
		{
			CMeshEditorDlg::s_Inst.OnSceneMouseMove(Ogre::Vector2(x - oldPos.x, y - oldPos.y),(float)x / r.Width(), (float)y / r.Height());
		}
			
		if(CEntCrtDlg::sInst.m_hWnd && CEntCrtDlg::sInst.IsWindowVisible())
		{			
			CEntCrtDlg::sInst.OnSceneMouseMove((float)x / r.Width(), (float)y / r.Height());
		}

		oldPos = CPoint(x, y);

		// HOVER

		if( CMeshEditorDlg::s_Inst.m_hWnd && 
			CMeshEditorDlg::s_Inst.IsWindowVisible() )
		{
			if(CMeshEditorDlg::s_TransMgr.IsVisible() && !(::GetKeyState(VK_LBUTTON) & 0x80))
			{
				CRect r;	
				::GetWindowRect(hwnd, r);
				CMeshEditorDlg::s_TransMgr.HoverDummies((float)x / r.Width(), (float)y / r.Height());
			}
		}

		return false;

	}
	// --------------------------------------------------------
	virtual bool CreateLButtonUpListener(){return true;}
	// --------------------------------------------------------
	virtual bool CreateKeyEventListener(){return true;}
	// --------------------------------------------------------
	virtual int CreateOTEMenuGroup(int startMenuID, HashMap<int, OTEMenu_t>& rMenuGroup)
	{
		OTEMenu_t tMenu;
		tMenu.pPI = this;
		// menu1
		tMenu.MenuID	= startMenuID;
		tMenu.MenuTitile = "场景实体编辑器";
		tMenu.MenuTips = "";
		tMenu.MenuParentTitle = "工具";
		gMeshEditorMenuID = tMenu.MenuID;

		m_MenuGroup[tMenu.MenuID] = tMenu;
		rMenuGroup[tMenu.MenuID] = tMenu;		

		// return last id
		return tMenu.MenuID;
	}
	// --------------------------------------------------------
	virtual int CreateOTEToolGroup(int startToolID, HashMap<int, OTETool_t>& rToolGroup)
	{
		OTETool_t tTool;
		tTool.pPI = this;
		
		// button1
		tTool.ToolID	= startToolID;
		tTool.ToolTitile = "";
		tTool.ToolTips = "场景实体编辑器";
		tTool.IconFileName = "gui/实体编辑器.bmp";
		m_ToolGroup[tTool.ToolID] = tTool;
		rToolGroup[tTool.ToolID] = tTool;

		gMeshEditorToolID = tTool.ToolID;		

		// return last id
		return tTool.ToolID;
	}
	// --------------------------------------------------------
	virtual bool OnKeyDown(unsigned int keyCode, bool isControlDown, bool isAltDown, bool isShiftDown)
	{	

		AFX_MANAGE_STATE(AfxGetStaticModuleState());
		if(!CMeshEditorDlg::s_Inst.m_hWnd || !CMeshEditorDlg::s_Inst.IsWindowVisible())
			return false;

		if(CMeshEditorDlg::s_Inst.m_hWnd)
			CMeshEditorDlg::s_Inst.OnSceneKeyDown();

		if(::GetKeyState('I') & 0x80)
		{
			if(CMeshEditorDlg::s_Inst.m_hWnd && 
				CMeshEditorDlg::s_Inst.IsWindowVisible() && 
				SCENE_MGR->IsEntityPresent("PlayerHelper"))
			{
				CMeshEditorDlg::s_Inst.ShowWindow(SW_HIDE);
			}
		}

		return false;
	}

};

// ========================================================
// 绘制接口
// ========================================================
void OnRenderScene(int ud)
{	
	if(!CMeshEditorDlg::s_Inst.m_hWnd || !CMeshEditorDlg::s_Inst.IsWindowVisible())
		return;

	/*static DWORD f = GetTickCount();
	DWORD s = GetTickCount();

	std::vector<Ogre::MovableObject*> elist;
	SCENE_MGR->GetEntityList(&elist);
	for(int i = 0; i < elist.size(); i ++)
	{
		COTEActorEntity* mo = (COTEActorEntity*)elist[i];

		if(mo == NULL || mo->getName().find("Ent_") == -1)
			continue;

		mo->UpdateState((s - f) / 1000.0f);
	}

	f = s;*/
	
	if(CMeshEditorDlg::s_Inst.m_hWnd)
		CMeshEditorDlg::s_TransMgr.DrawDummies();


}

// --------------------------------------------------------
void OnClearRender(int ud)
{
	if(CMeshEditorDlg::s_Inst.m_Snap3DMark)
	{ 
		CMeshEditorDlg::s_Inst.m_Snap3DMark = NULL;
		CMeshEditorDlg::s_EditState =  "选择";
	}

	CMeshEditorDlg::s_pSelGroup.clear();

	CEntCrtDlg::sInst.m_MouseObj = NULL;
}

// ========================================================
// Plugin 接口处理
// ========================================================
	
CEntityEditorPluginInterface gPI;
// --------------------------------------------------------
extern "C" void __declspec(dllexport) dllStartPlugin( void )
{		
	COTEPluginManager::GetInstance()->AddPluginInterface(&gPI);	

	SCENE_MGR->AddListener(COTEQTSceneManager::eRenderCB, OnRenderScene);

	SCENE_MGR->AddListener(COTEQTSceneManager::eSceneClear, OnClearRender);

}
// --------------------------------------------------------
extern "C" void __declspec(dllexport) dllStopPlugin( void )
{	
	// 回调函数注册

	SCENE_MGR -> RemoveListener(OnRenderScene);
	SCENE_MGR -> RemoveListener(OnClearRender);

	COTEPluginManager::GetInstance()->RemovePluginInterface(&gPI);		
}
