#include "stdafx.h"

#include "OTEStaticInclude.h"

#include "OTEPluginManager.h"
#include "OTEPluginInterface.h"

#include "EntEditorDlg.h"
#include "EntCrtDlg.h"

#include "OTEQTSceneManager.h"
#include "OTECollisionManager.h"
#include "OTED3DManager.h"
#include "D3DToolbox.h"


using namespace Ogre;
using namespace OTE;

int gMeshEditorMenuID = 0;
int gMeshEditorToolID = 0;
int gMeshEditorGridToolID = 0; 

// ========================================================
// CLogicEditorPluginInterface
// ========================================================

class  _OTEExport CLogicEditorPluginInterface : public COTEPluginInterface
{
private:	
	int   m_MenuID; 
	int   m_ToolID;
	HashMap<unsigned int, OTEMenu_t>	m_MenuGroup;	
	HashMap<unsigned int, OTETool_t>	m_ToolGroup;	

public:
	CLogicEditorPluginInterface()
	{			
	}
	// --------------------------------------------------------
	virtual void OnMenuEven(int menuID)
	{
		if(gMeshEditorMenuID == menuID)
		{
			AFX_MANAGE_STATE(AfxGetStaticModuleState());
			if(!CLogicEditorDlg::s_Inst.m_hWnd || !CLogicEditorDlg::s_Inst.IsWindowEnabled())
			   CLogicEditorDlg::s_Inst.Create( IDD_EntEditMain, NULL);
			CLogicEditorDlg::s_Inst.ShowWindow(SW_SHOW);		
		
		}
	}

	// --------------------------------------------------------
	virtual void OnToolEven(int toolID)
	{
		if(gMeshEditorToolID == toolID)
		{			
			AFX_MANAGE_STATE(AfxGetStaticModuleState());
			if(!CLogicEditorDlg::s_Inst.m_hWnd)
				CLogicEditorDlg::s_Inst.Create( IDD_EntEditMain, NULL);			
			CLogicEditorDlg::s_Inst.ShowWindow(SW_SHOW);
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
	
		if(!createSuccess && CLogicEditorDlg::s_Inst.m_hWnd && CLogicEditorDlg::s_Inst.IsWindowVisible())
			CLogicEditorDlg::s_Inst.OnLButtonDown((float)x / r.Width(), (float)y / r.Height());
	
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
		if(!CLogicEditorDlg::s_Inst.m_hWnd || !CLogicEditorDlg::s_Inst.IsWindowVisible())
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
		if(!CLogicEditorDlg::s_Inst.m_hWnd || !CLogicEditorDlg::s_Inst.IsWindowVisible())
			return false;	

		CRect r;	
		::GetWindowRect(hwnd, r);

		if(CLogicEditorDlg::s_Inst.m_hWnd)
			CLogicEditorDlg::s_Inst.OnLButtonDClick((float)x / r.Width(), (float)y / r.Height());
		
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
		if(!CLogicEditorDlg::s_Inst.m_hWnd || !CLogicEditorDlg::s_Inst.IsWindowVisible())
			return false;

		CRect r;	
		::GetWindowRect(hwnd, r);

		if(CLogicEditorDlg::s_Inst.m_hWnd)
			CLogicEditorDlg::s_Inst.OnLButtonUp((float)x / r.Width(), (float)y / r.Height());
		
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
		if(!CLogicEditorDlg::s_Inst.m_hWnd || !CLogicEditorDlg::s_Inst.IsWindowVisible())
			return false;

		CRect r;	
		::GetWindowRect(hwnd, r);		

		static CPoint oldPos = CPoint(x, y);

		if(CLogicEditorDlg::s_Inst.m_hWnd && CLogicEditorDlg::s_Inst.IsWindowVisible())
		{
			CLogicEditorDlg::s_Inst.OnSceneMouseMove(Ogre::Vector2(x - oldPos.x, y - oldPos.y),(float)x / r.Width(), (float)y / r.Height());
		}
			
		if(CEntCrtDlg::sInst.m_hWnd && CEntCrtDlg::sInst.IsWindowVisible())
		{			
			CEntCrtDlg::sInst.OnSceneMouseMove((float)x / r.Width(), (float)y / r.Height());
		}

		oldPos = CPoint(x, y);

		// HOVER

		if( CLogicEditorDlg::s_Inst.m_hWnd && 
			CLogicEditorDlg::s_Inst.IsWindowVisible() )
		{
			if(CLogicEditorDlg::s_TransMgr.IsVisible() && !(::GetKeyState(VK_LBUTTON) & 0x80))
			{
				CRect r;	
				::GetWindowRect(hwnd, r);
				CLogicEditorDlg::s_TransMgr.HoverDummies((float)x / r.Width(), (float)y / r.Height());
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
		if(!CLogicEditorDlg::s_Inst.m_hWnd || !CLogicEditorDlg::s_Inst.IsWindowVisible())
			return false;

		if(CLogicEditorDlg::s_Inst.m_hWnd)
			CLogicEditorDlg::s_Inst.OnSceneKeyDown();

		if(::GetKeyState('I') & 0x80)
		{
			if(CLogicEditorDlg::s_Inst.m_hWnd && 
				CLogicEditorDlg::s_Inst.IsWindowVisible() && 
				SCENE_MGR->IsEntityPresent("PlayerHelper"))
			{
				CLogicEditorDlg::s_Inst.ShowWindow(SW_HIDE);
			}
		}

		return false;
	}

};

// ========================================================
// 绘制接口
// ========================================================

// ----------------------------------------------
// 渲染器
void RenderLines()
{

	LPDIRECT3DDEVICE9 g_pDevice = COTED3DManager::GetD3DDevice();
	Ogre::Camera* cam = SCENE_CAM;

	// ----------------------------------------------------
	// 保存现场

	// 矩阵
	
	D3DXMATRIX matrix, oldWorldMat, oldViewMat, oldPrjMat;
	g_pDevice->GetTransform(D3DTS_WORLD,		&oldWorldMat); // SAVE TRANSFORM
	g_pDevice->GetTransform(D3DTS_VIEW,			&oldViewMat); 
	g_pDevice->GetTransform(D3DTS_PROJECTION ,	&oldPrjMat); 

	// 设置世界矩阵到默认

	D3DXMatrixIdentity(&matrix);
	g_pDevice->SetTransform(D3DTS_WORLD,		&matrix);

	// ----------------------------------------------------
	// 设置渲染状态

	g_pDevice->SetVertexShader(0);
	g_pDevice->SetPixelShader(0);

	g_pDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
	/*
	g_pDevice->SetRenderState( D3DRS_AMBIENT, FALSE );
	g_pDevice->SetRenderState( D3DRS_COLORVERTEX, FALSE );*/

	// ZBuffer
	g_pDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
	
	// ----------------------------------------------------
	// DRAW

	DWORD tColor = 0xFF808FFF;	

	if(SCENE_MGR->GetCurObjSection() && SCENE_MGR->GetCurObjSection()->getMovableType() == "OTEEntity")
	{
		COTEActorEntity* npc = ((COTEActorEntity*)SCENE_MGR->GetCurObjSection());
		Ogre::Vector3 lastPt = npc->GetParentSceneNode()->getPosition();
		int numWP = npc->GetIntVal("WayPoints");
		for(int i = 0; i < numWP; i ++)
		{
			std::stringstream ss;
			ss << "wp_pos_" << i;
			Ogre::Vector3 pos = npc->GetVector3Val(ss.str()) + Ogre::Vector3(0, 1, 0);

			DrawLine(g_pDevice, D3DXVECTOR3(lastPt.x, lastPt.y, lastPt.z), D3DXVECTOR3(pos.x, pos.y, pos.z), tColor);
			lastPt = pos;
		}
	}
	
	//Ogre::Vector3 pos = s_CurrentActorEntity->getParentSceneNode()->getWorldPosition() +
	//	s_CurrentActorEntity->getParentSceneNode()->getOrientation() * boneNode->getWorldPosition();

	//DrawWireBall(g_pDevice, D3DXVECTOR3(pos.x, pos.y, pos.z), BONE_SHOW_SIZE, tColor);	
	
	// ----------------------------------------------------
	// 还原现场
	
	g_pDevice->SetTransform( D3DTS_WORLD,			&oldWorldMat);
	g_pDevice->SetTransform( D3DTS_VIEW,			&oldViewMat); 
	g_pDevice->SetTransform( D3DTS_PROJECTION,		&oldPrjMat); 
	
}

// ----------------------------------------------------
void OnRenderScene(int ud)
{	
	if(!CLogicEditorDlg::s_Inst.m_hWnd || !CLogicEditorDlg::s_Inst.IsWindowVisible())
		return;

	RenderLines();
	
	if(CLogicEditorDlg::s_Inst.m_hWnd)
		CLogicEditorDlg::s_TransMgr.DrawDummies();


	// 目标点 逻辑区域

	static unsigned int sLstUpdateTime = 0;
	if(::GetTickCount() - sLstUpdateTime > 1000) // 每一秒钟更新
	{
		sLstUpdateTime = ::GetTickCount();

		int secIndex = CLogicEditorDlg::s_Inst.GetLogicSectorAt(SCENE_CAM->getPosition());
		if(secIndex >= 0)
			CLogicEditorDlg::s_Inst.SetDlgItemText(IDC_LOGICSEC, CLogicEditorDlg::s_Inst.m_vLogicSectors[secIndex].Name);
		else
			CLogicEditorDlg::s_Inst.SetDlgItemText(IDC_LOGICSEC, "未知区域");
	}		

}

// --------------------------------------------------------
void OnClearRender(int ud)
{
	if(CLogicEditorDlg::s_Inst.m_Snap3DMark)
	{ 
		CLogicEditorDlg::s_Inst.m_Snap3DMark = NULL;
		CLogicEditorDlg::s_EditState =  "选择";
	}

	CLogicEditorDlg::s_pSelGroup.clear();

	CEntCrtDlg::sInst.m_MouseObj = NULL;

	// 清理Npc

    for(unsigned int i=0; i < CLogicEditorDlg::s_Inst.m_vLogicSectors.size(); i ++)
	{	
		if(!CLogicEditorDlg::s_Inst.m_vLogicSectors[i].isLoaded && CLogicEditorDlg::s_Inst.m_vLogicSectors[i].vNpcStartPos.size() == 0) 
			continue;
		CLogicEditorDlg::s_Inst.m_vLogicSectors[i].vNpcStartPos.clear();
	}
}

// ========================================================
// Plugin 接口处理
// ========================================================
	
CLogicEditorPluginInterface gPI;
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
