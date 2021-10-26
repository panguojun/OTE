#pragma once


// CMatLibDlg 对话框

class CMatLibDlg : public CDialog
{
	DECLARE_DYNAMIC(CMatLibDlg)

public:
	CMatLibDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMatLibDlg();

	BOOL OnInitDialog();

	void LoadMatLib();

// 对话框数据
	enum { IDD = IDD_MATBROWSER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedButton2();
};
