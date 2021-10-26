#   ifdef GAME_DEBUG 

	#include "stdafx.h"
	#include "AppFrame.h"
	#include "MainFrm.h"
	
	#include "AppFrameDoc.h"
	#include "AppFrameView.h"
	#include "Resource.h"

	#ifdef _DEBUG
	#define new DEBUG_NEW
	#endif

	// CAppFrameApp

	BEGIN_MESSAGE_MAP(CAppFrameApp, CWinApp)

	END_MESSAGE_MAP()


	// CAppFrameApp 构造

	CAppFrameApp::CAppFrameApp()
	{
		// TODO: 在此处添加构造代码，
		// 将所有重要的初始化放置在 InitInstance 中
	}


	// 唯一的一个 CAppFrameApp 对象

	CAppFrameApp theApp;

	// CAppFrameApp 初始化

	BOOL CAppFrameApp::InitInstance()
	{
		// 如果一个运行在 Windows XP 上的应用程序清单指定要
		// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
		//则需要 InitCommonControls()。否则，将无法创建窗口。
		InitCommonControls();

		CWinApp::InitInstance();

		// 初始化 OLE 库
		if (!AfxOleInit())
		{
			AfxMessageBox("初始化 OLE 库 失败。");
			return FALSE;
		}
		AfxEnableControlContainer();
		
		SetRegistryKey(_T("应用程序向导生成的本地应用程序"));
		LoadStdProfileSettings(4);  // 加载标准 INI 文件选项(包括 MRU)
		// 注册应用程序的文档模板。文档模板
		// 将用作文档、框架窗口和视图之间的连接
		CSingleDocTemplate* pDocTemplate;
		pDocTemplate = new CSingleDocTemplate(
			IDR_MAINFRAME,
			RUNTIME_CLASS(CAppFrameDoc),
			RUNTIME_CLASS(CMainFrame),       // 主 SDI 框架窗口
			RUNTIME_CLASS(CAppFrameView));

		if (!pDocTemplate)
			return FALSE;

		AddDocTemplate(pDocTemplate);
		// 分析标准外壳命令、DDE、打开文件操作的命令行
		CCommandLineInfo cmdInfo;
		ParseCommandLine(cmdInfo);
		// 调度在命令行中指定的命令。如果
		// 用 /RegServer、/Register、/Unregserver 或 /Unregister 启动应用程序，则返回 FALSE。
		if (!ProcessShellCommand(cmdInfo))
			return FALSE;
		// 唯一的一个窗口已初始化，因此显示它并对其进行更新
		m_pMainWnd->MoveWindow(0, 0, 1024, 768);
		m_pMainWnd->ShowWindow(SW_SHOW);
		m_pMainWnd->UpdateWindow();
		m_pMainWnd->SetWindowText("Game Brooder");

		// 仅当存在后缀时才调用 DragAcceptFiles，
		//  在 SDI 应用程序中，这应在 ProcessShellCommand  之后发生

		((CMainFrame*)m_pMainWnd)->InitOgre();

		// 将直接退出
		
		return FALSE;
	}

#   else

	// 发布版本

	#include "OTEInterfaceInclude.h"

	#define __MemoryManager_H__
	
	#include "OgreDynLibManager.h"
	#include "OTEOgreManager.h"
	#include "OTEFrameListener.h"

	typedef bool (*pFunGameCoreStart) (Ogre::RenderWindow* mainWin);
	typedef void (*pFunGameCoreEnd) ();

	HMODULE ghModule;

	INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT)
	{
		// Initialize with the window handle.
		if(!COTEOgreManager::Instance()->Init(NULL))	
			return 0;

		// 启动客户端
		// ----------------------------------------------------------				

		ghModule	= ::LoadLibrary( "GameCore.dll");
		if(!ghModule)
		{
			::MessageBox(NULL, "加载GameCore.dll失败", "Error", MB_OK);
			return 0;
		}

		pFunGameCoreStart pFunCoreStart = (pFunGameCoreStart)::GetProcAddress( ghModule, "StartDll" );

		if(!pFunCoreStart)
		{
			::MessageBox(NULL, "启动客户端失败！", "Error", MB_OK);
			return 0;
		}

		if(!(*pFunCoreStart)(COTEOgreManager::Instance()->m_pWindow))
		{
			return 0;
		}	
	

		// ----------------------------------------------------------	
		//go render!

		COTEOgreManager::Instance()->StartRender();	

		return 0;
	}


#   endif 