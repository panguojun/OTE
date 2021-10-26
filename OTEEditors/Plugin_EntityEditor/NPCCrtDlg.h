#pragma once
#include "afxwin.h"
#include "resource.h"


// CNPCCrtDlg 对话框

class CNPCCrtDlg : public CDialog
{
	DECLARE_DYNAMIC(CNPCCrtDlg)

public:
	static CNPCCrtDlg s_Inst;
	CNPCCrtDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CNPCCrtDlg();

// 对话框数据
	enum { IDD = IDD_NPC_CRT_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnEnChangeEdit1();
	CEdit m_NpcIDEdit;
	CEdit m_NpcNameEdit;
	afx_msg void OnBnClickedButton1();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};
