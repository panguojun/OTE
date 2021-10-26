#pragma once


// CCustomGen 对话框

class CCustomGen : public CDialog
{
	DECLARE_DYNAMIC(CCustomGen)

public:
	CCustomGen(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CCustomGen();

// 对话框数据
	enum { IDD = IDD_CUSTOMCREATE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	BOOL OnInitDialog();

public:

	static CCustomGen s_Inst;

	afx_msg void OnEnChangeEdit11();
	afx_msg void OnBnClickedButton2();
};
