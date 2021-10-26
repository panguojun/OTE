#pragma once
#include "afxwin.h"


// CParticleCrtDlg 对话框

class CParticleCrtDlg : public CDialog
{
	DECLARE_DYNAMIC(CParticleCrtDlg)

public:
	static CParticleCrtDlg s_Inst;

	CParticleCrtDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CParticleCrtDlg();

// 对话框数据
	enum { IDD = IDD_PARTICLE_EDIT_CRTDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:

	void UpdataTemplate();	
	BOOL OnInitDialog();

public:
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnLbnSelchangeList1();

public:	
	CListBox m_TempListBox;

};
