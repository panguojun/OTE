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


	// CAppFrameApp ����

	CAppFrameApp::CAppFrameApp()
	{
		// TODO: �ڴ˴���ӹ�����룬
		// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
	}


	// Ψһ��һ�� CAppFrameApp ����

	CAppFrameApp theApp;

	// CAppFrameApp ��ʼ��

	BOOL CAppFrameApp::InitInstance()
	{
		// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
		// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
		//����Ҫ InitCommonControls()�����򣬽��޷��������ڡ�
		InitCommonControls();

		CWinApp::InitInstance();

		// ��ʼ�� OLE ��
		if (!AfxOleInit())
		{
			AfxMessageBox("��ʼ�� OLE �� ʧ�ܡ�");
			return FALSE;
		}
		AfxEnableControlContainer();
		
		SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));
		LoadStdProfileSettings(4);  // ���ر�׼ INI �ļ�ѡ��(���� MRU)
		// ע��Ӧ�ó�����ĵ�ģ�塣�ĵ�ģ��
		// �������ĵ�����ܴ��ں���ͼ֮�������
		CSingleDocTemplate* pDocTemplate;
		pDocTemplate = new CSingleDocTemplate(
			IDR_MAINFRAME,
			RUNTIME_CLASS(CAppFrameDoc),
			RUNTIME_CLASS(CMainFrame),       // �� SDI ��ܴ���
			RUNTIME_CLASS(CAppFrameView));

		if (!pDocTemplate)
			return FALSE;

		AddDocTemplate(pDocTemplate);
		// ������׼������DDE�����ļ�������������
		CCommandLineInfo cmdInfo;
		ParseCommandLine(cmdInfo);
		// ��������������ָ����������
		// �� /RegServer��/Register��/Unregserver �� /Unregister ����Ӧ�ó����򷵻� FALSE��
		if (!ProcessShellCommand(cmdInfo))
			return FALSE;
		// Ψһ��һ�������ѳ�ʼ���������ʾ����������и���
		m_pMainWnd->MoveWindow(0, 0, 1024, 768);
		m_pMainWnd->ShowWindow(SW_SHOW);
		m_pMainWnd->UpdateWindow();
		m_pMainWnd->SetWindowText("Game Brooder");

		// �������ں�׺ʱ�ŵ��� DragAcceptFiles��
		//  �� SDI Ӧ�ó����У���Ӧ�� ProcessShellCommand  ֮����

		((CMainFrame*)m_pMainWnd)->InitOgre();

		// ��ֱ���˳�
		
		return FALSE;
	}

#   else

	// �����汾

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

		// �����ͻ���
		// ----------------------------------------------------------				

		ghModule	= ::LoadLibrary( "GameCore.dll");
		if(!ghModule)
		{
			::MessageBox(NULL, "����GameCore.dllʧ��", "Error", MB_OK);
			return 0;
		}

		pFunGameCoreStart pFunCoreStart = (pFunGameCoreStart)::GetProcAddress( ghModule, "StartDll" );

		if(!pFunCoreStart)
		{
			::MessageBox(NULL, "�����ͻ���ʧ�ܣ�", "Error", MB_OK);
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