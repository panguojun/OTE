#pragma once


// CNewSceneDlg �Ի���

class CNewSceneDlg : public CDialog
{
	DECLARE_DYNAMIC(CNewSceneDlg)

public:
	CNewSceneDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CNewSceneDlg();

// �Ի�������
	enum { IDD = IDD_CreateNew };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnChangeEdit1();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnCbnSelchangeCombo1();
};
