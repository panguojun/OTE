#pragma once


// CCustomGen �Ի���

class CCustomGen : public CDialog
{
	DECLARE_DYNAMIC(CCustomGen)

public:
	CCustomGen(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CCustomGen();

// �Ի�������
	enum { IDD = IDD_CUSTOMCREATE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	BOOL OnInitDialog();

public:

	static CCustomGen s_Inst;

	afx_msg void OnEnChangeEdit11();
	afx_msg void OnBnClickedButton2();
};
