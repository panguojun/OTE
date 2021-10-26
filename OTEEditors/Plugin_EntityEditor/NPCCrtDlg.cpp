#include "stdafx.h"
#include "NPCCrtDlg.h"
#include "resource.h"
#include "EntCrtDlg.h"

// CNPCCrtDlg 对话框
CNPCCrtDlg CNPCCrtDlg::s_Inst;
IMPLEMENT_DYNAMIC(CNPCCrtDlg, CDialog)
CNPCCrtDlg::CNPCCrtDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNPCCrtDlg::IDD, pParent)
{
}

CNPCCrtDlg::~CNPCCrtDlg()
{
}

void CNPCCrtDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_NpcIDEdit);
	DDX_Control(pDX, IDC_EDIT2, m_NpcNameEdit);
}

BEGIN_MESSAGE_MAP(CNPCCrtDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_EN_CHANGE(IDC_EDIT1, OnEnChangeEdit1)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()

// CNPCCrtDlg 消息处理程序
void CNPCCrtDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	OnOK();
}

void CNPCCrtDlg::OnEnChangeEdit1()
{
	
}

void CNPCCrtDlg::OnBnClickedButton1()
{
	if(!CEntCrtDlg::sInst.m_MouseObj)
	{
		CEntCrtDlg::sInst.m_MouseObj = SCENE_MGR->CreateEntity("红头发女NPC.obj", "红头发女NPC.obj");
	}
}

void CNPCCrtDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CEntCrtDlg::sInst.ClearMouseEntity();

	CDialog::OnShowWindow(bShow, nStatus);
}

