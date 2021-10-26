// CommonMainDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Plugin_CommonEditor.h"
#include "CommonMainDlg.h"

// CCommonMainDlg 对话框
CCommonMainDlg		CCommonMainDlg::s_Inst;
IMPLEMENT_DYNAMIC(CCommonMainDlg, CDialog)
CCommonMainDlg::CCommonMainDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCommonMainDlg::IDD, pParent)
{
}

CCommonMainDlg::~CCommonMainDlg()
{
}

void CCommonMainDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CCommonMainDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
END_MESSAGE_MAP()


// CCommonMainDlg 消息处理程序

void CCommonMainDlg::OnBnClickedButton1()
{
	
}
