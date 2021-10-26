#include "stdafx.h"

#   ifdef GAME_DEBUG
	
	#include "AppFrame.h"

	#include "OTEQTSceneManager.h"
	#include "MainFrm.h"
	#include "OTEOgreManager.h"	
	#include "OTEPluginManager.h"  

	#include "AppFrameDoc.h"
	#include "AppFrameView.h"  
	
	// --------------------------------------------------------
	#ifdef _DEBUG
	#define new DEBUG_NEW
	#endif
	// --------------------------------------------------------
	using namespace Ogre;
	using namespace OTE;

	// --------------------------------------------------------

	typedef bool (*pFunGameCoreStart) (Ogre::RenderWindow* mainWin);
	typedef void (*pFunGameCoreEnd) ();

	HMODULE ghModule;

	// ----------------------------------------------------------	
	HashMap<int, OTETool_t>				g_ToolMap;
	std::vector<COTEPluginInterface*>	g_PIMouseEventListeners;
	std::vector<COTEPluginInterface*>	g_PIKeyEventListeners;

	#define TOOL_PLUGINS_BEGIN			1001		
	#define TOOL_PLUGINS_END			1101	

	//
	////////////////////////////////////////////////////////////////
	///////////////////////////// CMainFrame ///////////////////////
	////////////////////////////////////////////////////////////////
	//

	IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

	BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)

		ON_WM_ACTIVATE()
		ON_WM_CREATE()
		ON_WM_DESTROY()	
		ON_WM_MOUSEWHEEL()

		ON_COMMAND_RANGE(TOOL_PLUGINS_BEGIN, TOOL_PLUGINS_END, OnToolBar )
		
		ON_WM_LBUTTONDBLCLK()
		
		ON_WM_SIZE()	

	END_MESSAGE_MAP()

	// ----------------------------------------------

	CMainFrame::CMainFrame()
	{
		// TODO: 在此添加成员初始化代码
	}

	CMainFrame::~CMainFrame()
	{
		
	}

	// ----------------------------------------------
	int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
	{
		if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
			return -1;		

		//ShowControlBar(&m_wndToolBar ,false ,false); 

		return 0;
	}

	// ----------------------------------------------
	void CMainFrame::OnActivate(UINT nState, CWnd * pWndOther, BOOL bMinmized)
	{
		
	}
	// ----------------------------------------------
	void CMainFrame::InitOgre()
	{
		CAppFrameView *pView=(CAppFrameView *)this->GetActiveView();
		if(!( COTEOgreManager::Instance()->Init((HWND)pView->m_hWnd)))	
			return;	

		// Plugins 
		LoadPlugins(".\\oteplugins.cfg");
		COTEPluginManager::GetInstance()->m_hMainWnd = this->m_hWnd; // 窗口架构句柄

		if(COTEPluginManager::GetInstance()->GetPluginNum() == 0)
		{	
			// 启动客户端
			// ----------------------------------------------------------				

			ghModule	= ::LoadLibrary( "GameCore.dll");	
			if(!ghModule)
			{				
				::MessageBox(NULL, "加载GameCore.dll失败！", "Error", MB_OK);
				return;
			}

			pFunGameCoreStart pFunCoreStart = (pFunGameCoreStart)::GetProcAddress( ghModule, "StartDll" );

			if(!pFunCoreStart)
			{
				::MessageBox(NULL, "启动客户端失败！", "Error", MB_OK);
				return;
			}

			if(!(*pFunCoreStart)(COTEOgreManager::Instance()->m_pWindow))
			{
				return;
			}		
		
		}

		// ----------------------------------------------------------	
		//go render!
		COTEOgreManager::Instance()->StartRender();
		
		// 最后卸载dll

		if(ghModule)
		{
			::FreeLibrary(ghModule);
		}	
	}

	// ----------------------------------------------
	void CMainFrame::OnDestroy( )
	{
		// 清理插件

		ClearPlugins();
		COTEPluginManager::GetInstance()->UnloadPlugins();		
		COTEPluginManager::GetInstance()->Clear();

		// 退出客户端
		if(ghModule)
		{
			pFunGameCoreEnd pFunCoreEnd = (pFunGameCoreEnd)::GetProcAddress( ghModule, "StopDll" );
			(*pFunCoreEnd)();
		}

		// Ogre退出
		COTEOgreManager::Instance()->OgrExit();	//exit first 		

		CFrameWnd::OnDestroy();			
	}

	// ----------------------------------------------
	BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
	{
		if( !CFrameWnd::PreCreateWindow(cs) )
			return FALSE;

		cs.hMenu   =   NULL;

		return TRUE;
	}

	// ----------------------------------------------

	#ifdef _DEBUG
	void CMainFrame::AssertValid() const
	{
		CFrameWnd::AssertValid();
	}

	void CMainFrame::Dump(CDumpContext& dc) const
	{
		CFrameWnd::Dump(dc);
	}

	#endif //_DEBUG

	// --------------------------------------------------
	// 清理插件

	void CMainFrame::ClearPlugins()
	{
		g_ToolMap.clear();
		g_PIMouseEventListeners.clear();
		g_PIKeyEventListeners.clear();
		COTEPluginManager::GetInstance()->EndPlugins();
		COTEPluginManager::GetInstance()->RemoveAllPluginInterfaces();
	}

	// --------------------------------------------------
	// Load Plugins

	void CMainFrame::LoadPlugins(const char* szPluginCfg)
	{
		// Load Plugins
		COTEPluginManager::GetInstance()->LoadPlugins(szPluginCfg);	
		COTEPluginManager::GetInstance()->StartPlugins();			

		// ETArt接口
		//::LoadLibrary( "ETArt.dll");

		// 如果插件则不出现工具条菜单等
		if(COTEPluginManager::GetInstance()->GetPluginNum() == 0)
		{	
			// 隐藏鼠标

			//::ShowCursor(false);		
			
			return;
		}		

		// Create Tool bar

		if(!m_wndToolBar.m_hWnd)
		{
			if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
				| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC))
			{
				TRACE0("未能创建工具栏\n");
				return;      // 未能创建
			}

			m_wndToolBar.SetHeight(32); 
			m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
			EnableDocking(CBRS_ALIGN_ANY);
			DockControlBar(&m_wndToolBar);	
		}

		// Create Plugin Tools
		int tStartID = TOOL_PLUGINS_BEGIN;
 		for(unsigned int i = 0; i < COTEPluginManager::GetInstance()->m_PluginInterfaceList.size(); i++)
		{
			// 从插件创建

			HashMap<int, OTETool_t> toolMap;
			int tResult = COTEPluginManager::GetInstance()->m_PluginInterfaceList.at(i)->CreateOTEToolGroup(tStartID, toolMap);
			if(tResult != INVALID_ID)
				tStartID = tResult;	
			
			int count = 0;
			HashMap<int, OTETool_t>::iterator tItA = toolMap.begin();
			for(; tItA != toolMap.end(); tItA ++)
			{
				OTETool_t tTool = tItA->second;

				// 创建UI
					
				HBITMAP   hbmp;   
				hbmp   =   (HBITMAP)::LoadImage(   AfxGetInstanceHandle(),     
											tTool.IconFileName.c_str(),IMAGE_BITMAP,0,0,     
											LR_LOADFROMFILE);     
				CBitmap*   bitmap   =   CBitmap::FromHandle(hbmp);		
				int bitmapInd = m_wndToolBar.GetToolBarCtrl().AddBitmap(1, bitmap);

				TBBUTTON tbbutton;
				tbbutton.dwData    =   0;   
				tbbutton.fsState   =   TBSTATE_ENABLED;   
				tbbutton.fsStyle   =   TBSTYLE_BUTTON;   
				tbbutton.iBitmap   =   bitmapInd;   
				tbbutton.idCommand =   tTool.ToolID;   			
				tbbutton.iString   =   -1; 
				
				m_wndToolBar.GetToolBarCtrl().AddButtons(1, &tbbutton);

				CRect toolButtonRect;
				m_wndToolBar.GetItemRect(count ++, &toolButtonRect);
				
				// 添加到全局map中

				g_ToolMap[tItA->first] = tTool;
				
			}
			
			// 创建间隔		

			TBBUTTON tbSepButton;
			tbSepButton.dwData    =   0;   
			tbSepButton.fsState   =   TBSTATE_ENABLED;   
			tbSepButton.fsStyle   =   TBSTYLE_SEP;   
			tbSepButton.iBitmap   =   -1;   
			tbSepButton.idCommand =   -1;   
			tbSepButton.iString   =   -1; 
			m_wndToolBar.GetToolBarCtrl().AddButtons(1, &tbSepButton);			

			++ tStartID;		
		}			

		ShowControlBar(&m_wndToolBar, true, false); 	
		
		// Add Mouse event listener
		
		for(unsigned int i = 0; i < COTEPluginManager::GetInstance()->m_PluginInterfaceList.size(); i++)
		{
			bool tResult = false;
			tResult |= COTEPluginManager::GetInstance()->m_PluginInterfaceList.at(i)->CreateLButtonDownListener();
			tResult |= COTEPluginManager::GetInstance()->m_PluginInterfaceList.at(i)->CreateLButtonUpListener();
			tResult |= COTEPluginManager::GetInstance()->m_PluginInterfaceList.at(i)->CreateMouseMoveListener();
			tResult |= COTEPluginManager::GetInstance()->m_PluginInterfaceList.at(i)->CreateRButtonDownListener();
			tResult |= COTEPluginManager::GetInstance()->m_PluginInterfaceList.at(i)->CreateRButtonUpListener();
			tResult |= COTEPluginManager::GetInstance()->m_PluginInterfaceList.at(i)->CreateWheelListener();
		
			if(tResult)
			{
				g_PIMouseEventListeners.push_back(COTEPluginManager::GetInstance()->m_PluginInterfaceList.at(i));
			}
		}

		// Add Key event listener
		
		for(unsigned int i = 0; i < COTEPluginManager::GetInstance()->m_PluginInterfaceList.size(); i++)
		{
			bool tResult = COTEPluginManager::GetInstance()->m_PluginInterfaceList.at(i)->CreateKeyEventListener();
			if(tResult)
			{
				g_PIKeyEventListeners.push_back(COTEPluginManager::GetInstance()->m_PluginInterfaceList.at(i));
			}
		}

		// 初始化众插件

		for(unsigned int i = 0; i < COTEPluginManager::GetInstance()->m_PluginInterfaceList.size(); i++)
		{		
			COTEPluginManager::GetInstance()->m_PluginInterfaceList.at(i)->Init((HWND)this->m_hWnd);	
		}
		
	}

	// --------------------------------------------------
	// ToolBar

	void CMainFrame::OnToolBar(UINT nID)
	{
		HashMap<int, OTETool_t>::iterator tItA = g_ToolMap.find(nID);
		if(tItA != g_ToolMap.end())
			tItA->second.pPI->OnToolEven(nID);
	}

	// --------------------------------------------------

	void CMainFrame::OnLButtonDblClk(UINT nFlags, CPoint point)
	{	
		CFrameWnd::OnLButtonDblClk(nFlags, point);
	}

	// --------------------------------------------------

	void CMainFrame::OnSize(UINT nType, int cx, int cy)
	{
		//if(nType == SIZE_MAXIMIZED)
		{
			if(COTEOgreManager::Instance()->m_pWindow)
			{
				//COTEOgreManager::Instance()->m_pWindow->resize(cx, cy);
				SCENE_CAM->setAspectRatio(float(cx) / cy);
			}
		}
		CFrameWnd::OnSize(nType, cx, cy);		
	}
	
	// --------------------------------------------------

#   endif
