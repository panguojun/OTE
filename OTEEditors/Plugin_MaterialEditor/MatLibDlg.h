#pragma once


// CMatLibDlg �Ի���

class CMatLibDlg : public CDialog
{
	DECLARE_DYNAMIC(CMatLibDlg)

public:
	CMatLibDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CMatLibDlg();

	BOOL OnInitDialog();

	void LoadMatLib();

// �Ի�������
	enum { IDD = IDD_MATBROWSER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedButton2();
};
