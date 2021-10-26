#pragma once
#include "afxwin.h"


// CTrackPropCtrlDlg 对话框

class CTrackPropCtrlDlg : public CDialog
{
	DECLARE_DYNAMIC(CTrackPropCtrlDlg)

public:
	CTrackPropCtrlDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CTrackPropCtrlDlg();

// 对话框数据
	enum { IDD = IDD_TRACKCTRLDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();

	virtual BOOL OnInitDialog();

public:

	static CTrackPropCtrlDlg	s_Inst;

	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton6();
	afx_msg void OnEnChangeEdit10();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton5();
	CButton m_IsInvCheck;
	CButton m_IsCurveInvCheck;
};
