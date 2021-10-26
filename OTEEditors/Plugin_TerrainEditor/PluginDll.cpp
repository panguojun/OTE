#include "stdafx.h"
#include "OTEStaticInclude.h"
#include "OTEPluginManager.h"
#include "OTEPluginInterface.h" 
#include "OTEQTSceneManager.h"
#include "OTETerrainTexturePic.h"
#include "OTETilePage.h"
#include "OTEFog.h"

#include "TerrainEditorDlg.h"
#include "HeightEdtorDlg.h"
#include "TerrainMaterialEditor.h"
#include "GrassEditorDlg.h"
#include "WaterEditorDlg.h"
#include "LightmapEditDlg.h"
#include "TakeShot.h"

using namespace Ogre;
using namespace OTE;

extern CTerrainTexturePic* terrainPic;	//纹理编辑显示的图片

// ========================================================
// 全局变量
// ========================================================

int	 gTerrainMainToolID = 0;

// ========================================================
void ClearBrush()
{
	// 清理笔刷

	CHeightEditorDlg::s_Inst.m_BrushEntity.DeleteBrushEntity();	
	CTerrainMaterialEditor::s_Inst.m_BrushEntity.DeleteBrushEntity();
	CGrassEditorDlg::s_Inst.m_VegBrush.DeleteBrushEntity();
	CWaterEditorDlg::s_Inst.m_WaterBrush.DeleteBrushEntity();	
	CLightmapEditDlg::s_Inst.m_LightmapBrush.DeleteBrushEntity();

}

// ========================================================
// CActorEditorPluginInterface
// ========================================================

class  CTerrainEditorPluginInterface : public COTEPluginInterface
{
private:	
	int   m_MenuID;
	int   m_ToolID;
	HashMap<unsigned int, OTEMenu_t>	m_MenuGroup;	
	HashMap<unsigned int, OTETool_t>	m_ToolGroup;	

public:
	CTerrainEditorPluginInterface()
	{			
	}

	// --------------------------------------------------------
	virtual void OnMenuEven(int menuID)
	{

	}

	// --------------------------------------------------------
	virtual void OnToolEven(int toolID)
	{
		if(gTerrainMainToolID == toolID)
		{			
			AFX_MANAGE_STATE(AfxGetStaticModuleState());
			if(!CTerrainEditorDlg::s_Inst.m_hWnd)
			{
				CTerrainEditorDlg::s_Inst.Create( IDD_TERRAIN_EDITOR, NULL);			
				CTerrainEditorDlg::s_Inst.m_TabCtrl.SetCurSel(0);
			}
			//CTerrainEditorDlg::s_Inst.ShowWindow(SW_SHOW);

			if(!CHeightEditorDlg::s_Inst.m_hWnd)
			{
				CHeightEditorDlg::s_Inst.Create( IDD_HEIGHT_EDITOR, NULL);			
				CTerrainEditorDlg::s_Inst.m_TabCtrl.SetCurSel(1);
			}
			//CHeightEditorDlg::s_Inst.ShowWindow(SW_SHOW);		

			CTerrainEditorDlg::s_Inst.ShowWindow(SW_SHOW);
			switch(CTerrainEditorDlg::s_Inst.m_TabCtrl.GetCurSel())
			{
			case 0:
				CTerrainMaterialEditor::s_Inst.ShowWindow(SW_HIDE);			
				CHeightEditorDlg::s_Inst.ShowWindow(SW_SHOW);
				break;
			case 1:
				CHeightEditorDlg::s_Inst.ShowWindow(SW_HIDE);				
				CTerrainMaterialEditor::s_Inst.ShowWindow(SW_SHOW);
				break;
			case 2:
				CHeightEditorDlg::s_Inst.ShowWindow(SW_HIDE);
				CTerrainMaterialEditor::s_Inst.ShowWindow(SW_HIDE);
				
				break;
			}
		}

	}

	// --------------------------------------------------------	
	virtual bool OnLButtonDown(HWND hwnd, int x, int y, bool isControlDown, bool isAltDown, bool isShiftDown)
	{	
		if(isAltDown)
			return false;

		CRect r;	
		::GetWindowRect(hwnd, r);	

		if(CHeightEditorDlg::s_Inst.m_hWnd && CHeightEditorDlg::s_Inst.IsWindowVisible())
		{
			CHeightEditorDlg::s_Inst.OnLButtonDown((float)x / r.Width(), (float)y / r.Height());
			return false;
		}

		if(CTerrainMaterialEditor::s_Inst.m_hWnd && CTerrainMaterialEditor::s_Inst.IsWindowVisible())
		{
			CTerrainMaterialEditor::s_Inst.OnLButtonDownA(x/* / r.Width()*/, y /*/ r.Height()*/, r.Width(), r.Height());
			return false;
		}
		
		if(CLightmapEditDlg::s_Inst.m_hWnd && CLightmapEditDlg::s_Inst.IsWindowVisible())
		{
			CLightmapEditDlg::s_Inst.OnLButtonDown((float)x / r.Width(), (float)y / r.Height());
			return false;
		}
			
		return false;
	}

	// --------------------------------------------------------
	virtual bool OnLButtonUp(HWND hwnd, int x, int y, bool isControlDown, bool isAltDown, bool isShiftDown)
	{
		if(isAltDown)
			return false;

		CRect r;	
		::GetWindowRect(hwnd, r);

		if(CHeightEditorDlg::s_Inst.m_hWnd && CHeightEditorDlg::s_Inst.IsWindowVisible())
		{
			CHeightEditorDlg::s_Inst.OnLButtonUp((float)x / r.Width(), (float)y / r.Height());	
			return true;		
		}

		return false;
	}

	// --------------------------------------------------------
	virtual bool OnRButtonDown(HWND hwnd, int x, int y, bool isControlDown, bool isAltDown, bool isShiftDown)
	{
		if(isAltDown)
			return false;

		CRect r;	
		::GetWindowRect(hwnd, r);
		//::GetClientRect(hwnd, r);

		if(CTerrainMaterialEditor::s_Inst.m_hWnd && CTerrainMaterialEditor::s_Inst.IsWindowVisible())
			CTerrainMaterialEditor::s_Inst.OnLButtonDownA(x, y, r.Width(), r.Height());	

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

		CRect r;	
		::GetWindowRect(hwnd, r);

		if(CHeightEditorDlg::s_Inst.m_hWnd && CHeightEditorDlg::s_Inst.IsWindowVisible())
			CHeightEditorDlg::s_Inst.OnMouseMove((float)x / r.Width(), (float)y / r.Height());	
		
		if(CTerrainMaterialEditor::s_Inst.m_hWnd && CTerrainMaterialEditor::s_Inst.IsWindowVisible())
			return CTerrainMaterialEditor::s_Inst.OnMouseMove(x, y, r.Width(), r.Height());	

		if(CGrassEditorDlg::s_Inst.m_hWnd && CGrassEditorDlg::s_Inst.IsWindowVisible())
			CGrassEditorDlg::s_Inst.OnMouseMove(x, y, r.Width(), r.Height());	

		if(CWaterEditorDlg::s_Inst.m_hWnd && CWaterEditorDlg::s_Inst.IsWindowVisible())
			return CWaterEditorDlg::s_Inst.OnMouseMove(x, y, r.Width(), r.Height());
		
		if(CLightmapEditDlg::s_Inst.m_hWnd && CLightmapEditDlg::s_Inst.IsWindowVisible())
			CLightmapEditDlg::s_Inst.OnMouseMove(x, y, r.Width(), r.Height());
		
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

		// button2
		tTool.ToolID		= startToolID;
		tTool.ToolTitile	= "";
		tTool.ToolTips		= "山体编辑器";
		tTool.IconFileName	= "gui/山体编辑器.bmp";

		m_ToolGroup[tTool.ToolID]	= tTool;
		rToolGroup[tTool.ToolID]	= tTool;

		gTerrainMainToolID = tTool.ToolID;	

		// return last id
		return tTool.ToolID;
	}

	// --------------------------------------------------------
	virtual bool OnKeyDown(unsigned int keyCode, bool isControlDown, bool isAltDown, bool isShifDwon)
	{
		if(CHeightEditorDlg::s_Inst.m_hWnd && CHeightEditorDlg::s_Inst.IsWindowVisible())
			CHeightEditorDlg::s_Inst.OnKeyDown(keyCode);	


		// 牌照
		
		if(keyCode == 'P')
		{
			CTakeShot::s_Inst.TakeShot();
		}	

		// 摄像机位置
		
		if(keyCode == 'C' && !isControlDown)
		{
			// 摄像机
			Ogre::Camera* cam = SCENE_CAM;

			if(COTETilePage::GetCurrentPage())
			{				
				float x = COTETilePage::GetCurrentPage()->m_PageSizeX / 2.0f;
				float z = COTETilePage::GetCurrentPage()->m_PageSizeZ / 2.0f;
				float y = COTETilePage::GetCurrentPage()->GetHeightAt(x, z) + 5.0f;

				cam->setPosition(Ogre::Vector3(x, y, z));
			}
			else
			{
				cam->setPosition(Ogre::Vector3(0, 10, 10));
			}
		}

		return false;
	}

	// --------------------------------------------------------
	virtual void OnCustomMessage(const std::string& msg)
	{
		if(msg == "EM_HIDEWINDOWS")
		{
			AFX_MANAGE_STATE(AfxGetStaticModuleState());

			CTerrainEditorDlg::s_Inst.ShowWindow(SW_HIDE);
		}
	}

};

// --------------------------------------------------------
void OnClearRender(int ud)
{
	ClearBrush();	
}

// ========================================================
// Plugin 接口处理
// ========================================================
namespace Ogre
{	
	CTerrainEditorPluginInterface gPI;
	// --------------------------------------------------------
	extern "C" void __declspec(dllexport) dllStartPlugin( void )
	{		
		COTEPluginManager::GetInstance()->AddPluginInterface(&gPI); 		
			
		SCENE_MGR->AddListener(COTEQTSceneManager::eSceneClear, OnClearRender);

		OTE::COTEFog::GetInstance()->SetFogEnable(false); // 暂时
	}

	// --------------------------------------------------------	
	extern "C" void __declspec(dllexport) dllStopPlugin( void )
	{		
		delete terrainPic;	

		SCENE_MGR->RemoveListener(OnClearRender);	

		ClearBrush();

		// the plugin
		COTEPluginManager::GetInstance()->RemovePluginInterface(&gPI);
	}

}