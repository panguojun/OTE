#pragma once
#include "resource.h"

// CCommonMainDlg 对话框

class CCommonMainDlg : public CDialog
{
	DECLARE_DYNAMIC(CCommonMainDlg)
public:
	static CCommonMainDlg	s_Inst;

public:
	CCommonMainDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CCommonMainDlg();

// 对话框数据
	enum { IDD = IDD_COMMON_MAINDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
};
