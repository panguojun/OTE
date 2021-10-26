// SaveDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Plugin_EntEditor.h"
#include "SaveDlg.h"
#include "EntCrtDlg.h"

// CSaveDlg 对话框

IMPLEMENT_DYNAMIC(CSaveDlg, CDialog)
CSaveDlg::CSaveDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSaveDlg::IDD, pParent)
{
}

CSaveDlg::~CSaveDlg()
{
}

void CSaveDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_SaveTypeCombo);
}


BEGIN_MESSAGE_MAP(CSaveDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, OnBnClickedButton2)
	ON_CBN_SELCHANGE(IDC_COMBO1, OnCbnSelchangeCombo1)
END_MESSAGE_MAP()


// CSaveDlg 消息处理程序
BOOL CSaveDlg::OnInitDialog()
{
	BOOL ret = CDialog::OnInitDialog();
	if(CEntCrtDlg::sInst.m_hWnd)
	{
		m_SaveTypeCombo.SetCurSel(CEntCrtDlg::sInst.m_CrtType.GetCurSel());
		m_SaveTypeCombo.GetWindowText(m_SaveType);
	}
	return ret;
}

void CSaveDlg::OnBnClickedButton1()
{
	OnOK();
}

void CSaveDlg::OnBnClickedButton2()
{
	OnCancel();
}

void CSaveDlg::OnCbnSelchangeCombo1()
{
	m_SaveTypeCombo.GetWindowText(m_SaveType);
}
