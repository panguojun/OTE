#pragma once
#include "afxwin.h"
#include <string>

// CShaderFXEditor 对话框

class CShaderFXEditor : public CDialog
{
	DECLARE_DYNAMIC(CShaderFXEditor)

public:
	CShaderFXEditor(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CShaderFXEditor();

// 对话框数据
	enum { IDD = IDD_SHADERFXEDITOR };

	void TestVS(const std::string& hlslName);
	void TestPS(const std::string& hlslName);
	
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CListBox m_ParamList;
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnLbnSelchangeList1();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton5();
};
