// Plugin_CommonEditor.cpp : ���� DLL �ĳ�ʼ�����̡�
//

#include "stdafx.h"
#include "Plugin_CommonEditor.h"

//#ifdef _DEBUG
//#define new DEBUG_NEW
//#endif

//
//	ע�⣡
//
//		����� DLL ��̬���ӵ� MFC
//		DLL���Ӵ� DLL ������
//		���� MFC ���κκ����ں�������ǰ��
//		��������� AFX_MANAGE_STATE �ꡣ
//
//		����:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// �˴�Ϊ��ͨ������
//		}
//
//		�˺������κ� MFC ����
//		������ÿ��������ʮ����Ҫ������ζ��
//		��������Ϊ�����еĵ�һ�����
//		���֣������������ж������������
//		������Ϊ���ǵĹ��캯���������� MFC
//		DLL ���á�
//
//		�й�������ϸ��Ϣ��
//		����� MFC ����˵�� 33 �� 58��
//

// CPlugin_CommonEditorApp

BEGIN_MESSAGE_MAP(CPlugin_CommonEditorApp, CWinApp)
END_MESSAGE_MAP()


// CPlugin_CommonEditorApp ����

CPlugin_CommonEditorApp::CPlugin_CommonEditorApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CPlugin_CommonEditorApp ����

CPlugin_CommonEditorApp theApp;


// CPlugin_CommonEditorApp ��ʼ��

BOOL CPlugin_CommonEditorApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}
