#pragma once
#include "afxwin.h"


// CParticleCrtDlg �Ի���

class CParticleCrtDlg : public CDialog
{
	DECLARE_DYNAMIC(CParticleCrtDlg)

public:
	static CParticleCrtDlg s_Inst;

	CParticleCrtDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CParticleCrtDlg();

// �Ի�������
	enum { IDD = IDD_PARTICLE_EDIT_CRTDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

public:

	void UpdataTemplate();	
	BOOL OnInitDialog();

public:
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnLbnSelchangeList1();

public:	
	CListBox m_TempListBox;

};
