// Plugin_ZHFileConvertor.cpp : ���� DLL �ĳ�ʼ�����̡�
//

#include "stdafx.h"
#include "Plugin_ZHFileConvertor.h"

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

// CPlugin_ZHFileConvertorApp

BEGIN_MESSAGE_MAP(CPlugin_ZHFileConvertorApp, CWinApp)
END_MESSAGE_MAP()


// CPlugin_ZHFileConvertorApp ����

CPlugin_ZHFileConvertorApp::CPlugin_ZHFileConvertorApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CPlugin_ZHFileConvertorApp ����

CPlugin_ZHFileConvertorApp theApp;


// CPlugin_ZHFileConvertorApp ��ʼ��

BOOL CPlugin_ZHFileConvertorApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}
