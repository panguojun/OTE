// Plugin_TerrainEditor.cpp : 定义 DLL 的初始化例程。
//

#include "stdafx.h"
#include "Plugin_TerrainEditor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//	注意！
//
//		如果此 DLL 动态链接到 MFC
//		DLL，从此 DLL 导出并
//		调入 MFC 的任何函数在函数的最前面
//		都必须添加 AFX_MANAGE_STATE 宏。
//
//		例如:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// 此处为普通函数体
//		}
//
//		此宏先于任何 MFC 调用
//		出现在每个函数中十分重要。这意味着
//		它必须作为函数中的第一个语句
//		出现，甚至先于所有对象变量声明，
//		这是因为它们的构造函数可能生成 MFC
//		DLL 调用。
//
//		有关其他详细信息，
//		请参阅 MFC 技术说明 33 和 58。
//

// CPlugin_TerrainEditorApp

BEGIN_MESSAGE_MAP(CPlugin_TerrainEditorApp, CWinApp)
END_MESSAGE_MAP()


// CPlugin_TerrainEditorApp 构造

CPlugin_TerrainEditorApp::CPlugin_TerrainEditorApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CPlugin_TerrainEditorApp 对象

CPlugin_TerrainEditorApp theApp;


// CPlugin_TerrainEditorApp 初始化

BOOL CPlugin_TerrainEditorApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}
