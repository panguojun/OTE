// Plugin_CommonEditor.cpp : 定义 DLL 的初始化例程。
//

#include "stdafx.h"
#include "Plugin_ParticleEditor.h"

#define OGRE_DEBUG_MEMORY_MANAGER 1
//#ifdef _DEBUG
//#define new DEBUG_NEW
//#endif

// CPlugin_CommonEditorApp

BEGIN_MESSAGE_MAP(CPlugin_ParticleEditorApp, CWinApp)
END_MESSAGE_MAP()


// CPlugin_CommonEditorApp 构造

CPlugin_ParticleEditorApp::CPlugin_ParticleEditorApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CPlugin_CommonEditorApp 对象

CPlugin_ParticleEditorApp theApp;


// CPlugin_CommonEditorApp 初始化

BOOL CPlugin_ParticleEditorApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}
