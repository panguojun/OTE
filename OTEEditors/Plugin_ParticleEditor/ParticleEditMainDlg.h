#pragma once
#include "afxcmn.h"
#include "resource.h"

// CParticleEditMainDlg �Ի���

class CParticleEditMainDlg : public CDialog
{
	DECLARE_DYNAMIC(CParticleEditMainDlg)

public:
	static CParticleEditMainDlg	s_Inst;
	CParticleEditMainDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CParticleEditMainDlg();

// �Ի�������
	enum { IDD = IDD_PARTICLE_EDIT_MAINDLG };

	BOOL OnInitDialog();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CTabCtrl m_TabCtrl;
	afx_msg void OnBnClickedOpen();
	afx_msg void OnBnClickedClose();
	afx_msg void OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedSave();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
};
