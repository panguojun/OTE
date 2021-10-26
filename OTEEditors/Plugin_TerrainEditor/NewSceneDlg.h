#pragma once


// CNewSceneDlg 对话框

class CNewSceneDlg : public CDialog
{
	DECLARE_DYNAMIC(CNewSceneDlg)

public:
	CNewSceneDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CNewSceneDlg();

// 对话框数据
	enum { IDD = IDD_CreateNew };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnChangeEdit1();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnCbnSelchangeCombo1();
};
