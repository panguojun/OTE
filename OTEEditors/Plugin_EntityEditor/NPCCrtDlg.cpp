#include "stdafx.h"
#include "NPCCrtDlg.h"
#include "resource.h"
#include "EntCrtDlg.h"

// CNPCCrtDlg �Ի���
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

// CNPCCrtDlg ��Ϣ�������
void CNPCCrtDlg::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	OnOK();
}

void CNPCCrtDlg::OnEnChangeEdit1()
{
	
}

void CNPCCrtDlg::OnBnClickedButton1()
{
	if(!CEntCrtDlg::sInst.m_MouseObj)
	{
		CEntCrtDlg::sInst.m_MouseObj = SCENE_MGR->CreateEntity("��ͷ��ŮNPC.obj", "��ͷ��ŮNPC.obj");
	}
}

void CNPCCrtDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CEntCrtDlg::sInst.ClearMouseEntity();

	CDialog::OnShowWindow(bShow, nStatus);
}

