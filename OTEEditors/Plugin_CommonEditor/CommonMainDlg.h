#pragma once
#include "resource.h"

// CCommonMainDlg �Ի���

class CCommonMainDlg : public CDialog
{
	DECLARE_DYNAMIC(CCommonMainDlg)
public:
	static CCommonMainDlg	s_Inst;

public:
	CCommonMainDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CCommonMainDlg();

// �Ի�������
	enum { IDD = IDD_COMMON_MAINDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
};
