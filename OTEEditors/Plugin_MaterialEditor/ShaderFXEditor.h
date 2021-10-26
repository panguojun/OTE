#pragma once
#include "afxwin.h"
#include <string>

// CShaderFXEditor �Ի���

class CShaderFXEditor : public CDialog
{
	DECLARE_DYNAMIC(CShaderFXEditor)

public:
	CShaderFXEditor(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CShaderFXEditor();

// �Ի�������
	enum { IDD = IDD_SHADERFXEDITOR };

	void TestVS(const std::string& hlslName);
	void TestPS(const std::string& hlslName);
	
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CListBox m_ParamList;
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnLbnSelchangeList1();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton5();
};
