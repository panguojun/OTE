#pragma once
#include "afxwin.h"


// CSaveDlg 对话框

class CSaveDlg : public CDialog
{
	DECLARE_DYNAMIC(CSaveDlg)

public:
	CSaveDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSaveDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG2 };

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_SaveTypeCombo;
	CString m_SaveType;
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnCbnSelchangeCombo1();
};
